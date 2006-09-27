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
 * $Id: creater.cpp 18 2006-09-19 21:18:42Z palatis $
 */

#include "version.h"
#include "creater.h"
#include "chmlcodec.h"

#include <iostream>
#include <sstream>
#include <unistd.h>

#include <glibmm/i18n.h>
using namespace std;

Creater::Creater():
	cur_chars_(), current_filename()
{
	set_title("Character Creater");
	set_default_size(400, 400);

	refactiongroup_ = Gtk::ActionGroup::create();
	// MainMenu
	refactiongroup_->add( Gtk::Action::create("FileMenu", _("_File")) );
	refactiongroup_->add( Gtk::Action::create("FileNew", Gtk::Stock::NEW),
		sigc::mem_fun(*this, &Creater::on_menu_file_new) );
	refactiongroup_->add( Gtk::Action::create("FileOpen", Gtk::Stock::OPEN),
		sigc::mem_fun(*this, &Creater::on_menu_file_open) );
	refactiongroup_->add( Gtk::Action::create("FileSave", Gtk::Stock::SAVE),
		sigc::mem_fun(*this, &Creater::on_menu_file_save) );
	refactiongroup_->add( Gtk::Action::create("FileSaveAs", Gtk::Stock::SAVE_AS),
		sigc::mem_fun(*this, &Creater::on_menu_file_saveas) );
	refactiongroup_->add( Gtk::Action::create("FileImport", _("_Import...")),
		sigc::mem_fun(*this, &Creater::on_menu_file_import) );
	refactiongroup_->add( Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
		sigc::mem_fun(*this, &Creater::on_menu_file_quit) );
	refactiongroup_->add( Gtk::Action::create("HelpMenu", _("_Help")) );
	refactiongroup_->add( Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT),
		sigc::mem_fun(*this, &Creater::on_menu_help_about) );
	// Popup Menu
	refactiongroup_->add( Gtk::Action::create("PopupNew", Gtk::Stock::NEW),
		sigc::mem_fun(*this, &Creater::on_popup_new) );
	refactiongroup_->add( Gtk::Action::create("PopupEdit", Gtk::Stock::EDIT),
		sigc::mem_fun(*this, &Creater::on_popup_edit) );
	refactiongroup_->add( Gtk::Action::create("PopupDelete", Gtk::Stock::DELETE),
		sigc::mem_fun(*this, &Creater::on_popup_delete) );

	refuimanager_ = Gtk::UIManager::create();
	refuimanager_->insert_action_group( refactiongroup_ );
	add_accel_group( refuimanager_->get_accel_group() );
	Glib::ustring ui_info =
		"<ui>"
		"  <menubar name='MenuBar'>"
		"    <menu action='FileMenu'>"
		"      <menuitem action='FileNew'/>"
		"      <separator/>"
		"      <menuitem action='FileOpen'/>"
		"      <menuitem action='FileSave'/>"
		"      <menuitem action='FileSaveAs'/>"
		"      <separator/>"
		"      <menuitem action='FileImport'/>"
		"      <separator/>"
		"      <menuitem action='FileQuit'/>"
		"    </menu>"
		"    <menu action='HelpMenu'>"
		"      <menuitem action='HelpAbout'/>"
		"    </menu>"
		"  </menubar>"
		"  <popup name='PopupMenu'>"
		"    <menuitem action='PopupNew'/>"
		"    <menuitem action='PopupEdit'/>"
		"    <menuitem action='PopupDelete'/>"
		"  </popup>"
		"</ui>";
	try
	{      
		refuimanager_->add_ui_from_string(ui_info);
	}
	catch(const Glib::Error& ex)
	{
		std::cerr << "building menus failed: " <<  ex.what();
	}

	scrolledwindow_.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	columns_.add(col_char_);
	columns_.add(col_id_);
	refliststore_ = Gtk::ListStore::create(columns_);
	treeview_.set_model(refliststore_);
	treeview_.append_column(_("Chars"), col_char_);
	//treeview_.append_column("ID", col_id_);
	treeview_.get_column(0)->set_reorderable();

	drawing_.signal_expose_event().connect( sigc::mem_fun(*this, &Creater::on_drawing_expose) );
	eventbox_drawing_.signal_button_press_event().connect( sigc::mem_fun(*this, &Creater::on_drawing_press) );
	eventbox_drawing_.signal_button_release_event().connect( sigc::mem_fun(*this, &Creater::on_drawing_release) );
	eventbox_drawing_.signal_motion_notify_event().connect( sigc::mem_fun(*this, &Creater::on_drawing_motion_notify) );
	treeview_.signal_button_press_event().connect_notify( sigc::mem_fun(*this, &Creater::on_treeview_button_press) );
	treeview_.signal_row_activated().connect( sigc::mem_fun(*this, &Creater::on_treeview_row_activated) );

	// hierarchy
	add(vbox1_);
	vbox1_.pack_start(*(refuimanager_->get_widget("/MenuBar")), Gtk::PACK_SHRINK);
	vbox1_.pack_start(hpaned1_, Gtk::PACK_EXPAND_WIDGET);
		hpaned1_.pack1(eventbox_drawing_, Gtk::EXPAND);
			eventbox_drawing_.add(drawing_);
		hpaned1_.pack2(scrolledwindow_, Gtk::SHRINK);
			scrolledwindow_.add(treeview_);
	vbox1_.pack_start(status_, Gtk::PACK_SHRINK);
	// end hierarchy	

	// initialize colors...
	colors[0] = Gdk::Color("Red");		// color of begining point
	colors[1] = Gdk::Color("Green");	// color of ending point
	colors[2] = Gdk::Color("Black");	// color of the strke
	
	// initialize stroke number...
	stroke_num = 1;

	show_all();

	status_.push(_("done!"));
}

