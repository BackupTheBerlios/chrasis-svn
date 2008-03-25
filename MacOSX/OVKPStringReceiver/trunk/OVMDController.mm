// OVMDController.mm: OpenVanilla Module Debugger
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

#import "OVMDController.h"

#import <OpenVanilla/OpenVanilla.h>
#import <OpenVanilla/OVLibrary.h>
#import <OpenVanilla/OVUtility.h>

#import <dlfcn.h>

#import "FakeOVObjects.h"

@implementation OVMDController

// class specified
- (void)awakeFromNib {
	configData = [[NSMutableDictionary dictionary] retain];
	inputMethodContextPool = new std::vector< OVInputMethodContext * >();
	service = new FakeOVService((id)self);
	candidate = new FakeOVCandidate((id)self);
}

- (void)dealloc {
	[bufferPoolManager drainPool];
	
	for (std::vector< OVInputMethodContext * >::const_iterator imi = inputMethodContextPool->begin();
		 imi != inputMethodContextPool->end();
		 ++imi)
		delete *imi;
	delete inputMethodContextPool;

	[super dealloc];
}

- (void)windowWillClose: (id)sender {
	[[NSApplication sharedApplication] terminate: self];
}

// Module Tab funcitons...
- (void)loadConfigFile: (NSString *)path {
	[configData removeAllObjects];
	NSString *s = [NSString stringWithContentsOfFile: path encoding: NSUTF8StringEncoding error: nil];
	if (s) {
		NSArray *entries = [s componentsSeparatedByString: @"\n"];
		NSEnumerator *e = [entries objectEnumerator];
		NSString *tmp;
		@try {
			while (tmp = [e nextObject]) {
				NSArray *kvp = [tmp componentsSeparatedByString: @"="];
				[configData setObject: [kvp objectAtIndex: 1] forKey: [kvp objectAtIndex: 0]];
			}
		}
		@catch (NSException * e) {
			// do nothing...
		}

		NSString *confInfo = @"";
		e = [configData keyEnumerator];
		while (tmp = [e nextObject])
			confInfo = [confInfo stringByAppendingFormat: @"%@ = %@;\n", tmp, [configData objectForKey: tmp]];
		[textViewConfigFileContent selectAll: nil];
		[textViewConfigFileContent setEditable: YES];
		[textViewConfigFileContent insertText: confInfo];
		[textViewConfigFileContent setEditable: NO];
	}
}

- (IBAction)textPathChanged: (id)sender {
	switch ([sender tag]) {
		case 2: // config file path
			[self loadConfigFile: [sender stringValue]];
			break;
		default:
			// do nothing...
			break;
	}
}

- (void)fileChoosePanelDidEnd: (NSOpenPanel *)panel returnCode: (int)returnCode contextInfo: (void *)contextInfo {
	if (returnCode == NSOKButton) {
		NSString *path = [[panel filenames] lastObject];
		NSLog(@"file %@ choosen.", path);
		switch ((char *)contextInfo - (char *)NULL) {
			case 0:
				[textModuleFile setStringValue: path];
				break;
			case 1:
				[textDataDirectory setStringValue: path];
				break;
			case 2:
				[textConfigFile setStringValue: path];
				[self loadConfigFile: path];
				break;
			default:
				NSLog(@"dunno what this file is choosed for.");
		}
	}
	[panel release];
}

- (IBAction)buttonChooseFilePressed: (id)sender {
	NSOpenPanel *panel = [[NSOpenPanel openPanel] retain];
	[panel
		beginForDirectory: nil
		file: nil
		types: nil // [NSArray arrayWithObjects: @"dylib", nil]
		modelessDelegate: self
		didEndSelector: @selector(fileChoosePanelDidEnd:returnCode:contextInfo:)
		contextInfo: (void *)[sender tag]];
}

