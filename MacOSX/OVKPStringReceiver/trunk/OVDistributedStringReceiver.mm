// OVDistributedStringReceiver.mm: Distributed String Receiver
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

#import <OpenVanilla/OpenVanilla.h>
#import <OpenVanilla/OVLibrary.h>
#import <OpenVanilla/OVUtility.h>

#import "OVDistributedStringReceiver.h"

#include <deque>
#include <set>

@implementation OVDistributedStringReceiver

- (void)init {
	_bufSet = new std::set< OVBuffer * >();
	_bufQueue = new std::deque< OVBuffer * >();
	[super init];
}

- (void)dealloc {
	delete _bufSet;
	delete _bufQueue;
	[super dealloc];
}

- (void)pushBuffer: (OVBuffer *)buffer {
	if (_bufSet->find(buffer) == _bufSet->end())
		_bufQueue->push_back(buffer);
	_bufSet->insert(buffer);
}

- (void)popBuffer {
	if (!_bufQueue->empty()) {
		_bufSet->erase(*(_bufQueue->begin()));
		_bufQueue->pop_front();
	}
}

// protocol OVKPDistributedStringReceiverProtocol
- (BOOL) ping {
	murmur("OVDistributedStringReceiver: pong!");
	return YES;
}

- (void) sendStringToCurrentComposingBuffer: (NSString *)str {
	murmur([[NSString stringWithFormat: @"OVDistributedStringReceiver: send string: %@", str] UTF8String]);
	for (std::set< OVBuffer * >::const_iterator ci = _bufSet->begin();
		 ci != _bufSet->end();
		 ++ci)
		(*ci)->append([str UTF8String])->update()->send();
}

- (void) lowLevelBufferAppend: (NSString *)str {
	for (std::set< OVBuffer * >::const_iterator ci = _bufSet->begin();
		 ci != _bufSet->end();
		 ++ci)
		(*ci)->append([str UTF8String]);
}

- (void) lowLevelBufferUpdate {
	for (std::set< OVBuffer * >::const_iterator ci = _bufSet->begin();
		 ci != _bufSet->end();
		 ++ci)
		(*ci)->update();
}

- (void) lowLevelBufferUpdateWithPos: (NSInteger)cursorPos markFrom: (NSInteger)fromPos markTo: (NSInteger)toPos {
	for (std::set< OVBuffer * >::const_iterator ci = _bufSet->begin();
		 ci != _bufSet->end();
		 ++ci)
		(*ci)->update(cursorPos, fromPos, toPos);
}

- (void) lowLevelBufferSend {
	for (std::set< OVBuffer * >::const_iterator ci = _bufSet->begin();
		 ci != _bufSet->end();
		 ++ci)
		(*ci)->send();
}
@end