void Creater::on_menu_file_new()
{
	refliststore_->clear();
	cur_chars_.clear();
	drawing_.get_window()->clear();
}

/**
 * print a dialog and get a filename
 *
 * @param dlg_t		the dialog type
 * @param filename	the string object of where the filename goes...
 *
 * @return false on dialog cancel, else true.
 */
bool Creater::get_filename(const Creater::DialogType dlg_t, string & filename)
{
	Glib::ustring dlg_title;
	Gtk::FileChooserAction dlg_action = Gtk::FILE_CHOOSER_ACTION_SAVE;
	Gtk::StockID dlg_okbtn = Gtk::Stock::SAVE;

	switch (dlg_t)
	{
	case SAVE_DIALOG:
		dlg_title = _("Select a file to save...");
		break;
	case SAVEAS_DIALOG:
		dlg_title = _("Select a file to save as...");
		break;
	case OPEN_DIALOG:
		dlg_title = _("Select a file to open...");
		dlg_action = Gtk::FILE_CHOOSER_ACTION_OPEN;
		dlg_okbtn = Gtk::Stock::OPEN;
		break;
	default:
		// should raise an exception, but now just return false.
		return false;
	}

	Gtk::FileChooserDialog dlg(dlg_title, dlg_action);
	dlg.set_transient_for(*this);
	
	//Add response buttons the the dialog:
	dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dlg.add_button(dlg_okbtn, Gtk::RESPONSE_OK);
	
	//Add filters, so that only certain file types can be selected:
	Gtk::FileFilter filter_chml;
	filter_chml.set_name(_("CHML Files (.chml, .xml)"));
	filter_chml.add_pattern("*.chml");
	filter_chml.add_pattern("*.xml");
	dlg.add_filter(filter_chml);

	Gtk::FileFilter filter_text;
	filter_text.set_name(_("Text files (.txt)"));
	filter_text.add_mime_type("text/plain");
	dlg.add_filter(filter_text);

	Gtk::FileFilter filter_any;
	filter_any.set_name(_("Any files"));
	filter_any.add_pattern("*");
	dlg.add_filter(filter_any);

	if (!filename.empty())
		dlg.select_filename(filename);

	if (Gtk::RESPONSE_CANCEL == dlg.run())
		return false;
	
	filename = dlg.get_filename();
	return true;
}

