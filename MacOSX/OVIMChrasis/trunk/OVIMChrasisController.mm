/*
 * OVIMChrasis - Chrasis OpenVanilla binding for MacOSX
 *
 * Copyright (c) 2006 Victor Tseng <palatis@gmail.com>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * $Id: character.h 53 2007-10-27 23:33:29Z palatis $
 */

#import "OVIMChrasisController.h"
#import "chrasis.h"
#import "OVKPDistributedStringReceiverProtocol.h"

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
	[view_keypad setHidden: YES];
	for (int i=0;i<[view_writing_area_array count];++i) {
		ChrasisDrawingView *view = [view_writing_area_array objectAtIndex: i];
		ChrasisCandidateListPopUpButton *popup = [popup_candidate_list_array objectAtIndex: i];
		[view setHidden: YES];
		[popup setHidden: YES];
	}

	// resize and put them in position
	CGFloat popup_height = [popup_candidate_list_1 frame].size.height;
	[panel_writing_pad
		setFrame: NSMakeRect(
			[panel_writing_pad frame].origin.x, [panel_writing_pad frame].origin.y,
			(writing_area_size + 2) * num_writing_areas + 1 + [view_keypad frame].size.width,
			[self titleBarHeight] + popup_height + writing_area_size + 4)
		display: YES animate: YES];
	CGFloat popup_y = [popup_candidate_list_1 frame].origin.y;
	for (int i=0;i<num_writing_areas;++i) {
		ChrasisDrawingView *view = [view_writing_area_array objectAtIndex: i];
		ChrasisCandidateListPopUpButton *popup = [popup_candidate_list_array objectAtIndex: i];
		[popup setFrame: NSMakeRect((writing_area_size + 2) * i + 4, popup_y, writing_area_size, popup_height)];
		[view setFrame: NSMakeRect((writing_area_size + 2) * i + 4, 5, writing_area_size, writing_area_size)];
		[popup setHidden: NO];
		[view setHidden: NO];
	}
	[view_keypad setHidden: NO];
}

