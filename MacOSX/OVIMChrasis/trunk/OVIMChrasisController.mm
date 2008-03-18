//
//  OVIMChrasisController.m
//  OVIMChrasis
//
//  Created by Palatis on 2008/3/16.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "OVIMChrasisController.h"


@implementation OVIMChrasisController

- (BOOL) shouldLearnRecognized { return learn_recognized; }
- (BOOL) shouldSaveWrittenCharacter { return save_chml; }

- (float) titleBarHeight
{
    NSRect frame = NSMakeRect (0, 0, 100, 100);
    NSRect contentRect = [NSWindow contentRectForFrameRect: frame styleMask: NSTitledWindowMask];
    return (frame.size.height - contentRect.size.height);
}

- (void) initializeWritingAreas {
	int i;
	for (i=0;i<8;++i) {
		ChrasisDrawingView *view = [view_writing_area_array objectAtIndex: i];
		ChrasisCandidateListPopUpButton *popup = [popup_candidate_list_array objectAtIndex: i];
		[view setHidden: YES];
		[popup setHidden: YES];
	}

	// resize and put them in position
	for (i=0;i<num_writing_areas;++i)
	{
		ChrasisDrawingView *view = [view_writing_area_array objectAtIndex: i];
		ChrasisCandidateListPopUpButton *popup = [popup_candidate_list_array objectAtIndex: i];
		[popup setHidden: NO];
		[view setHidden: NO];
	}
	CGFloat popup_height = [popup_candidate_list_1 frame].size.height;
	[panel_writing_pad
		setFrame: NSMakeRect(
			[panel_writing_pad frame].origin.x,
			[panel_writing_pad frame].origin.y,
			(writing_area_size + 2) * num_writing_areas + 1 + [view_keypad frame].size.width,
			[self titleBarHeight] + popup_height + writing_area_size + 4)
		display: YES animate: YES];
	CGFloat popup_y = [popup_candidate_list_1 frame].origin.y;
	for (i=0;i<num_writing_areas;++i) {
		ChrasisDrawingView *view = [view_writing_area_array objectAtIndex: i];
		ChrasisCandidateListPopUpButton *popup = [popup_candidate_list_array objectAtIndex: i];
		[popup setFrame: NSMakeRect((writing_area_size + 2) * i + 4, popup_y, writing_area_size, popup_height)];
		[view setFrame: NSMakeRect((writing_area_size + 2) * i + 4, 5, writing_area_size, writing_area_size)];
	}
}