void Creater::on_menu_file_open()
{
	if (get_filename(OPEN_DIALOG, current_filename))
		cur_chars_ = read_chml(current_filename);

	// clean current character lists
	refliststore_->clear();

	int charcnt;
	for (cur_char_ = cur_chars_.begin(), charcnt = 0;
	     cur_char_ != cur_chars_.end();
	     ++cur_char_, ++charcnt)
	{
		Gtk::TreeModel::Row row = *(refliststore_->append());
		row[col_char_] = cur_char_->get_name();
		row[col_id_] = charcnt;
	}

	cur_char_ = cur_chars_.begin();

	drawing_.get_window()->clear();
	draw_character();
}

void Creater::on_menu_file_save()
{
	bool got_filename = false;

	// only try get a filename when there isn't one
	if (current_filename.empty())
		got_filename = get_filename(SAVE_DIALOG, current_filename);

	if (got_filename)
		write_chml(cur_chars_, current_filename);
}

void Creater::on_menu_file_saveas()
{
	if (get_filename(SAVEAS_DIALOG, current_filename))
		write_chml(cur_chars_, current_filename);
}

void Creater::on_menu_file_import()
{
	character_collection tmp_chars;
	// read first...
	if (get_filename(OPEN_DIALOG, current_filename))
		tmp_chars = read_chml(current_filename);

	// first set cur_chars_ to desired capacity
	// this is done for performance...
	cur_chars_.reserve(cur_chars_.size() + tmp_chars.size());
	
	// then merge the two vectors && add into refliststore_
	// O(tmp_chars.size()) complexity...
	int charcnt;
	for (cur_char_ = tmp_chars.begin(), charcnt = cur_chars_.size();
	     cur_char_ != tmp_chars.end();
	     ++cur_char_, ++charcnt)
	{
		cur_chars_.push_back(*cur_char_);
		Gtk::TreeModel::Row row = *(refliststore_->append());
		row[col_char_] = cur_char_->get_name();
		row[col_id_] = charcnt;
	}

	cur_char_ = cur_chars_.begin();
}

void Creater::on_menu_file_quit()
{
	Gtk::Main::quit();
}

void Creater::on_menu_help_about()
{
	Gtk::AboutDialog dlg;
	dlg.set_transient_for(*this);

	dlg.set_name(VersionInfo().PROGRAM_NAME());
	dlg.set_version(VersionInfo().PROGRAM_VERSION());
	dlg.set_copyright(VersionInfo().PROGRAM_COPYRIGHT());
	dlg.set_comments(VersionInfo().PROGRAM_COMMENTS());
	dlg.set_license(VersionInfo().PROGRAM_LICENSE());
	dlg.set_website(VersionInfo().PROGRAM_WEBSITE());
	dlg.set_website_label(VersionInfo().PROGRAM_WEBSITE_LABEL());
	  
	std::list<Glib::ustring> list_authors;
	for (int i=0;i<VersionInfo().PROGRAM_AUTHORS().size();++i)
		list_authors.push_back(VersionInfo().PROGRAM_AUTHORS()[i]);
	dlg.set_authors(list_authors);

	dlg.run();
}

void Creater::on_popup_new()
{
	add_char();

	drawing_.get_window()->clear();
}

void Creater::on_popup_edit()
{
	if (0 == cur_chars_.size())
	{
		Gtk::MessageDialog dialog(*this, _("No character selected"));
		dialog.set_secondary_text(_("Add a character before editing it."));
		dialog.run();
		
		return;
	}

	Gtk::Dialog dlg(_("Input a chinese character"));
	dlg.set_transient_for(*this);

        dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dlg.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	Gtk::Label dlg_label(_("Input a chinese character."));
	Gtk::Entry dlg_entry;
	dlg_entry.set_max_length(1);
	dlg.get_vbox()->pack_start(dlg_label);
	dlg.get_vbox()->pack_start(dlg_entry);
	dlg.get_vbox()->show_all();

	if (dlg.run() == Gtk::RESPONSE_CANCEL)
		return;
	
	(*(treeview_.get_selection()->get_selected()))[col_char_] = dlg_entry.get_text();
	cur_char_->set_name(dlg_entry.get_text());
}

