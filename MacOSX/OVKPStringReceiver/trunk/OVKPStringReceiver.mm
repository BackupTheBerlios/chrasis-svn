// OVKPStringReceiver.mm: Distributed String Receiver
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

#include "OVKPStringReceiver.h"
#include "OVDistributedStringReceiver.h"
#include "OVKPDistributedStringReceiverProtocol.h"
#include <strings.h>


const char *
OVKPStringReceiver::localizedName(const char *locale)
{
	if (!strcasecmp(locale, "zh_TW"))
		return OVKPSR_NAME_ZHTW;
	//if (!strcasecmp(locale, "zh_CN"))
	//	return OVKPSR_NAME_ZHCN;
	return OVKPSR_NAME;
}

OVKPStringReceiverContextSingleton::OVKPStringReceiverContextSingleton()
{
	murmur("OVKPStringReceiverContextSingleton::OVKPStringReceiverContextSingleton()");
	_M_receiver = [[OVDistributedStringReceiver alloc] init];
	
	NSMachPort *port = [NSMachPort new];
	_M_connection = [[NSConnection connectionWithReceivePort: port sendPort: port] retain];
	[_M_connection setRootObject: _M_receiver];

	if (![_M_connection registerName: OVDSTRSTRRCVR_SRVNAME]) {
		murmur("    CANNOT register DO, name: %@.", [OVDSTRSTRRCVR_SRVNAME UTF8String]);
		return;
	}
	murmur("    DO service %@ registered.", [OVDSTRSTRRCVR_SRVNAME UTF8String]);
}

OVKPStringReceiverContextSingleton::~OVKPStringReceiverContextSingleton()
{
	[[NSPortNameServer systemDefaultPortNameServer] removePortForName: OVDSTRSTRRCVR_SRVNAME];
	[_M_connection invalidate];
	[_M_connection release];
	[_M_receiver release];
	murmur("OVKPStringReceiverContextSingleton::~OVKPStringReceiverContext()");
}

void
OVKPStringReceiverContextSingleton::start(OVBuffer* b, OVCandidate* c, OVService* s)
{
	murmur("OVKPStringReceiverContextSingleton::start(0x%08x, 0x%08x, 0x%08x)", b, c, s);
	[_M_receiver pushBuffer: b];
}

void
OVKPStringReceiverContextSingleton::end()
{
	murmur("OVKPStringReceiverContextSingleton::end()");
	[_M_receiver popBuffer];
}