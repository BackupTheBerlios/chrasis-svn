//
//  OVDistributedStringReceive
//  OVKPStringReceiver
//
//  Created by Palatis on 2008/3/21.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <OpenVanilla/OpenVanilla.h>

#import "OVDistributedStringReceiver.h"


@implementation OVDistributedStringReceiver

@synthesize buffer;

- (BOOL) ping { return YES; }

- (void) aboutOpenVanillaDialog {
	//we can't show the dialog yet.
}

- (void) sendStringToCurrentComposingBuffer: (NSString *)str {
	if (buffer)
		buffer->append([str UTF8String])->update()->send();
}

- (void) sendCharacterToCurrentComposingBuffer: (NSString *)str {
	// how should we implement this?
}

- (void) quitStringReceiver {
	// how should we implement this?
}
@end
