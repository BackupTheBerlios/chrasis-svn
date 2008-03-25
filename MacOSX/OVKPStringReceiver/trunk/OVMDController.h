// OVMDController.h: OpenVanilla Module Debugger
//
// Copyright (c) 2008 The Chrasis Project (http://chrasis.blogspot.com)
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of OpenVanilla nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#import <Cocoa/Cocoa.h>
#import <OpenVanilla/OpenVanilla.h>
#import <OpenVanilla/OVLibrary.h>
#import <OpenVanilla/OVUtility.h>

#include <vector>

#import "OVMDBufferManager.h"
#import "FakeOVObjects.h"

@interface OVMDController : NSObject <FakeOVCandidateDelegate, FakeOVServiceDelegate> {
	// Module tab
	IBOutlet NSTextField *textModuleFile;
	IBOutlet NSTextField *textDataDirectory;
	IBOutlet NSTextField *textConfigFile;
	IBOutlet NSTextView *textViewModuleInformation;
	IBOutlet NSTextView *textViewConfigFileContent;
	
	// Debugger Tab
	IBOutlet NSTextField *textOutput;
	IBOutlet NSTextField *textCandidates;
	IBOutlet NSComboBox *comboSelectedBuffer;
	IBOutlet NSTextField *textKeyCode;

	// Other objects
	IBOutlet OVMDBufferPoolManager *bufferPoolManager;

	// non-nib objects
	NSMutableDictionary *configData;
	std::vector< OVInputMethodContext * > *inputMethodContextPool;
	
	FakeOVService *service;
	FakeOVCandidate *candidate;
}

// Module tab
- (IBAction)textPathChanged: (id)sender;
- (IBAction)buttonChooseFilePressed: (id)sender;
- (IBAction)buttonLoadModulePressed: (id)sender;

// Debugger tab
- (IBAction)textCandidatesChanged: (id)sender;
- (IBAction)buttonNewBufferPressed: (id)sender;
- (IBAction)comboSelectedBufferChanged: (id)sender;
- (IBAction)buttonBroadcastEventPressed: (id)sender;

@end
