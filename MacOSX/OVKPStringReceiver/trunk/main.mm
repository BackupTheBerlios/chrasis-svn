//
//  main.mm
//  OVKPStringReceiver
//
//  Created by Palatis on 2008/3/21.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "OVKPStringReceiver.h"

extern "C" {
	unsigned int OVGetLibraryVersion() { return OV_VERSION; }
	
	int OVInitializeLibrary(OVService*, const char*) { return 1; }
	
	OVModule *OVGetModuleFromLibrary(int idx)
	{
        return (idx==0) ? new OVKPStringReceiver() : NULL;
    }
}