- (void)awakeFromNib {
	[panel_writing_pad setCollectionBehavior: NSWindowCollectionBehaviorCanJoinAllSpaces];
	[panel_writing_pad setLevel: NSScreenSaverWindowLevel - 1];
	[panel_options setLevel: NSScreenSaverWindowLevel];
	[panel_learning setLevel: NSScreenSaverWindowLevel];
	
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
	[textfield_num_writing_areas setIntegerValue: num_writing_areas];
	[textfield_writing_area_size setIntegerValue: writing_area_size];
	[textfield_recognize_delay setIntegerValue: recognize_delay];
	[stepper_num_writing_areas setIntegerValue: num_writing_areas];
	[stepper_writing_area_size setIntegerValue: writing_area_size];
	[stepper_recognize_delay setIntegerValue: recognize_delay];
	[button_save_chml setState: save_chml];
	[button_learn_recognized setState: learn_recognized];

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

	view_writing_area_array = [[NSArray arrayWithObjects:
		view_writing_area_1, view_writing_area_2, view_writing_area_3,
		view_writing_area_4, view_writing_area_5, view_writing_area_6,
		view_writing_area_7, view_writing_area_8, nil] retain];
	popup_candidate_list_array = [[NSArray arrayWithObjects:
		popup_candidate_list_1, popup_candidate_list_2, popup_candidate_list_3,
		popup_candidate_list_4, popup_candidate_list_5, popup_candidate_list_6,
		popup_candidate_list_7, popup_candidate_list_8, nil] retain];

	[self initializeWritingAreas];

	int i;
	for (i=0;i<8;++i) {
		ChrasisDrawingView *view = [view_writing_area_array objectAtIndex: i];
		ChrasisCandidateListPopUpButton *popup = [popup_candidate_list_array objectAtIndex: i];
		[view setPopupCandidateList: popup];
		[popup setAssociatedDrawingView: view];
		[view setController: self];
		[popup setController: self];
	}

	NSString *src =	@"tell application \"System Events\"\nkeystroke (ASCII character %@)\nend tell";
	dict_button_script = [[NSDictionary dictionaryWithObjectsAndKeys:
		[[NSAppleScript alloc] initWithSource: [NSString stringWithFormat: src, @"8"]],		[button_sendkey_backsp title],
		[[NSAppleScript alloc] initWithSource: [NSString stringWithFormat: src, @"127"]],	[button_sendkey_del title],
		[[NSAppleScript alloc] initWithSource: [NSString stringWithFormat: src, @"13"]],	[button_sendkey_enter title],
		[[NSAppleScript alloc] initWithSource: [NSString stringWithFormat: src, @"32"]],	[button_sendkey_space title],
		[[NSAppleScript alloc] initWithSource: [NSString stringWithFormat: src, @"9"]],		[button_sendkey_tab title],
		[[NSAppleScript alloc] initWithSource: [NSString stringWithFormat: src, @"28"]],	[button_sendkey_left title],
		[[NSAppleScript alloc] initWithSource: [NSString stringWithFormat: src, @"29"]],	[button_sendkey_right title],
		nil] retain];
	for (id key in dict_button_script)
		[[dict_button_script objectForKey: key] compileAndReturnError: nil];

	[textfield_informations setStringValue: [NSString stringWithFormat:
		NSLocalizedString(
			@"Chrasis Informations:\n"
			@"System Database Path:\n\t%@\n"
			@"User Database Path:\n\t%@\n",
			@"Chrasis information in about tab of option panel."
			@"this string has two parameters, the return values from"
			@"    system_database_path()"
			@"and"
			@"    user_database_path()"),
		[NSString stringWithUTF8String: chrasis::platform::system_database_path().c_str()],
		[NSString stringWithUTF8String: chrasis::platform::user_database_path().c_str()]
	]];
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
	[[dict_button_script objectForKey: [sender title]] executeAndReturnError: nil];
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
		[textfield_correct_character setStringValue: @""];

		[self registerRecognizeTimer: viewToBeLearned];
		viewToBeLearned = view;
		[self unregisterRecognizeTimer: viewToBeLearned];

		[panel_learning center];
		[textfield_correct_character selectText: nil];
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
		NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
		[standardUserDefaults setFloat: [panel_writing_pad frame].origin.x forKey: @"writing_pad_x"];
		[standardUserDefaults setFloat: [panel_writing_pad frame].origin.y forKey: @"writing_pad_y"];
		[standardUserDefaults synchronize];

		[[NSApplication sharedApplication] terminate: self];
	}
}

- (IBAction) learning_panel_confirmed: (id)sender {
	[panel_learning close];
	[viewToBeLearned learnCharacter: [textfield_correct_character stringValue]];
	[viewToBeLearned updateCandidateList];
}

- (void) sendStringToOpenVanilla: (NSString *)str {
	id stringReceiver = [[NSConnection rootProxyForConnectionWithRegisteredName: OVDSTRSTRRCVR_SRVNAME host: nil] retain];
	if (stringReceiver == nil) {
		NSRunCriticalAlertPanel(
			NSLocalizedString(@"Unable to connect, Check OpenVanilla status.", @"no-connection panel title"),
			[NSString stringWithFormat: 
				NSLocalizedString(@"Connection to %@ failed.\nCheck OpenVanilla status!", @"no-connection panel message"),
				OVDSTRSTRRCVR_SRVNAME],
				NSLocalizedString(@"Too Bad!", @"no-connection panel button text"),
			nil, nil);
		return;
	}
	[stringReceiver retain];
	[stringReceiver setProtocolForProxy:@protocol(OVKPDistributedStringReceiverProtocol)];

	[stringReceiver sendStringToCurrentComposingBuffer: str];
	[stringReceiver release];
}

@end