- (void)awakeFromNib {
	_displayServer = [[NSConnection rootProxyForConnectionWithRegisteredName:OVDSPSRVR_NAME host:nil] retain];	
	if (!_displayServer) {
		NSLog(@"cannot find display server");
		[[NSApplication sharedApplication] terminate:self];
	}
	
	[_displayServer setProtocolForProxy:@protocol(CVDisplayServerPart)];

	[panel_writing_pad setCollectionBehavior: NSWindowCollectionBehaviorCanJoinAllSpaces];
	[panel_writing_pad setLevel: NSScreenSaverWindowLevel];
	[panel_options setLevel: NSScreenSaverWindowLevel + 1];
	[panel_learning setLevel: NSScreenSaverWindowLevel + 1];
	
	[panel_options center];
	[panel_learning center];

	NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
	num_writing_areas = [standardUserDefaults integerForKey: @"num_writing_areas"];
	writing_area_size = [standardUserDefaults integerForKey: @"writing_area_size"];
	recognize_delay = [standardUserDefaults integerForKey: @"recognize_delay"];
	save_chml = [standardUserDefaults boolForKey: @"save_chml"];
	learn_recognized = [standardUserDefaults boolForKey: @"learn_recognized"];
	NSPoint winpos = NSMakePoint(
		[standardUserDefaults floatForKey: @"writing_pad_x"],
		[standardUserDefaults floatForKey: @"writing_pad_y"]);
	
	// default values
	if (num_writing_areas == 0)
		num_writing_areas = 4;
	if (writing_area_size == 0)
		writing_area_size = 120;
	if (recognize_delay == 0)
		recognize_delay = 3500;
	if (winpos.x == 0.0 && winpos.y == 0.0)
		[panel_writing_pad center];
	else
		[panel_writing_pad setFrameOrigin: winpos];

	// range checks:
	if (num_writing_areas < 1)
		num_writing_areas = 1;
	else if (num_writing_areas > 8)
		num_writing_areas = 8;
	
	if (writing_area_size < 50)
		writing_area_size = 50;
	else if (writing_area_size > 500)
		writing_area_size = 500;
	
	if (recognize_delay < 100)
		recognize_delay = 100;
	else if (recognize_delay > 60000)
		recognize_delay = 60000;

	view_writing_area_array = [[NSMutableArray array] retain];
	[view_writing_area_array addObject: view_writing_area_1];
	[view_writing_area_array addObject: view_writing_area_2];
	[view_writing_area_array addObject: view_writing_area_3];
	[view_writing_area_array addObject: view_writing_area_4];
	[view_writing_area_array addObject: view_writing_area_5];
	[view_writing_area_array addObject: view_writing_area_6];
	[view_writing_area_array addObject: view_writing_area_7];
	[view_writing_area_array addObject: view_writing_area_8];
	popup_candidate_list_array = [[NSMutableArray array] retain];
	[popup_candidate_list_array addObject: popup_candidate_list_1];
	[popup_candidate_list_array addObject: popup_candidate_list_2];
	[popup_candidate_list_array addObject: popup_candidate_list_3];
	[popup_candidate_list_array addObject: popup_candidate_list_4];
	[popup_candidate_list_array addObject: popup_candidate_list_5];
	[popup_candidate_list_array addObject: popup_candidate_list_6];
	[popup_candidate_list_array addObject: popup_candidate_list_7];
	[popup_candidate_list_array addObject: popup_candidate_list_8];

	[self initializeWritingAreas];

	int i;
	for (i=0;i<8;++i) {
		ChrasisDrawingView *view = [view_writing_area_array objectAtIndex: i];
		ChrasisCandidateListPopUpButton *popup = [popup_candidate_list_array objectAtIndex: i];
		[view setPopupCandidateList: popup];
		[popup setAssociatedDrawingView: view];
		[view setController: self];
		[popup setController: self];
		[view setDisplayServer: _displayServer];
	}
}

- (void) registerRecognizeTimer: (ChrasisDrawingView *)view {
	[self unregisterRecognizeTimer: view];
	[view setRecognizeTimer: [NSTimer
		scheduledTimerWithTimeInterval: (recognize_delay / 1000.0)
		target: view 
		selector: @selector(recognizeTimerTick:)
		userInfo: nil
		repeats: NO]];
}

- (void) unregisterRecognizeTimer: (ChrasisDrawingView *)view {
	if ([view recognizeTimer] != nil)
	{
		[[view recognizeTimer] invalidate];
		[view setRecognizeTimer: nil];
	}
}

- (IBAction) button_sendkey_pressed: (id)sender {
	// uncomment when we can send special keyevent through OV.
	// looks like
	//		CVContext::event() invoked by
	//		CVKeyReceiver::sendCharacter() invoked by
	//		CVDisplayServerPart::sendCharacterToCurrentComosingBuffer()
	// doesn't support this.
	/*
	NSString *key;
	NSString *keyCode;

	if (sender == button_sendkey_backsp)
	{
		key = @"BackSpace";
		keyCode = @"\x16";
	}
	else if (sender == button_sendkey_del)
	{
		key = @"Delete";
		keyCode = @"\x6b";
	}
	else if (sender == button_sendkey_tab)
	{
		key = @"Tab";
		keyCode = @"\x17";
	}
	else if (sender == button_sendkey_enter)
	{
		key = @"Return";
		keyCode = @"\x6c";
	}
	else if (sender == button_sendkey_space)
	{
		key = @"Space";
		keyCode = @"\x20";
	}
	else if (sender == button_sendkey_left)
	{
		key = @"KP_Left";
		keyCode = @"\x53";
	}
	else if (sender == button_sendkey_right)
	{
		key = @"KP_Right";
		keyCode = @"\x55";
	}
	else
	{
		key = @"Unknown";
		NSLog (@"OVIMChrasisController: unknown key!");
	}

	NSLog (@"OVIMChrasisController: sendkey %@ %@", key, keyCode);
	
	[_displayServer sendCharacterToCurrentComposingBuffer: keyCode];
	*/
}