void Creater::on_popup_delete()
{
	Gtk::TreeModel::iterator iter = treeview_.get_selection()->get_selected();
	if (iter)
	{
		cur_char_ = cur_chars_.begin() + (*iter)[col_id_];
		cur_chars_.erase(cur_char_);

		for (iter = refliststore_->erase(iter);
		     iter;
		     ++iter)
			(*iter)[col_id_] = (*iter)[col_id_] - 1;

	}
}

void Creater::add_char()
{
	// a dialog to get the character...
	Gtk::Dialog dlg(_("Input a chinese character"));
	dlg.set_transient_for(*this);

        dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dlg.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	Gtk::Label dlg_label(_("Input a chinese character."));
	Gtk::Entry dlg_entry;
	dlg_entry.set_max_length(1);
	dlg.get_vbox()->pack_start(dlg_label);
	dlg.get_vbox()->pack_start(dlg_entry);
	dlg.get_vbox()->show_all();

	if (dlg.run() == Gtk::RESPONSE_CANCEL)
		return;

	Gtk::TreeModel::Row row = *(refliststore_->append());
	row[col_char_] = dlg_entry.get_text();
	row[col_id_] = cur_chars_.size();

	cur_chars_.push_back(Character(dlg_entry.get_text()));
	cur_char_ = cur_chars_.end() - 1;
}

bool Creater::on_delete_event(GdkEventAny * event)
{
	Gtk::Main::quit();
	return true;
}

bool Creater::on_drawing_motion_notify(GdkEventMotion * event)
{
	if (event->state & GDK_BUTTON1_MASK)
	{
		if (cur_chars_.size() == 0)
			return false;
	
		double x = static_cast<double>(event->x) / drawing_.get_width();
		double y = static_cast<double>(event->y) / drawing_.get_height();

		// do not process redundent points
		if (event->x == last_x && event->y == last_y)
			return false;
		cur_char_->add_point(x, y);

		Glib::RefPtr<Gdk::Window> win = drawing_.get_window();
		Glib::RefPtr<Gdk::GC> gc = drawing_.get_style()->get_black_gc();

		gc->set_rgb_fg_color(colors[2]);
		win->draw_line(
			gc,
			static_cast<int>(event->x), static_cast<int>(event->y),
			last_x, last_y
		);

		last_x = static_cast<int>(event->x);
		last_y = static_cast<int>(event->y);
	}

	return false;
}

bool Creater::on_drawing_press(GdkEventButton * event)
{
	if (event->button == 1)
	{
		if (cur_chars_.size() == 0)
		{
			Gtk::MessageDialog dialog(*this, _("No character selected"));	
			dialog.set_secondary_text(_("Add a character before drawing it."));
			dialog.run();

			return false;
		}

		cur_char_->new_stroke();

		double x = static_cast<double>(event->x) / drawing_.get_width();
		double y = static_cast<double>(event->y) / drawing_.get_height();
		cur_char_->add_point(x, y);

		Glib::RefPtr<Gdk::Window> win = drawing_.get_window();
		Glib::RefPtr<Gdk::GC> gc = drawing_.get_style()->get_black_gc();

		// draw a number before each stroke
		stringstream ss;
		ss << stroke_num++;
		Glib::RefPtr<Pango::Layout> num = create_pango_layout(ss.str());
		int lx, ly;
		num->get_size(lx, ly);
		gc->set_rgb_fg_color(colors[2]);
		win->draw_layout(
			gc,
			static_cast<int>(event->x) - lx / Pango::SCALE - 5,
			static_cast<int>(event->y) - ly / Pango::SCALE - 5,
			num
		);

		// draw a dot
		gc->set_rgb_fg_color(colors[0]);
		win->draw_arc( gc, true,
			static_cast<int>(event->x) - 3,
			static_cast<int>(event->y) - 3,
			5, 5, 0, 23040);

		last_x = static_cast<int>(event->x);
		last_y = static_cast<int>(event->y);
	}

	return false;
}

