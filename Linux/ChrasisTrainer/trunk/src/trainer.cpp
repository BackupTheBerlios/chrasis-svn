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

#include <chrasis.h>
#include <unistd.h>
#include <glibmm/i18n.h>

#include <sstream>
#include <string>

inline
std::string
utf8_to_hex(std::string s)
{
	std::ostringstream oss;
	for(int i=0;i<s.length();++i)
		oss << std::hex << static_cast<int>(static_cast<unsigned char>(s[i]));
	return oss.str();
}

#include "trainer.h"
#include "chmlcodec.h"

using namespace std;

Trainer::Trainer():
	db_("chr_data.db")
{
	set_title("Chrasis Trainer");
	set_default_size(400, 400);

	refactiongroup_ = Gtk::ActionGroup::create();
	// MainMenu
	refactiongroup_->add( Gtk::Action::create("FileMenu", _("_File")) );
	refactiongroup_->add( Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
		sigc::mem_fun(*this, &Trainer::on_menu_file_quit) );

	refuimanager_ = Gtk::UIManager::create();
	refuimanager_->insert_action_group( refactiongroup_ );
	add_accel_group( refuimanager_->get_accel_group() );
	try
	{      
		refuimanager_->add_ui_from_string(
			"<ui>"
			"  <menubar name='MenuBar'>"
			"    <menu action='FileMenu'>"
			"      <menuitem action='FileQuit'/>"
			"    </menu>"
			"  </menubar>"
			"</ui>"
		);
	}
	catch(const Glib::Error& ex)
	{
		std::cerr << "building menus failed: " <<  ex.what();
	}

	scrolledwindow_.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	columns_.add(col_char_);
	columns_.add(col_diff_);
	columns_.add(col_id_);
	refliststore_ = Gtk::ListStore::create(columns_);
	treeview_.set_model(refliststore_);
	treeview_.append_column(_("Chars"), col_char_);
	treeview_.append_column(_("Difference"), col_diff_);
	//treeview_.append_column("ID", col_id_);
	treeview_.get_column(0)->set_reorderable();

	drawing_.signal_expose_event().connect(
		sigc::mem_fun(*this, &Trainer::on_drawing_expose) );
	eventbox_drawing_.signal_button_press_event().connect( 
		sigc::mem_fun(*this, &Trainer::on_drawing_press) );
	eventbox_drawing_.signal_button_release_event().connect( 
		sigc::mem_fun(*this, &Trainer::on_drawing_release) );
	eventbox_drawing_.signal_motion_notify_event().connect( 
		sigc::mem_fun(*this, &Trainer::on_drawing_motion_notify) );
	button_learn_.signal_clicked().connect(
		sigc::mem_fun(*this, &Trainer::on_learn_clicked) );
	button_newchar_.signal_clicked().connect(
		sigc::mem_fun(*this, &Trainer::on_newchar_clicked) );
	button_clear_.signal_clicked().connect(
		sigc::mem_fun(*this, &Trainer::on_clear_clicked) );

	// hierarchy
	add(vbox1_);
	vbox1_.pack_start(*(refuimanager_->get_widget("/MenuBar")), Gtk::PACK_SHRINK);
	vbox1_.pack_start(hpaned1_, Gtk::PACK_EXPAND_WIDGET);
		hpaned1_.pack1(eventbox_drawing_, Gtk::EXPAND);
			eventbox_drawing_.add(drawing_);
		hpaned1_.pack2(vbox2_, Gtk::SHRINK);
			vbox2_.pack_start(scrolledwindow_, Gtk::PACK_EXPAND_WIDGET);
				scrolledwindow_.add(treeview_);
			vbox2_.pack_start(button_learn_, Gtk::PACK_SHRINK);
			vbox2_.pack_start(button_newchar_, Gtk::PACK_SHRINK);
			vbox2_.pack_start(button_clear_, Gtk::PACK_SHRINK);
	vbox1_.pack_start(status_, Gtk::PACK_SHRINK);
	// end hierarchy

	button_learn_.set_label( _("Learn") );
	button_newchar_.set_label( _("All Wrong!") );
	button_clear_.set_label( _("Clear") );

	// initialize colors...
	colors[0] = Gdk::Color("Red");		// color of begining point
	colors[1] = Gdk::Color("Green");	// color of ending point
	colors[2] = Gdk::Color("Blue");		// color of the stroke
	colors[3] = Gdk::Color("Black");	// color of normalized stroke

	// initialize stroke number...
	stroke_num = 0;

	show_all();

	status_.push(_("done!"));
}

