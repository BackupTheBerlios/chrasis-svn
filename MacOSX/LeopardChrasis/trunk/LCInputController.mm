//
//  LeopardChrasisController.mm
//  LeopardChrasis
//
//  Created by Palatis on 2008/3/22.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LCInputController.h"


@implementation LCInputController

NSMutableSet *_senders = nil;

- (void)alloc {
	NSLog(@"LeopardChrasisController: alloc.");
}

- (void)awakeFromNib {
	NSLog(@"LeopardChrasisController: awake from nib.");
}

// === start IMKServer implementation ===
- (void)activateServer: (id)sender {
	if (!_senders)
		_senders = [[NSMutableSet new] retain];
	NSLog(@"LCInputController: activate server with sender %@.", sender);
	[_senders addObject: sender];
	NSLog(@"Sender stack: %@", _senders);
}

- (void)deactivateServer: (id)sender {
	NSLog(@"LCInputController: deactivate server with sender %@.", sender);
	[_senders removeObject: sender];
	NSLog(@"Sender stack: %@", _senders);
}

-(NSMenu*)menu
{
	NSLog(@"LCInputController: asking for menu");
	NSMenu *_menu = [[NSMenu new] autorelease];
	NSMenuItem *_item = [_menu 
		addItemWithTitle: NSLocalizedString(@"Show Writing Pad", @"MenuItem to show the writing pad")
		action: @selector(showWritingPad:)
		keyEquivalent: @""];
	[_item setTarget: self];
	return _menu;
}

- (void)showWritingPad: (id)sender {
	NSLog(@"LCInputController: ask delegate to show the writing pad...");
	[[NSApp delegate] showWritingPad: self];
}

+ (void)doSendString: (NSString *)str {
	NSLog(@"LCInputController: broadcasting string %@...", str);
	NSEnumerator *e = [_senders objectEnumerator];
	id s;
	while (s = [e nextObject]) {
		NSLog(@"    sending string to %@", s);
		[s insertText:str replacementRange:NSMakeRange(NSNotFound, NSNotFound)];
	}
}
// === end IMKServer implementation ===

@end
