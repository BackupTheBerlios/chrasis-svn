//
//  ChrasisCandidateListPopupButton.m
//  OVIMChrasis
//
//  Created by Palatis on 2008/3/18.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "ChrasisCandidateListPopUpButton.h"
#import "OVIMChrasisController.h"

@implementation ChrasisCandidateListPopUpButton

@synthesize controller;
@synthesize associatedDrawingView;

- (void)mouseDown: (NSEvent *)event {
	[controller unregisterRecognizeTimer: associatedDrawingView];
	
	if ([[self menu] numberOfItems] != 0)
		[super mouseDown: event];
}

- (void)menuDidClose: (NSMenu *)menu {
	[controller registerRecognizeTimer: associatedDrawingView];
}

@end