void
Trainer::on_menu_file_quit()
{
	Gtk::Main::quit();
}

void
Trainer::on_learn_clicked()
{
	Glib::RefPtr<Gtk::TreeSelection> refts = treeview_.get_selection();

	if ( Gtk::TreeIter sl = refts->get_selected() )
	{
		Glib::ustring cn = (*sl)[col_char_];
		cur_char_.set_name(cn);
		learn_curchar();
		on_clear_clicked();
		return;
	}

	Gtk::Dialog dlg(_("Not selected"));
	dlg.set_transient_for(*this);

	dlg.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	Gtk::Label dlg_label(_(
		"Select a character from the list first.\n"
		"If there's nothing on the list, click \"All Wrong!\"."
	));
	dlg.get_vbox()->pack_start(dlg_label);
	dlg.get_vbox()->show_all();
	dlg.run();
}

void
Trainer::on_newchar_clicked()
{
	Gtk::Dialog dlg(_("Input a chinese character"));
	dlg.set_transient_for(*this);

	dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dlg.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	Gtk::Label dlg_label(_("Input a chinese character."));
	Gtk::Entry dlg_entry;
	dlg.get_vbox()->pack_start(dlg_label);
	dlg.get_vbox()->pack_start(dlg_entry);
	dlg.get_vbox()->show_all();

	if (dlg.run() == Gtk::RESPONSE_CANCEL)
		return;

	cur_char_.set_name(dlg_entry.get_text());
	learn_curchar();
	on_clear_clicked();
}

void
Trainer::on_clear_clicked()
{
	refliststore_->clear();
	stroke_num = 0;
	cur_char_ = chrasis::Character();
	drawing_.get_window()->clear();
}

bool
Trainer::on_delete_event(GdkEventAny * event)
{
	Gtk::Main::quit();
	return true;
}

bool
Trainer::on_drawing_expose(GdkEventExpose * event)
{
	draw_character();
	return true;
}

bool
Trainer::on_drawing_press(GdkEventButton * event)
{
	if (event->button == 1)
	{
		cur_char_.new_stroke();
		cur_char_.add_point(event->x, event->y);
		last_x = static_cast<int>(event->x);
		last_y = static_cast<int>(event->y);

		Glib::RefPtr<Gdk::Window> win = drawing_.get_window();
		Glib::RefPtr<Gdk::GC> gc = drawing_.get_style()->get_black_gc();

		// draw a number before each stroke
		Glib::RefPtr<Pango::Layout> num = create_pango_layout(chrasis::toString(++stroke_num));
		int lx, ly;
		num->get_size(lx, ly);
		gc->set_rgb_fg_color(colors[2]);
		win->draw_layout(
			gc,
			static_cast<int>(event->x - lx / Pango::SCALE - 5),
			static_cast<int>(event->y - ly / Pango::SCALE - 5),
			num
		);

		// draw a dot
		gc->set_rgb_fg_color(colors[0]);
		win->draw_arc( gc, true,
			static_cast<int>(event->x) - 3,
			static_cast<int>(event->y) - 3,
			5, 5, 0, 23040);
	}

	return false;
}

