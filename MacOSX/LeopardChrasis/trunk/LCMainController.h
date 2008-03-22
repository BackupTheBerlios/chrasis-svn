//
//  OVIMChrasisController.h
//  OVIMChrasis
//
//  Created by Palatis on 2008/3/16.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDrawingView.h"
#import "LCCandidateListPopUpButton.h"

#import "LCWritingPanel.h"

@interface LCMainController : NSObject {
	IBOutlet LCWritingPanel *panel_writing_pad;
	IBOutlet NSPanel *panel_options;
	IBOutlet NSPanel *panel_learning;

	// from panel_writing_pad
	IBOutlet NSView *view_keypad;
	IBOutlet NSButton *button_sendkey_backsp;
	IBOutlet NSButton *button_sendkey_del;
	IBOutlet NSButton *button_sendkey_tab;
	IBOutlet NSButton *button_sendkey_enter;
	IBOutlet NSButton *button_sendkey_space;
	IBOutlet NSButton *button_sendkey_left;
	IBOutlet NSButton *button_sendkey_right;
	IBOutlet LCCandidateListPopUpButton *popup_candidate_list_1;
	IBOutlet LCCandidateListPopUpButton *popup_candidate_list_2;
	IBOutlet LCCandidateListPopUpButton *popup_candidate_list_3;
	IBOutlet LCCandidateListPopUpButton *popup_candidate_list_4;
	IBOutlet LCCandidateListPopUpButton *popup_candidate_list_5;
	IBOutlet LCCandidateListPopUpButton *popup_candidate_list_6;
	IBOutlet LCCandidateListPopUpButton *popup_candidate_list_7;
	IBOutlet LCCandidateListPopUpButton *popup_candidate_list_8;
	IBOutlet LCDrawingView *view_writing_area_1;
	IBOutlet LCDrawingView *view_writing_area_2;
	IBOutlet LCDrawingView *view_writing_area_3;
	IBOutlet LCDrawingView *view_writing_area_4;
	IBOutlet LCDrawingView *view_writing_area_5;
	IBOutlet LCDrawingView *view_writing_area_6;
	IBOutlet LCDrawingView *view_writing_area_7;
	IBOutlet LCDrawingView *view_writing_area_8;

	NSArray *popup_candidate_list_array;
	NSArray *view_writing_area_array;

	// from panel_options
	IBOutlet NSTextField *textfield_num_writing_areas;
	IBOutlet NSTextField *textfield_writing_area_size;
	IBOutlet NSTextField *textfield_recognize_delay;
	IBOutlet NSStepper *stepper_num_writing_areas;
	IBOutlet NSStepper *stepper_writing_area_size;
	IBOutlet NSStepper *stepper_recognize_delay;
	IBOutlet NSButton *button_save_chml;
	IBOutlet NSButton *button_learn_recognized;
	IBOutlet NSTextField *textfield_informations;
	
	// from panel_learning
	IBOutlet NSTextField *textfield_correct_character;
	
	int num_writing_areas;
	int writing_area_size;
	int recognize_delay;
	BOOL save_chml;
	BOOL learn_recognized;

	LCDrawingView *viewToBeLearned;
	
	NSDictionary *dict_button_script;
}

- (void) initializeWritingAreas;

- (void) awakeFromNib;

- (void) registerRecognizeTimer: (LCDrawingView *)view;
- (void) unregisterRecognizeTimer: (LCDrawingView *)view;
- (BOOL) shouldLearnRecognized;
- (BOOL) shouldSaveWrittenCharacter;

- (IBAction) candidate_list_item_selected: (id)sender;

- (IBAction) button_sendkey_pressed: (id)sender;
- (IBAction) button_options_pressed: (id)sender;

- (IBAction) learning_panel_confirmed: (id)sender;

- (void) windowWillClose: (NSNotification *)notification;

@end
