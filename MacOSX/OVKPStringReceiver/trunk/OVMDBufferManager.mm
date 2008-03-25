// OVMDBufferManager.mm: OpenVanilla Module Debugger
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

#import "OVMDBufferManager.h"

@implementation OVMDBufferPoolManager

@synthesize activeBufferIndex;

- (void)awakeFromNib {
	pool = new std::vector< FakeOVBuffer * >();
	activeBufferIndex = -1;
}

- (void)dealloc {
	[self drainPool];
	delete pool;
	[super dealloc];
}

- (void)newBuffer {
	pool->push_back( new FakeOVBuffer( (id)self ) );
	[self bufferContentUpdated];
}

- (void)drainPool {
	for (std::vector< FakeOVBuffer * >::const_iterator pi = pool->begin();
		 pi != pool->end();
		 ++pi)
		delete *pi;
	delete pool;
	pool = new std::vector< FakeOVBuffer * >();
	[self bufferContentUpdated];
}

- (FakeOVBuffer *)bufferAtIndex: (NSInteger)idx {
	return (*pool)[idx];
}

- (NSUInteger)bufferCount {
	return pool->size();
}

- (void)setActiveBufferIndex: (NSInteger)idx {
	activeBufferIndex = idx;
	[self bufferContentUpdated];
}

- (FakeOVBuffer *)activeBuffer {
	return (*pool)[activeBufferIndex];
}

// protocol FakeOVBufferDelegate
- (void)bufferContentUpdated {
	[comboSelectedBuffer removeAllItems];
	for (int i = 0;i < pool->size();++i) {
		NSString *s = [NSString stringWithFormat: @"0x%08x: %@", (*pool)[i], (*pool)[i]->getInternalBuffer()];
		if (i == activeBufferIndex)
			s = [s stringByAppendingString: @" *"];
		[comboSelectedBuffer addItemWithObjectValue: s];
	}
	@try {
		[comboSelectedBuffer selectItemAtIndex: activeBufferIndex];
		[comboSelectedBuffer setStringValue: (*pool)[activeBufferIndex]->getInternalBuffer()];
	}
	@catch (NSException * e) {
		[comboSelectedBuffer deselectItemAtIndex: [comboSelectedBuffer indexOfSelectedItem]];
		[comboSelectedBuffer setStringValue: @""];
	}
	
	NSLog(@"selected item: %d", [comboSelectedBuffer indexOfSelectedItem]);
}

- (void)bufferContentCommitted: (FakeOVBuffer *)buffer {
	if ((*pool)[activeBufferIndex] == buffer)
		[textOutput setStringValue: [[textOutput stringValue] stringByAppendingString: buffer->getInternalBuffer()]];
	[self bufferContentUpdated];
}

@end
