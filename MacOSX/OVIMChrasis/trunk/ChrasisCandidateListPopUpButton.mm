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
