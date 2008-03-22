//
//  ChrasisDrawingView.m
//  OVIMChrasis
//
//  Created by Palatis on 2008/3/17.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LCDrawingView.h"
#import "LCMainController.h"
#import "LCInputController.h"

#import <fstream>

@implementation LCDrawingView

@synthesize popupCandidateList;
@synthesize popupCandidateListItemClear;
@synthesize popupCandidateListItemIncorrect;

@synthesize controller;
@synthesize recognizeTimer;

- (BOOL)mouseDownCanMoveWindow { return NO; }
- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)isFlipped { return YES; }

- (void)awakeFromNib {
	character = new chrasis::Character();
}

- (void)clearCharacterData {
	[popupCandidateList removeAllItems];

	delete character;
	character = new chrasis::Character();

	[self setNeedsDisplay: true];
}

- (void)learnCharacter: (NSString *)name {
	if ([name UTF8String] != nil)
	{
		chrasis::Character nc(normalize(*character));
		nc.set_name([name UTF8String]);
		chrasis::learn(nc);
	}
}

- (void)updateCandidateList {
	[popupCandidateList removeAllItems];

	chrasis::platform::initialize_userdir();
	chrasis::ItemPossibilityList likely(recognize(normalize(*character)));
	likely.sort(chrasis::ItemPossibilityList::SORTING_POSSIBILITY);

	for (chrasis::ItemPossibilityList::const_iterator li = likely.begin();
		 li != likely.end();
		 ++li)
		[popupCandidateList addItemWithTitle: [NSString stringWithUTF8String: li->name.c_str()]];

	[[popupCandidateList menu] addItem: [NSMenuItem separatorItem]];
	popupCandidateListItemClear = [[popupCandidateList menu] addItemWithTitle: NSLocalizedString(@"Clear", @"Candidate List MenuItem - Clear") action: nil keyEquivalent: @""];
	popupCandidateListItemIncorrect = [[popupCandidateList menu] addItemWithTitle: NSLocalizedString(@"Incorrect", @"Candidate List MenuItem - Incorrect") action: nil keyEquivalent: @""];
}

- (void) recognizeTimerTick: (NSTimer *)timer {
	[LCInputController doSendString: [popupCandidateList titleOfSelectedItem]];
	recognizeTimer = nil;

	if ([controller shouldLearnRecognized])
		[self learnCharacter: [popupCandidateList titleOfSelectedItem]];

	if ([controller shouldSaveWrittenCharacter])
	{
		if ([[popupCandidateList titleOfSelectedItem] UTF8String] != nil)
		{
			NSString *filename = [[NSString stringWithCString: getenv("HOME")] stringByAppendingString: @"/.chrasis/OVIMChrasis-saved-characters.chml"];
			std::ofstream fout([filename UTF8String], std::ios_base::out | std::ios_base::app);

			fout << "<character name=\"" << [[popupCandidateList titleOfSelectedItem] UTF8String]<< "\">" << std::endl;
			for (chrasis::Character::Stroke::const_iterator si = character->strokes_begin();
				 si != character->strokes_end();
				 ++si)
			{
				fout << "  <stroke>" << std::endl;
				for (chrasis::Character::Stroke::Point::const_iterator pi = si->points_begin();
					 pi != si->points_end();
					 ++pi)
					fout << "    <point x=\"" << pi->x() << "\" y=\"" << pi->y() << "\"/>" << std::endl;
				fout << "  </stroke>" << std::endl;
			}
			fout << "</character>" << std::endl;
			fout.close();
		}
	}

	[self clearCharacterData];
}

- (void)mouseDown: (NSEvent *)event {
	[controller unregisterRecognizeTimer: self];

	NSPoint p = [self convertPoint: [event locationInWindow] fromView: nil];
	character->new_stroke();
	character->add_point(p.x, p.y);

	[self setNeedsDisplay: true];
}

- (void)mouseUp: (NSEvent *)event {
	NSPoint p = [self convertPoint: [event locationInWindow] fromView: nil];
	character->add_point(p.x, p.y);

	// put recognize code here
	[self updateCandidateList];

	[self setNeedsDisplay: true];

	[controller registerRecognizeTimer: self];
}

- (void)mouseDragged: (NSEvent *)event {
	NSPoint p = [self convertPoint: [event locationInWindow] fromView: nil];
	character->add_point(p.x, p.y);

	[self setNeedsDisplay: true];
}

- (id)initWithFrame: (NSRect)frame {
    self = [super initWithFrame: frame];
    if (self) {
        // Initialization code here.
		dash[0] = 4.0;
		dash[1] = 3.0;
    }
    return self;
}

- (void)drawRect: (NSRect)rect {
	// Drawing code here.
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path setLineCapStyle: NSRoundLineCapStyle];
	[path setLineJoinStyle: NSRoundLineJoinStyle];

	[path setLineWidth: 1];
	[path setLineDash: self->dash count: 2 phase: 0.0];
	[path moveToPoint: NSMakePoint(0.0,								rect.size.height / 2)];
	[path lineToPoint: NSMakePoint(rect.origin.x + rect.size.width,	rect.size.height / 2)];
	[path moveToPoint: NSMakePoint(rect.size.width / 2,				0.0)];
	[path lineToPoint: NSMakePoint(rect.size.width / 2,				rect.origin.y + rect.size.height)];
	[[NSColor darkGrayColor] set];
	[path stroke];

	[path removeAllPoints];
	[path setLineWidth: 2];
	[path setLineDash: nil count: 0 phase: 0.0];
	for (chrasis::Character::Stroke::const_iterator si = character->strokes_begin();
		 si != character->strokes_end();
		 ++si)
	{
		[path moveToPoint: NSMakePoint(si->points_begin()->x(), si->points_begin()->y())];
		for (chrasis::Character::Stroke::Point::const_iterator pi = si->points_begin();
			 pi != si->points_end();
			 ++pi)
			[path lineToPoint: NSMakePoint(pi->x(), pi->y())];
	}
	[[NSColor whiteColor] set];
	[path stroke];

	[path removeAllPoints];
	[path setLineWidth: 1];
	[path appendBezierPathWithRoundedRect: rect xRadius: 6 yRadius: 6];
	[[NSColor lightGrayColor] set];
	[path stroke];
}

@end