bool Creater::on_drawing_release(GdkEventButton * event)
{
	if (event->button == 1)
	{
		if (cur_chars_.size() == 0)
			return false;

		double x = static_cast<double>(event->x) / drawing_.get_width();
		double y = static_cast<double>(event->y) / drawing_.get_height();

		// do not process redundent points
		if (event->x != last_x || event->y != last_y)
			cur_char_->add_point(x, y);

		Glib::RefPtr<Gdk::Window> win = drawing_.get_window();
		Glib::RefPtr<Gdk::GC> gc = drawing_.get_style()->get_black_gc();

		// draw a dot
		gc->set_rgb_fg_color(colors[1]);
		win->draw_arc( gc, true,
			static_cast<int>(event->x) - 3,
			static_cast<int>(event->y) - 3,
			5, 5, 0, 23040);
	}

	return false;
}

bool Creater::on_drawing_expose(GdkEventExpose * event)
{
	draw_character();

	return true;
}

void Creater::on_treeview_button_press(GdkEventButton * event)
{
	if (event->type == GDK_BUTTON_PRESS)
	{
		if (event->button == 3)
		{
			Gtk::Menu * menu = dynamic_cast<Gtk::Menu *>(refuimanager_->get_widget("/PopupMenu"));
			menu->popup(event->button, event->time);
		}
	}
}

void Creater::on_treeview_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
	Gtk::TreeModel::iterator iter = refliststore_->get_iter(path);
	if(iter)
	{
		Gtk::TreeModel::Row row = *iter;
		cur_char_ = cur_chars_.begin() + row[col_id_];

		drawing_.get_window()->clear();
		draw_character();
	}
}

void Creater::draw_character()
{
	if (cur_chars_.size() == 0)
		return;

	Glib::RefPtr<Gdk::Window> win = drawing_.get_window();
	Glib::RefPtr<Gdk::GC> gc = drawing_.get_style()->get_black_gc();

	stroke_num = 1;
	for (Character::strokes_iterator si = cur_char_->strokes_begin();
	     si != cur_char_->strokes_end();
	     ++si)
	{
		gc->set_rgb_fg_color(colors[2]);
		for (Stroke::points_iterator pi = si->points_begin();
		     pi != si->points_end() - 1;
		     ++pi)
		{
			win->draw_line(
				gc,
				static_cast<int>(pi->x() * drawing_.get_width()),
				static_cast<int>(pi->y() * drawing_.get_height()),
				static_cast<int>((pi+1)->x() * drawing_.get_width()),
				static_cast<int>((pi+1)->y() * drawing_.get_height())
			);
		}
		// draw a number before each stroke
		stringstream ss;
		ss << stroke_num++;
		Glib::RefPtr<Pango::Layout> num = create_pango_layout(ss.str());
		int lx, ly;
		num->get_size(lx, ly);
		win->draw_layout(
			gc,
			static_cast<int>(si->points_begin()->x() * drawing_.get_width()) - lx / Pango::SCALE - 5,
			static_cast<int>(si->points_begin()->y() * drawing_.get_height()) - ly / Pango::SCALE - 5,
			num
		);

		// enchant the beginning and ending points
		Stroke::points_iterator pi[2];
		pi[0] = si->points_begin();
		pi[1] = si->points_end() - 1;

		for (int i=0;i<2;++i)
		{
			gc->set_rgb_fg_color(colors[i]);
			win->draw_arc( gc, true,
				static_cast<int>(pi[i]->x() * drawing_.get_width()) - 3,
				static_cast<int>(pi[i]->y() * drawing_.get_height()) - 3,
				5, 5, 0, 23040);
		}
	}

}
