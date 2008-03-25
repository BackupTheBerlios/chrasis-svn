// FakeOVObjects.mm: OpenVanilla Module Debugger
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

#include "FakeOVObjects.h"

// FakeOVBuffer
OVBuffer *
FakeOVBuffer::clear() {
	_buf = @"";
	[_delegate bufferContentUpdated];
	return this;
}

OVBuffer *
FakeOVBuffer::append(const char *s) {
	_buf = [_buf stringByAppendingString: [NSString stringWithUTF8String: s]];
	[_delegate bufferContentUpdated];
	return this;
}

OVBuffer *
FakeOVBuffer::send() {
	[_delegate bufferContentCommitted: this];
	_buf = @"";
	[_delegate bufferContentUpdated];
	return this;
}

OVBuffer *
FakeOVBuffer::update() {
	[_delegate bufferContentUpdated];
	return this;
}

OVBuffer *
FakeOVBuffer::update(int cursorPos, int markFrom, int markTo) {
	[_delegate bufferContentUpdated];
	return this;
}

int
FakeOVBuffer::isEmpty() {
	return [_buf isEqualToString: @""];
}

// FakeOVDictionary
int
FakeOVDictionary::keyExist(const char *key) {
	return [_dictionary objectForKey:[NSString stringWithUTF8String:key]] ? 1 : 0;
}

int
FakeOVDictionary::getInteger(const char *key) {
    NSString *s;
    return (s=[_dictionary objectForKey: [NSString stringWithUTF8String:key]]) ? [s intValue] : 0;
}

int
FakeOVDictionary::setInteger(const char *key, int value) {
    [_dictionary setObject: [NSString stringWithFormat: @"%d", value] forKey: [NSString stringWithUTF8String:key]];
    return value;
}

const char *
FakeOVDictionary::getString(const char *key) {
    NSString *s;
    return (s=[_dictionary objectForKey: [NSString stringWithUTF8String:key]]) ? [s UTF8String] : "";
}

const char *
FakeOVDictionary::setString(const char *key, const char *value) {
    [_dictionary setObject:[NSString stringWithUTF8String:value] forKey:[NSString stringWithUTF8String:key]];
    return value;
}

// FakeOVCandidate
OVCandidate*
FakeOVCandidate::clear() {
	_candidate = @"";
	[_delegate candidateContentUpdated];
	return this;
}

OVCandidate*
FakeOVCandidate::append(const char *s) {
	_candidate = [_candidate stringByAppendingString: [NSString stringWithUTF8String: s]];
	[_delegate candidateContentUpdated];
	return this;
}

OVCandidate*
FakeOVCandidate::update() {
	[_delegate candidateUpdate];
	return this;
}

// FakeOVService
const char *
FakeOVService::toUTF8(const char *encoding, const char *src) {
	return "";
}


const char *
FakeOVService::fromUTF8(const char *encoding, const char *src) {
	return "";
}
	   
const char *
FakeOVService::UTF16ToUTF8(unsigned short *src, int len) {
	return "";
}

int 
FakeOVService::UTF8ToUTF16(const char *src, unsigned short **rcvr) {
	return 0;
}