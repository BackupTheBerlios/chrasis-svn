// OVKPStringReceiver.mm: Distributed String Receiver
//
// Copyright (c) 2004-2006 The OpenVanilla Project (http://openvanilla.org)
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

#include "OVKPStringReceiver.h"
#include "OVDistributedStringReceiver.h"
#include "OVKPStringReceiverProtocol.h"
#include <strings.h>


const char *
OVKPStringReceiver::localizedName(const char *locale)
{
	if (!strcasecmp(locale, "zh_TW"))
		return OVKPSR_NAME_ZHTW;
	//if (!strcasecmp(locale, "zh_CN"))
	//	return cininfo.scname.c_str();
	return OVKPSR_NAME;
}

void
OVKPStringReceiverContext::start(OVBuffer* b, OVCandidate* c, OVService* s)
{
	if (!_M_receiver)
		_M_receiver = [OVDistributedStringReceiver new];
	[_M_receiver setBuffer: b];

	if (!_M_connection)
	{
		_M_connection = [[NSConnection defaultConnection] retain];
		[_M_connection setRootObject: _M_receiver];
    
		if (![_M_connection registerName: OVDSTRSTRRCVR_SRVNAME]) {
			NSLog(@"Cannot register distant object service under the name %@", OVDSTRSTRRCVR_SRVNAME);
			return;
		} else {
			NSLog(@"Distant object service registered");
		}
	}
}

void
OVKPStringReceiverContext::clear()
{
	// what should i do here?
	// feel free to modify..... PLEASE!!!
	//[_M_receiver setBuffer: NULL];
}

void
OVKPStringReceiverContext::end()
{
	// what should i do? stop server?
	// feel free to modify..... PLEASE!!!
	if (_M_receiver)
		[_M_receiver setBuffer: NULL];
}

int
OVKPStringReceiverContext::keyEvent(OVKeyCode*, OVBuffer* b, OVCandidate*, OVService*)
{
	// too bad, we cannot do this elsewhere = =
	// OpenVanilla doesn't have a buffer-changed notification.
	if ([_M_receiver buffer] != NULL)
		[_M_receiver setBuffer: b];
	return 0;
}