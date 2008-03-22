//
//  LeopardChrasisController.h
//  LeopardChrasis
//
//  Created by Palatis on 2008/3/22.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <InputMethodKit/InputMethodKit.h>

@interface LCInputController : IMKInputController {

}

- (void)awakeFromNib;

+ (void)doSendString: (NSString *)str;

@end