bool
Trainer::on_drawing_release(GdkEventButton * event)
{
	if (event->button == 1)
	{
		// do not process redundent points
		if (event->x != last_x || event->y != last_y)
			cur_char_.add_point(event->x, event->y);

		Glib::RefPtr<Gdk::Window> win = drawing_.get_window();
		Glib::RefPtr<Gdk::GC> gc = drawing_.get_style()->get_black_gc();

		// draw a dot
		gc->set_rgb_fg_color(colors[1]);
		win->draw_arc( gc, true,
			static_cast<int>(event->x) - 3,
			static_cast<int>(event->y) - 3,
			5, 5, 0, 23040);

		update_candidate_list();
	}
	return false;
}

bool
Trainer::on_drawing_motion_notify(GdkEventMotion * event)
{
	if (event->state & GDK_BUTTON1_MASK)
	{
		// do not process redundent points
		if (event->x == last_x && event->y == last_y)
			return false;
		cur_char_.add_point(event->x, event->y);

		Glib::RefPtr<Gdk::Window> win = drawing_.get_window();
		Glib::RefPtr<Gdk::GC> gc = drawing_.get_style()->get_black_gc();

		gc->set_rgb_fg_color(colors[2]);
		win->draw_line(
			gc,
			static_cast<int>(event->x),
			static_cast<int>(event->y),
			last_x, last_y
		);

		last_x = static_cast<int>(event->x);
		last_y = static_cast<int>(event->y);
	}

	return false;
}

void
Trainer::draw_character()
{
	Glib::RefPtr<Gdk::Window> win = drawing_.get_window();
	Glib::RefPtr<Gdk::GC> gc = drawing_.get_style()->get_black_gc();

	stroke_num = 0;
	for (chrasis::Stroke::iterator si = cur_char_.strokes_begin();
	     si != cur_char_.strokes_end();
	     ++si)
	{
		// draw the stroke body
		gc->set_rgb_fg_color(colors[2]);
		for (chrasis::Point::iterator pi = si->points_begin();
		     pi != si->points_end() - 1;
		     ++pi)
		{
			win->draw_line( gc,
				static_cast<int>(pi->x()),
				static_cast<int>(pi->y()),
				static_cast<int>((pi+1)->x()),
				static_cast<int>((pi+1)->y())
			);
		}

		// draw a number before each stroke
		stringstream ss;
		ss << ++stroke_num;
		Glib::RefPtr<Pango::Layout> num = create_pango_layout(ss.str());
		int lx, ly;
		num->get_size(lx, ly);
		win->draw_layout(
			gc,
			static_cast<int>(si->points_begin()->x() - lx / Pango::SCALE - 5),
			static_cast<int>(si->points_begin()->y() - ly / Pango::SCALE - 5),
			num
		);

		// enchant the beginning and ending points
		chrasis::Point::iterator pi[2] =
			{ si->points_begin(), si->points_end() - 1 };

		for (int i=0;i<2;++i)
		{
			gc->set_rgb_fg_color(colors[i]);
			win->draw_arc( gc, true,
				static_cast<int>(pi[i]->x() - 3),
				static_cast<int>(pi[i]->y() - 3),
				5, 5, 0, 23040);
		}
	}
}

void
Trainer::update_candidate_list()
{
	refliststore_->clear();

	chrasis::character_possibility_t likely(recognize(normalize(cur_char_), db_));

	for (chrasis::character_possibility_t::iterator it = likely.begin();
	     it != likely.end();
	     ++it)
	{
		Gtk::TreeModel::Row row = *(refliststore_->append());
		row[col_char_] = it->second.second;
		row[col_diff_] = it->first;
		row[col_id_] = it->second.first;
	}
}

void
Trainer::learn_curchar()
{
	std::string fn(std::string("raw_data/") + utf8_to_hex(cur_char_.get_name()) + ".chml");

	chrasis::Character::collection cc = chrasis::read_chml(fn);
	cc.push_back(cur_char_);
	chrasis::write_chml(cc, fn);

	chrasis::learn(normalize(cur_char_), db_);
}
