//
//  ChrasisCandidateListPopupButton.h
//  OVIMChrasis
//
//  Created by Palatis on 2008/3/18.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "ChrasisDrawingView.h"

@class OVIMChrasisController;

@interface ChrasisCandidateListPopUpButton : NSPopUpButton {
	OVIMChrasisController *controller;
	ChrasisDrawingView *associatedDrawingView;
}

@property(readwrite, assign) OVIMChrasisController *controller;
@property(readwrite, assign) ChrasisDrawingView *associatedDrawingView;

- (void)mouseDown: (NSEvent *)event;

@end
