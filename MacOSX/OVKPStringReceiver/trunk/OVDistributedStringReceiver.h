//
//  OVDistributedStringReceive.h
//  OVKPStringReceiver
//
//  Created by Palatis on 2008/3/21.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenVanilla/OpenVanilla.h>

@interface OVDistributedStringReceiver : NSObject {
	OVBuffer *buffer;
}
@property(readwrite, assign) OVBuffer *buffer;

- (BOOL) ping;
- (void) aboutOpenVanillaDialog;
- (void) sendStringToCurrentComposingBuffer: (NSString *)string;
- (void) sendCharacterToCurrentComposingBuffer: (NSString *)string;
- (void) quitStringReceiver;

@end