- (IBAction)buttonLoadModulePressed: (id)sender {
	NSString *path = [textModuleFile stringValue];
	void *libh = dlopen([[path stringByExpandingTildeInPath] UTF8String], RTLD_LAZY);
	if (!libh) {
		NSLog(@"failed loading library %s", path);
		return;
	}
	
	_OVGetLibraryVersion_t *g		= (_OVGetLibraryVersion_t*)		dlsym(libh, "OVGetLibraryVersion");
	_OVInitializeLibrary_t *i		= (_OVInitializeLibrary_t*)		dlsym(libh, "OVInitializeLibrary");
	_OVGetModuleFromLibrary_t *m	= (_OVGetModuleFromLibrary_t*)	dlsym(libh, "OVGetModuleFromLibrary");

	if (g && i && m) {
		// clear last loaded library
		[bufferPoolManager drainPool];
		delete candidate;
		candidate = new FakeOVCandidate((id)self);
		for (std::vector< OVInputMethodContext * >::const_iterator imi = inputMethodContextPool->begin();
			 imi != inputMethodContextPool->end();
			 ++imi)
			delete *imi;
		delete inputMethodContextPool;
		inputMethodContextPool = new std::vector< OVInputMethodContext * >();
		[bufferPoolManager setActiveBufferIndex: -1];

		// load new library
		NSString *info;
		info = [NSString stringWithFormat: @"Full path: %@\n", path];
		info = [info stringByAppendingFormat: @"Version: 0x%08x\n", g()];
		info = [info stringByAppendingFormat: @"Initialize result: %d\n", i(service, [[path stringByExpandingTildeInPath] UTF8String])];
		for(int idx=0; OVModule *module = m(idx); ++idx) {
			info = [info stringByAppendingFormat: @"Module Information #%d (0x%08x):\n", idx, module];
			info = [info stringByAppendingFormat: @"\tModuleType: %@\n",
					[NSString stringWithUTF8String: module->moduleType()]];
			info = [info stringByAppendingFormat: @"\tIdentifier: %@\n",
					[NSString stringWithUTF8String: module->identifier()]];
			info = [info stringByAppendingString: @"\tLocalized Name:\n"];
			info = [info stringByAppendingFormat: @"\t\t(default) %@\n",
					[NSString stringWithUTF8String: module->localizedName("")]];
			info = [info stringByAppendingFormat: @"\t\t(zh_TW) %@\n",
					[NSString stringWithUTF8String: module->localizedName("zh_TW")]];
			info = [info stringByAppendingFormat: @"\t\t(zh_CN) %@\n",
					[NSString stringWithUTF8String: module->localizedName("zh_CN")]];

			FakeOVDictionary cfg(configData); 
			module->initialize(&cfg, service, [[path stringByExpandingTildeInPath] UTF8String]);
			if (strcasecmp(module->moduleType(), "OVInputMethod") ||
				strcasecmp(module->moduleType(), "OVKeyPreprocessor"))
				inputMethodContextPool->push_back(static_cast< OVInputMethod * >(module)->newContext());
		}
		[textViewModuleInformation selectAll: nil];
		[textViewModuleInformation setEditable: YES];
		[textViewModuleInformation insertText: info];
		[textViewModuleInformation setEditable: NO];
	}
}

// Debugger tab
- (IBAction)textCandidatesChanged: (id)sender {
	// do nothing for now, fix lator.
}

- (IBAction)buttonNewBufferPressed: (id)sender {
	[bufferPoolManager newBuffer];
}

- (IBAction)comboSelectedBufferChanged: (id)sender {
	if ([sender indexOfSelectedItem] < 0)
		return;
	[sender setStringValue: [bufferPoolManager bufferAtIndex: [sender indexOfSelectedItem]]->getInternalBuffer()];
}

- (IBAction)buttonBroadcastEventPressed: (id)sender {
	switch ([sender tag]) {
		case 0:	// start()
			if ([comboSelectedBuffer indexOfSelectedItem] < 0)
				return;
			[bufferPoolManager setActiveBufferIndex: [comboSelectedBuffer indexOfSelectedItem]];
			for (int i = 0;i < inputMethodContextPool->size();++i)
				(*inputMethodContextPool)[i]->start([bufferPoolManager activeBuffer], candidate, service);
			return;
		case 1: // end()
			if ([comboSelectedBuffer indexOfSelectedItem] < 0)
				return;
			for (int i = 0;i < inputMethodContextPool->size();++i)
				(*inputMethodContextPool)[i]->end();
			[bufferPoolManager bufferContentUpdated];
			return;	
		case 2:	// clear()
			for (int i = 0;i < inputMethodContextPool->size();++i)
				(*inputMethodContextPool)[i]->clear();
			return;
		case 3:	// keyEvent()
			return;
		default: // ??????
			return;
	}
}

// protocol FakeOVCandidateDelegate
- (void)candidateContentUpdated {

}

- (void)candidateUpdate {
	
}

- (void)candidateHide {
	
}

- (void)candidateShow {
	
}

- (NSInteger)candidateOnScreen {
	return 0;
}

// protocol FakeOVServiceDelegate
- (void)fakeOVServiceBeep {
	
}

- (void)fakeOVServiceNotify: (NSString *)msg {
	
}

- (NSString *)fakeOVServiceLocale {
	return @"zh_TW";
}

- (NSString *)fakeOVServiceUserSpacePath: (NSString *)modid {
	return @"what?";
}
@end
