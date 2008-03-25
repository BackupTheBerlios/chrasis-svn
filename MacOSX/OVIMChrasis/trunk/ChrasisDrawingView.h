/*
 * OVIMChrasis - Chrasis OpenVanilla binding for MacOSX
 *
 * Copyright (c) 2006 Victor Tseng <palatis@gmail.com>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * $Id: character.h 53 2007-10-27 23:33:29Z palatis $
 */

#import <Cocoa/Cocoa.h>
#import <chrasis.h>

@class OVIMChrasisController;

@interface ChrasisDrawingView : NSView {
	OVIMChrasisController *controller;

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

@property(readwrite, assign) OVIMChrasisController *controller;
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
