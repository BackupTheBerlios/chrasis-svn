/*
 * Character Creater
 *
 * Copyright (c) 2006 Victor Tseng <palatis@gentoo.tw>
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
 * $Id: creater.h 18 2006-09-19 21:18:42Z palatis $
 */

#ifndef _CREATER_H
#define _CREATER_H

#include <string>

#include <gtkmm.h>
#include <gdkmm.h>
#include <glibmm.h>

#include "character.h"

class Creater: public Gtk::Window
{
public:
	Creater();

protected:
	void on_menu_file_new();
	void on_menu_file_open();
	void on_menu_file_save();
	void on_menu_file_saveas();
	void on_menu_file_import();
	void on_menu_file_quit();
	void on_menu_help_about();
	void on_popup_new();
	void on_popup_edit();
	void on_popup_delete();

	bool on_delete_event(GdkEventAny *);
	bool on_drawing_expose(GdkEventExpose *);
	bool on_drawing_press(GdkEventButton *);
	bool on_drawing_release(GdkEventButton *);
	bool on_drawing_motion_notify(GdkEventMotion *);
	void on_treeview_button_press(GdkEventButton *);
	void on_treeview_row_activated(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);

private:
	void add_char();			///< add character with a dialog to get the name.

	void draw_character();
	enum DialogType {
		SAVE_DIALOG,
		SAVEAS_DIALOG,
		OPEN_DIALOG
	};
	bool get_filename(const DialogType, std::string &);
	bool column_id_fixer(const Gtk::TreeModel::iterator& iter) { (*iter)[col_id_] = column_id_fisrt_; return true; };

	character_collection cur_chars_;
	character_collection::iterator cur_char_;

	Gtk::TreeModelColumn<Glib::ustring> col_char_;
	Gtk::TreeModelColumn<unsigned int> col_id_;
	Gtk::TreeModelColumnRecord columns_;
	Glib::RefPtr<Gtk::ListStore> refliststore_;

	Glib::RefPtr<Gtk::UIManager> refuimanager_;
	Glib::RefPtr<Gtk::ActionGroup> refactiongroup_;

	Gtk::Menu treeview_popup_;

	// hierarchy
	Gtk::VBox vbox1_;
		// *(refuimanager_->get_widget("/MenuBar")); // MenuBar
		Gtk::HPaned hpaned1_;
			Gtk::EventBox eventbox_drawing_;
				Gtk::DrawingArea drawing_;
			Gtk::ScrolledWindow scrolledwindow_;
				Gtk::TreeView treeview_;
		Gtk::Statusbar status_;
	// end hierarchy
	
	Gdk::Color colors[3];	// 0: beginning point, 1: ending point, 2: stroke body
	int stroke_num;
	int last_x;
	int last_y;

	std::string current_filename;
	unsigned int column_id_fisrt_;
};

#endif
