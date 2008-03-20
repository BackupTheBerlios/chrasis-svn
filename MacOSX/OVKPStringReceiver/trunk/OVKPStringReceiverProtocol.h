/*
 *  OVKPStringReceiverProtocol.h
 *  OVKPStringReceiver
 *
 *  Created by Palatis on 2008/3/21.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#define OVDSTRSTRRCVR_SRVNAME @"OVKPStringReceiver"

@protocol OVDistributedStringReceiverProtocol
- (BOOL) ping;
- (void) aboutOpenVanillaDialog;
- (void) sendStringToCurrentComposingBuffer: (NSString *)string;
- (void) sendCharacterToCurrentComposingBuffer: (NSString *)string;
- (void) quitStringReceiver;
@end