- (IBAction) button_options_pressed: (id)sender {
	[textfield_num_writing_areas setIntegerValue: num_writing_areas];
	[textfield_writing_area_size setIntegerValue: writing_area_size];
	[textfield_recognize_delay setIntegerValue: recognize_delay];
	[stepper_num_writing_areas setIntegerValue: num_writing_areas];
	[stepper_writing_area_size setIntegerValue: writing_area_size];
	[stepper_recognize_delay setIntegerValue: recognize_delay];
	[button_save_chml setState: save_chml];
	[button_learn_recognized setState: learn_recognized];

	[panel_options makeKeyAndOrderFront: nil];
}

- (IBAction) candidate_list_item_selected: (id)sender {
	ChrasisCandidateListPopUpButton *popup = sender;
	ChrasisDrawingView *view = [popup associatedDrawingView];

	if ([popup selectedItem] == [view popupCandidateListItemIncorrect])
	{
		NSLog (@"incorrect character!");
		[self registerRecognizeTimer: viewToBeLearned];
		viewToBeLearned = view;
		[self unregisterRecognizeTimer: view];

		[panel_learning center];
		[panel_learning makeKeyAndOrderFront: nil];
	}
	else if ([popup selectedItem] == [view popupCandidateListItemClear])
	{
		[view clearCharacterData];
		[self unregisterRecognizeTimer: view];
	}
}

- (IBAction) windowWillClose: (NSNotification *)notification {
	if ([notification object] == panel_options)
	{
		num_writing_areas = [textfield_num_writing_areas integerValue];
		writing_area_size = [textfield_writing_area_size integerValue];
		recognize_delay = [textfield_recognize_delay integerValue];
		save_chml = [button_save_chml state];
		learn_recognized = [button_learn_recognized state];

		NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
		[standardUserDefaults setInteger: num_writing_areas forKey: @"num_writing_areas"];
		[standardUserDefaults setInteger: writing_area_size forKey: @"writing_area_size"];
		[standardUserDefaults setInteger: recognize_delay forKey: @"recognize_delay"];
		[standardUserDefaults setBool: save_chml forKey: @"save_chml"];
		[standardUserDefaults setBool: learn_recognized forKey: @"learn_recognized"];
		[standardUserDefaults synchronize];

		[self initializeWritingAreas];
		[panel_options center];
	}
	else if ([notification object] == panel_learning)
	{
		[[viewToBeLearned popupCandidateList] selectItemAtIndex: 0];
		[self registerRecognizeTimer: viewToBeLearned];
	}
	else if ([notification object] == panel_writing_pad)
	{
		NSLog(@"OVIMChrasis terminated.");
		NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
		[standardUserDefaults setFloat: [panel_writing_pad frame].origin.x forKey: @"writing_pad_x"];
		[standardUserDefaults setFloat: [panel_writing_pad frame].origin.y forKey: @"writing_pad_y"];
		[standardUserDefaults synchronize];
		[[NSApplication sharedApplication] terminate: self];
	}
}

- (IBAction) learning_panel_confirmed: (id)sender {
	[panel_learning close];
	NSString *name = [textfield_correct_character stringValue];
	if (![name isEqualToString: @""])
		[viewToBeLearned learnCharacter: name];
	[viewToBeLearned updateCandidateList];
}

@end
