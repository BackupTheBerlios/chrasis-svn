// FakeOVObjects.h: OpenVanilla Module Debugger
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

class FakeOVKeyCode;
class FakeOVBuffer;
class FakeOVCandidate;
class FakeOVDictionary;
class FakeOVService;

enum {
	fovkShift		= 1,
	fovkCapslock	= 2,
	fovkCtrl		= 4,
	fovkAlt			= 8,
	fovkOpt			= 16,
	fovkCommand		= 32,
	fovkNum			= 64
};

class FakeOVKeyCode: public OVKeyCode {
public:
	FakeOVKeyCode(int c, int m):
		_keyCode(c), _modifier(m)
	{}

    virtual int code()			{ return _keyCode; }
    virtual int isShift()		{ return _modifier & fovkShift; }
    virtual int isCapslock()	{ return _modifier & fovkCapslock; }    
    virtual int isCtrl()		{ return _modifier & fovkCtrl; }
    virtual int isAlt()			{ return _modifier & fovkAlt; }
    virtual int isOpt()			{ return _modifier & fovkOpt; }
    virtual int isCommand()		{ return _modifier & fovkCommand; }
    virtual int isNum()			{ return _modifier & fovkNum; }
    //virtual int isFunctionKey() {
    //    return isCtrl() || isAlt() || isOpt() || isCommand() || isNum();
    //}
private:
	int _keyCode;
	int _modifier;
};


@protocol FakeOVBufferDelegate
- (void)bufferContentUpdated;
- (void)bufferContentCommitted: (FakeOVBuffer *)buffer;
@end

class FakeOVBuffer: public OVBuffer {
public:
	FakeOVBuffer(id <FakeOVBufferDelegate> d):
		_buf([[NSString new] autorelease]), _delegate(d)
	{}

    virtual OVBuffer* clear();
    virtual OVBuffer* append(const char *s);
    virtual OVBuffer* send();
    virtual OVBuffer* update();
    virtual OVBuffer* update(int cursorPos, int markFrom=-1, int markTo=-1);
    virtual int isEmpty();

	NSString *getInternalBuffer() { return _buf; }

private:
	NSString *_buf;
	id <FakeOVBufferDelegate> _delegate;
};

@protocol FakeOVCandidateDelegate
- (void)candidateContentUpdated;
- (void)candidateUpdate;
- (void)candidateHide;
- (void)candidateShow;
- (NSInteger)candidateOnScreen;
@end

class FakeOVCandidate: public OVCandidate {
public:
	FakeOVCandidate(id <FakeOVCandidateDelegate> d):
		_candidate([[NSString new] autorelease]), _delegate(d)
	{}

    virtual OVCandidate* clear();
    virtual OVCandidate* append(const char *s);
    virtual OVCandidate* hide() { [_delegate candidateHide]; return this; }
    virtual OVCandidate* show() { [_delegate candidateHide]; return this; }
    virtual OVCandidate* update();
    virtual int onScreen() { return [_delegate candidateOnScreen]; };
	
	NSString *getInternalCandidate() { return _candidate; }

private:
	NSString *_candidate;
	id <FakeOVCandidateDelegate> _delegate;
};

class FakeOVDictionary: public OVDictionary {
public:
	FakeOVDictionary(NSMutableDictionary *dict):
		_dictionary([dict retain])
	{}
	~FakeOVDictionary() { [_dictionary release]; }
    virtual int keyExist(const char *key);
    virtual int getInteger(const char *key);
    virtual int setInteger(const char *key, int value);
    virtual const char * getString(const char *key) ;
    virtual const char * setString(const char *key, const char *value);
    //virtual int getIntegerWithDefault(const char *key, int value) {
    //    if (!keyExist(key)) setInteger(key, value); return getInteger(key);
    //}
    //virtual const char* getStringWithDefault(const char *key, const char *value) {
    //    if (!keyExist(key)) setString(key, value); return getString(key);
    //}

private:
	NSMutableDictionary *_dictionary;
};

@protocol FakeOVServiceDelegate
- (void)fakeOVServiceBeep;
- (void)fakeOVServiceNotify: (NSString *)msg;
- (NSString *)fakeOVServiceLocale;
- (NSString *)fakeOVServiceUserSpacePath: (NSString *)modid;
@end

class FakeOVService: public OVService {
public:
	FakeOVService(id <FakeOVServiceDelegate> d):
		_delegate(d)
	{}
	
    virtual void beep()
	{ [_delegate fakeOVServiceBeep]; }
    virtual void notify(const char *msg)
	{ [_delegate fakeOVServiceNotify: [NSString stringWithUTF8String: msg]]; }
    virtual const char *locale()
	{ return [[_delegate fakeOVServiceLocale] UTF8String]; }
    virtual const char *userSpacePath(const char *modid)
	{ return [[_delegate fakeOVServiceUserSpacePath: [NSString stringWithUTF8String: modid]] UTF8String]; }
    virtual const char *pathSeparator()
	{ return "/"; }

    // limited support for encoding conversion
    virtual const char *toUTF8(const char *encoding, const char *src);
    virtual const char *fromUTF8(const char *encoding, const char *src);
    
    // the endian order of these two functions is always that of machine order
    // (ie. on a big endian machine it's UTF16BE, otherwise UTF16LE)
    // no byte order mark (0xfffe or 0xfeff) is ever used here) 
    // also note no boundry check mechanism exists, the Loader takes care 
    // about it
    virtual const char *UTF16ToUTF8(unsigned short *src, int len);
    virtual int UTF8ToUTF16(const char *src, unsigned short **rcvr);

private:
	id <FakeOVServiceDelegate> _delegate;
};