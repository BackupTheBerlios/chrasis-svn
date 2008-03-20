//
//  OVIMChrasisController.h
//  OVIMChrasis
//
//  Created by Palatis on 2008/3/16.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "ChrasisDrawingView.h"
#import "ChrasisCandidateListPopUpButton.h"

#import "ChrasisWritingPanel.h"

@interface OVIMChrasisController : NSObject {
	IBOutlet ChrasisWritingPanel *panel_writing_pad;
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
	IBOutlet ChrasisCandidateListPopUpButton *popup_candidate_list_1;
	IBOutlet ChrasisCandidateListPopUpButton *popup_candidate_list_2;
	IBOutlet ChrasisCandidateListPopUpButton *popup_candidate_list_3;
	IBOutlet ChrasisCandidateListPopUpButton *popup_candidate_list_4;
	IBOutlet ChrasisCandidateListPopUpButton *popup_candidate_list_5;
	IBOutlet ChrasisCandidateListPopUpButton *popup_candidate_list_6;
	IBOutlet ChrasisCandidateListPopUpButton *popup_candidate_list_7;
	IBOutlet ChrasisCandidateListPopUpButton *popup_candidate_list_8;
	IBOutlet ChrasisDrawingView *view_writing_area_1;
	IBOutlet ChrasisDrawingView *view_writing_area_2;
	IBOutlet ChrasisDrawingView *view_writing_area_3;
	IBOutlet ChrasisDrawingView *view_writing_area_4;
	IBOutlet ChrasisDrawingView *view_writing_area_5;
	IBOutlet ChrasisDrawingView *view_writing_area_6;
	IBOutlet ChrasisDrawingView *view_writing_area_7;
	IBOutlet ChrasisDrawingView *view_writing_area_8;

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
	
	// from panel_learning
	IBOutlet NSTextField *textfield_correct_character;
	
	int num_writing_areas;
	int writing_area_size;
	int recognize_delay;
	BOOL save_chml;
	BOOL learn_recognized;

	ChrasisDrawingView *viewToBeLearned;
	
	NSDictionary *dict_button_script;
}

- (void) initializeWritingAreas;

- (void) awakeFromNib;

- (void) registerRecognizeTimer: (ChrasisDrawingView *)view;
- (void) unregisterRecognizeTimer: (ChrasisDrawingView *)view;
- (BOOL) shouldLearnRecognized;
- (BOOL) shouldSaveWrittenCharacter;
- (void) sendStringToOpenVanilla: (NSString *)str;

- (IBAction) candidate_list_item_selected: (id)sender;

- (IBAction) button_sendkey_pressed: (id)sender;
- (IBAction) button_options_pressed: (id)sender;

- (IBAction) learning_panel_confirmed: (id)sender;

- (void) windowWillClose: (NSNotification *)notification;

- (IBAction) button_aboutov_clicked: (id)sender;

@end
