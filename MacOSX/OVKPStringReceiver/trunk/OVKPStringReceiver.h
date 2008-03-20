// OVKPPhraseTools.h: Phrase management tool
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

#ifndef __OVKPStringReceiver_h
#define __OVKPStringReceiver_h

// #define OV_DEBUG
#include <OpenVanilla/OpenVanilla.h>
#include <OpenVanilla/OVLibrary.h>
#include <OpenVanilla/OVUtility.h>

#import <Cocoa/Cocoa.h>

#import "OVDistributedStringReceiver.h"

#define OVKPSR_NAME			"OVKPStringReceiver"
#define OVKPSR_VERSION		"0.0.1"
#define OVKPSR_NAMEVER		OVKPSR_NAME "-" OVKPSR_VERSION

#define OVKPSR_NAME_ZHTW	"\xe5\xad\x97\xe4\xb8\xb2\xe6\x8e\xa5\xe6\x94\xb6\xe5\x99\xa8" // "字串接收器"
//#define OVKPSR_NAME_ZHCN	"@o@"

class OVKPStringReceiver;

class OVKPStringReceiverContext : public OVInputMethodContext {
public:
	OVKPStringReceiverContext():
		_M_receiver(nil), _M_connection(nil)
	{ }
	virtual void start(OVBuffer*, OVCandidate*, OVService*);
	virtual void clear();
	virtual void end();
	virtual int keyEvent(OVKeyCode*, OVBuffer*, OVCandidate*, OVService*);

private:
	OVDistributedStringReceiver *_M_receiver;
	NSConnection *_M_connection;
};

class OVKPStringReceiver: public OVInputMethod {
public:
	// === from OVModule ===
	virtual const char *moduleType() { return "OVKeyPreprocessor"; }
	virtual const char *identifier() { return OVKPSR_NAMEVER; }
	virtual const char *localizedName(const char*);
	// currently this does nothing
	virtual int initialize(OVDictionary *moduleCfg, OVService *srv, const char *modulePath) { return true; }
	// === end from OVModule ===

	// === from OVInputMethod ===
	virtual OVInputMethodContext* newContext() { return new OVKPStringReceiverContext; }
	// === end from OVInputMethod ===
};
#endif
