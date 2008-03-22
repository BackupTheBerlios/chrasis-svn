//
//  ChrasisDrawingView.h
//  OVIMChrasis
//
//  Created by Palatis on 2008/3/17.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Chrasis/chrasis.h>

@class LCMainController;

@interface LCDrawingView : NSView {
	LCMainController *controller;

	NSPopUpButton *popupCandidateList;
	NSMenuItem *popupCandidateListItemClear;
	NSMenuItem *popupCandidateListItemIncorrect;

	CGFloat dash[2];

	chrasis::Character *character;
	
	NSTimer *recognizeTimer;
}

@property(readwrite, assign) NSPopUpButton *popupCandidateList;
@property(readwrite, assign) NSMenuItem *popupCandidateListItemClear;
@property(readwrite, assign) NSMenuItem *popupCandidateListItemIncorrect;

@property(readwrite, assign) LCMainController *controller;
@property(readwrite, assign) NSTimer *recognizeTimer;

- (BOOL)mouseDownCanMoveWindow;
- (BOOL)isFlipped;
- (BOOL)acceptsFirstResponder;

- (void)awakeFromNib;

- (void)clearCharacterData;
- (void)learnCharacter: (NSString *)name;
- (void)updateCandidateList;

- (void)recognizeTimerTick: (NSTimer *)timer;
- (void)mouseDown: (NSEvent *)event;
- (void)mouseUp: (NSEvent *)event;

@end
