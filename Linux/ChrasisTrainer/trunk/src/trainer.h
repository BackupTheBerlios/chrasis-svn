/*
 * Chrasis Trainer
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
 * $Id: creater.h 18 2006-09-19 21:18:42Z palatis $
 */

#ifndef _TRAINER_H
#define _TRAINER_H

#include <gtkmm.h>
#include <gdkmm.h>
#include <glibmm.h>

#include <chrasis.h>

class Trainer:
	public Gtk::Window
{
public:
	Trainer();

protected:
	// Signal handlers:
	void on_menu_file_quit();

	void on_learn_clicked();
	void on_newchar_clicked();
	void on_clear_clicked();

	bool on_delete_event(GdkEventAny *);
	bool on_drawing_expose(GdkEventExpose *);
	bool on_drawing_press(GdkEventButton *);
	bool on_drawing_release(GdkEventButton *);
	bool on_drawing_motion_notify(GdkEventMotion *);
	void on_treeview_button_press(GdkEventButton *);
	void on_treeview_row_activated(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);

private:
	void learn_curchar();
	void draw_character();
	void update_candidate_list();

	chrasis::Database db_;
	chrasis::Character cur_char_;

	Glib::RefPtr<Gtk::UIManager> refuimanager_;
	Glib::RefPtr<Gtk::ActionGroup> refactiongroup_;

	Glib::RefPtr<Gtk::ListStore> refliststore_;
		Gtk::TreeModelColumnRecord columns_;
			Gtk::TreeModelColumn<Glib::ustring> col_char_;
			Gtk::TreeModelColumn<unsigned int> col_diff_;
			Gtk::TreeModelColumn<unsigned int> col_id_;

	// Child widgets:
	Gtk::VBox vbox1_;
		// *(refuimanager_->get_widget("/MenuBar")); // MenuBar
		Gtk::HPaned hpaned1_;
			Gtk::EventBox eventbox_drawing_;
				Gtk::DrawingArea drawing_;
			Gtk::VBox vbox2_;
				Gtk::ScrolledWindow scrolledwindow_;
					Gtk::TreeView treeview_;
				Gtk::Button button_learn_;
				Gtk::Button button_newchar_;
				Gtk::Button button_clear_;
		Gtk::Statusbar status_;

	Gdk::Color colors[4];	// 0: beginning point
				// 1: ending point
				// 2: stroke body
				// 3: simplified stroke

	int stroke_num;
	int last_x;
	int last_y;
};

#endif
