//
//  ChrasisCandidateListPopupButton.h
//  OVIMChrasis
//
//  Created by Palatis on 2008/3/18.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDrawingView.h"

@class LCMainController;

@interface LCCandidateListPopUpButton : NSPopUpButton {
	LCMainController *controller;
	LCDrawingView *associatedDrawingView;
}

@property(readwrite, assign) LCMainController *controller;
@property(readwrite, assign) LCDrawingView *associatedDrawingView;

- (void)mouseDown: (NSEvent *)event;

@end
