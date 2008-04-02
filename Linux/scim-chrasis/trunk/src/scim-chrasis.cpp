/** @file scim-chrasis.cpp
 */

/*
 * SCIM-Chrasis
 * 
 * Copyright (c) 2008 Victor Tseng <palatis@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <chrasis.h>

#include <wctype.h>
#include <gtk/gtk.h>

#define Uses_SCIM_UTILITY
#define Uses_SCIM_OBJECT
#define Uses_SCIM_POINTER
#define Uses_SCIM_EVENT
#define Uses_SCIM_HELPER
#define Uses_SCIM_CONFIG_BASE
#include <scim.h>

#if defined(HAVE_LIBINTL_H) && defined(ENABLE_NLS)
  #include <libintl.h>
  #define _(String) dgettext(GETTEXT_PACKAGE,String)
  #define N_(String) (String)
#else
  #define _(String) (String)
  #define N_(String) (String)
  #define bindtextdomain(Package,Directory)
  #define textdomain(domain)
  #define bind_textdomain_codeset(domain,codeset)
#endif

#include <set>

using namespace scim;

#define scim_module_init			chrasis_LTX_scim_module_init
#define scim_module_exit			chrasis_LTX_scim_module_exit
#define scim_helper_module_number_of_helpers	chrasis_LTX_scim_helper_module_number_of_helpers
#define scim_helper_module_get_helper_info	chrasis_LTX_scim_helper_module_get_helper_info
#define scim_helper_module_run_helper		chrasis_LTX_scim_helper_module_run_helper

#define SCIM_CONFIG_HELPER_CHRASIS_NUM_OF_CHARS		"/Helper/Chrasis/NumOfChars"
#define SCIM_CONFIG_HELPER_CHRASIS_DRAWINGAREA_SIZE	"/Helper/Chrasis/DrawingAreaSize"
#define SCIM_CONFIG_HELPER_CHRASIS_RECOGNIZE_DELAY	"/Helper/Chrasis/RecognizeDelay"
#define SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_XPOS	"/Helper/Chrasis/MainWindowXPos"
#define SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_YPOS	"/Helper/Chrasis/MainWindowYPos"
#define SCIM_CONFIG_HELPER_CHRASIS_SAVE_CHML		"/Helper/Chrasis/SaveCHML"
#define SCIM_CONFIG_HELPER_CHRASIS_LEARN_CHARACTER	"/Helper/Chrasis/LearnCharacter"

#define SCIM_CHRASIS_UUID	"bf0a3c4d-244e-467f-b44e-27d74c840c30"
#define SCIM_CHRASIS_ICON	(SCIM_ICONDIR "/scim-chrasis.png")
#define SCIM_CHRASIS_CHML_FILE	"/.scim/scim-chrasis-saved-characters.chml"

static void	send_key_event(GtkButton *button);
static gboolean	main_window_delete_callback (GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean	main_window_destroy_callback (GtkWidget *widget, GdkEvent *event, gpointer data);
static void	button_sendkey_pressed_callback (GtkWidget *button, gpointer user_data);
static void	button_sendkey_released_callback (GtkWidget *button, gpointer user_data);
static void	button_options_clicked_callback (GtkWidget *button, gpointer user_data);
static gboolean	button_repeat_timeout (gpointer data);
static gboolean	candidate_menu_popup_handler (GtkWidget *menu, GdkEvent *event);
static void	menu_choosen_callback (GtkWidget *menu, gpointer user_data);
static gboolean	drawingarea_expose_callback (GtkWidget *drawingarea, GdkEventExpose *event, gpointer user_data);
static gboolean	drawingarea_mousedown_callback (GtkWidget *drawingarea, GdkEventButton *event, gpointer user_data);
static gboolean	drawingarea_mouseup_callback (GtkWidget *drawingarea, GdkEventButton *event, gpointer user_data);
static gboolean drawingarea_mousemotion_callback(GtkWidget *drawingarea, GdkEventMotion *event, gpointer user_data);
static gboolean	recognize_timeout (gpointer user_data);
static gboolean	helper_agent_input_handler (GIOChannel *source, GIOCondition condition, gpointer user_data);
static void	slot_exit (const HelperAgent *, int ic, const String &uuid);
static void	slot_update_screen (const HelperAgent *, int ic, const String &uuid, int screen);
static void	slot_trigger_property (const HelperAgent *agent, int ic, const String &uuid, const String &property);
static void	create_main_window();
static void	run (const String &display);

static HelperAgent	helper_agent;
static GtkWidget	*main_window;

static gint	num_of_chars		= 4;
static gint	drawingarea_size	= 120;
static gint	recognize_delay		= 3000;
static gint	main_window_xpos	= 0;
static gint	main_window_ypos	= 0;
static gboolean	save_chml		= FALSE;
static gboolean learn_character		= FALSE;

static HelperInfo	helper_info(	String (SCIM_CHRASIS_UUID),
					"",
					String (SCIM_CHRASIS_ICON),
					"",
					SCIM_HELPER_STAND_ALONE | SCIM_HELPER_NEED_SCREEN_INFO);

static chrasis::Character::container chars;

static const int LINE_WIDTH = 2;

// Module Interface
extern "C" {
	void scim_module_init (void)
	{
		bindtextdomain (GETTEXT_PACKAGE, SCIM_CHRASIS_LOCALEDIR);
		bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");

		helper_info.name = String(_("Chinese Handwriting Recognition As-Is"));
		helper_info.description = String (_("A Chinese handwriting character recognition utility."));
	}

	void scim_module_exit (void)
	{
	}

	unsigned int scim_helper_module_number_of_helpers (void)
	{
		return 1;
	}

	bool scim_helper_module_get_helper_info (unsigned int idx, HelperInfo &info)
	{
		if (idx == 0)
		{
			info = helper_info;
			return true;
		}
		return false;
	}

	void scim_helper_module_run_helper (const String &uuid, const ConfigPointer &config, const String &display)
	{
		SCIM_DEBUG_MAIN(1) << "chrasis_LTX_scim_helper_module_run_helper ()\n";

		if (uuid == SCIM_CHRASIS_UUID)
		{
			num_of_chars = config->read(
				String(SCIM_CONFIG_HELPER_CHRASIS_NUM_OF_CHARS),
				static_cast<int>(num_of_chars));
			drawingarea_size = config->read(
				String(SCIM_CONFIG_HELPER_CHRASIS_DRAWINGAREA_SIZE),
				static_cast<int>(drawingarea_size));
			recognize_delay = config->read(
				String(SCIM_CONFIG_HELPER_CHRASIS_RECOGNIZE_DELAY),
				static_cast<int>(recognize_delay));
			main_window_xpos = config->read(
				String(SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_XPOS),
				static_cast<int>(main_window_xpos));
			main_window_ypos = config->read(
				String(SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_YPOS),
				static_cast<int>(main_window_ypos));
			save_chml = config->read(
				String(SCIM_CONFIG_HELPER_CHRASIS_SAVE_CHML),
				static_cast<bool>(save_chml));
			learn_character = config->read(
				String(SCIM_CONFIG_HELPER_CHRASIS_LEARN_CHARACTER),
				static_cast<bool>(learn_character));

			run(display);

			config->write(
				String(SCIM_CONFIG_HELPER_CHRASIS_NUM_OF_CHARS),
				static_cast<int>(num_of_chars));
			config->write(
				String(SCIM_CONFIG_HELPER_CHRASIS_DRAWINGAREA_SIZE),
				static_cast<int>(drawingarea_size));
			config->write(
				String(SCIM_CONFIG_HELPER_CHRASIS_RECOGNIZE_DELAY),
				static_cast<int>(recognize_delay));
			config->write(
				String(SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_XPOS),
				static_cast<int>(main_window_xpos));
			config->write(
				String(SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_YPOS),
				static_cast<int>(main_window_ypos));
			config->write(
				String(SCIM_CONFIG_HELPER_CHRASIS_SAVE_CHML),
				static_cast<bool>(save_chml));
			config->write(
				String(SCIM_CONFIG_HELPER_CHRASIS_LEARN_CHARACTER),
				static_cast<bool>(learn_character));
		}

		SCIM_DEBUG_MAIN(1) << "exit chrasis_LTX_scim_helper_module_run_helper ()\n";
	}
}

static gboolean
helper_agent_input_handler (GIOChannel *source, GIOCondition condition, gpointer user_data)
{
	if (condition == G_IO_IN)
	{
		HelperAgent *agent = static_cast<HelperAgent *> (user_data);
		if (agent && agent->has_pending_event())
			agent->filter_event();
	}
	else if (condition == G_IO_ERR || condition == G_IO_HUP)
		gtk_main_quit();

	return TRUE;
}

static void
slot_exit (const HelperAgent *, int ic, const String &uuid)
{
	gtk_main_quit();
}

static void
slot_update_screen (const HelperAgent *, int ic, const String &uuid, int screen)
{
	if (gdk_display_get_n_screens (gdk_display_get_default ()) > screen)
	{
		GdkScreen *scr = gdk_display_get_screen(gdk_display_get_default(), screen);
		if (scr)
			gtk_window_set_screen(GTK_WINDOW(main_window), scr);
	}
}

static void
slot_trigger_property (const HelperAgent *agent, int ic, const String &uuid, const String &property)
{
	if (property == "/Chrasis")
		if (GTK_WIDGET_VISIBLE (main_window))
		{
			gtk_window_get_position (GTK_WINDOW (main_window), &main_window_xpos, &main_window_ypos);
			gtk_widget_hide (main_window);
		}
		else
		{
			gtk_window_move (GTK_WINDOW (main_window), main_window_xpos, main_window_ypos);
			gtk_widget_show(main_window);
		}
}

static gboolean
drawingarea_expose_callback (GtkWidget *drawingarea, GdkEventExpose *event, gpointer user_data)
{
	chrasis::Character *c = static_cast<chrasis::Character *>(user_data);

#ifdef __GDK_CAIRO_H__
	cairo_t *cr = gdk_cairo_create (drawingarea->window);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
	cairo_set_line_width(cr, 1);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle (cr, 0, 0, drawingarea_size, drawingarea_size);
	cairo_fill (cr);

	double dash_pattern[] = {3.0};
	cairo_set_source_rgb (cr, 0.75, 0.75, 0.75);
	cairo_set_dash (cr, dash_pattern, 1, 0);
	cairo_move_to (cr, drawingarea_size/2, 0);
	cairo_line_to (cr, drawingarea_size/2, drawingarea_size);
	cairo_move_to (cr, 0, drawingarea_size/2);
	cairo_line_to (cr, drawingarea_size, drawingarea_size/2);
	cairo_stroke (cr);

	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_set_dash (cr, NULL, 0, 0);
	cairo_rectangle (cr, 0, 0, drawingarea_size-1, drawingarea_size-1);
	cairo_stroke (cr);

	cairo_set_source_rgb (cr, 0.3, 0.3, 0.3);
	cairo_set_line_width(cr, LINE_WIDTH);
	if (g_object_get_data (G_OBJECT (drawingarea), "draw_character") == (gpointer) 1)
	{
		for (chrasis::Stroke::iterator si = c->strokes_begin();
		     si != c->strokes_end();
		     ++si)
		{
			if (si->points_begin() != si->points_end())
			{
				cairo_move_to (cr, si->points_begin()->x(), si->points_begin()->y());
				cairo_line_to (cr, si->points_begin()->x(), si->points_begin()->y());
			}
			for (chrasis::Point::iterator pi = si->points_begin() + 1;
			     pi != si->points_end();
			     ++pi)
				cairo_line_to (cr, pi->x(), pi->y());
		}
	}
	cairo_stroke (cr);

	cairo_destroy (cr);
#else
	GdkColor black, white, silver, grey;
	black.red = black.green = black.blue = 0x0000;
	white.red = white.green = white.blue = 0xffff;
	silver.red = silver.green = silver.blue = 0xc000;
	grey.red = grey.green = grey.blue = 0x5555;

	GdkGC *gc = gdk_gc_new(drawingarea->window);

	gdk_gc_set_rgb_fg_color(gc, &white);
	gdk_draw_rectangle(drawingarea->window, gc, TRUE, 0, 0, drawingarea_size, drawingarea_size);

	gdk_gc_set_rgb_fg_color(gc, &silver);
	gdk_gc_set_line_attributes(gc, 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_ROUND, GDK_JOIN_ROUND);
	gdk_draw_line(drawingarea->window, gc, drawingarea_size/2, 0, drawingarea_size/2, drawingarea_size);
	gdk_draw_line(drawingarea->window, gc, 0, drawingarea_size/2, drawingarea_size, drawingarea_size/2);

	gdk_gc_set_rgb_fg_color(gc, &black);
	gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
	gdk_draw_rectangle(drawingarea->window, gc, FALSE, 0, 0, drawingarea_size-1, drawingarea_size-1);

	gdk_gc_set_rgb_fg_color(gc, &grey);
	gdk_gc_set_line_attributes(gc, LINE_WIDTH, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);

	if (g_object_get_data (G_OBJECT (drawingarea), "draw_character") == (gpointer) 1)
	{
		for (chrasis::Stroke::iterator si = c->strokes_begin();
		     si != c->strokes_end();
		     ++si)
			for (chrasis::Point::iterator pi = si->points_begin();
			     pi != si->points_end() - 1;
			     ++pi)
				gdk_draw_line(drawingarea->window, gc,
					static_cast<int>(pi->x()), static_cast<int>(pi->y()),
					static_cast<int>((pi+1)->x()), static_cast<int>((pi+1)->y())
				);
	}

	g_object_unref(G_OBJECT(gc));
#endif
}


static void
menu_cleaner (GtkWidget *child, gpointer)
{
	gtk_widget_destroy (child);
}

static void
_populate_candidate_list(GtkMenu *menu, GtkButton *button, chrasis::Character const & c)
{
	chrasis::platform::initialize_userdir();

	// clear candidate list menu
	gtk_container_foreach (GTK_CONTAINER (menu), menu_cleaner, NULL);

	// set new candidate list menu
	chrasis::ItemPossibilityList likely(recognize(normalize(c)));
	likely.sort(chrasis::ItemPossibilityList::SORTING_POSSIBILITY);
	GtkWidget *item;
	if (!likely.empty())
	{
		gtk_button_set_label (GTK_BUTTON (button), likely.begin()->name.c_str());
		std::set< std::string > char_bank;
		for (chrasis::ItemPossibilityList::const_iterator li = likely.begin();
		     li != likely.end();
		     ++li)
		{
			if (char_bank.find(li->name) == char_bank.end())
			{
				item = gtk_menu_item_new_with_label(li->name.c_str());
				gtk_widget_show(item);
				gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
			}
			char_bank.insert(li->name);
		}
	}
	else
		gtk_button_set_label (GTK_BUTTON (button), " ");

	item = gtk_separator_menu_item_new();
	gtk_widget_show(item);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

	item = gtk_menu_item_new_with_mnemonic(_("Clear"));
	gtk_widget_show(item);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
	g_object_set_data (G_OBJECT (menu), "menu_item_clear", (gpointer) item);

	item = gtk_menu_item_new_with_mnemonic(_("Incorrect"));
	gtk_widget_show(item);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
	g_object_set_data (G_OBJECT (menu), "menu_item_incorrect", (gpointer) item);
}

double last_x, last_y;

static gboolean
drawingarea_mousemotion_callback(GtkWidget *drawingarea, GdkEventMotion *event, gpointer user_data)
{
	if (event->state & GDK_BUTTON1_MASK)
	{
		chrasis::Character *c = static_cast<chrasis::Character *>(user_data);

		c->add_point(event->x, event->y);

#ifdef __GDK_CAIRO_H__
		cairo_t *cr = gdk_cairo_create (drawingarea->window);
		cairo_set_source_rgb (cr, 0.3, 0.3, 0.3);
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
		cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
		cairo_set_line_width(cr, LINE_WIDTH);

		cairo_move_to(cr, last_x, last_y);
		cairo_line_to(cr, event->x, event->y);
		cairo_stroke(cr);

		cairo_destroy (cr);
#else
		GdkColor grey;
		grey.red = grey.green = grey.blue = 0x5555;

		GdkGC *gc = gdk_gc_new(drawingarea->window);
		gdk_gc_set_rgb_fg_color(gc, &grey);
		gdk_gc_set_line_attributes(gc, LINE_WIDTH, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
		gdk_draw_line(drawingarea->window, gc,
			static_cast<gint>(last_x), static_cast<gint>(last_y),
			static_cast<gint>(event->x), static_cast<gint>(event->y));
		g_object_unref(G_OBJECT(gc));
#endif

		last_x = event->x;
		last_y = event->y;

		return TRUE;
	}
	return FALSE;
}

static gboolean
drawingarea_mousedown_callback (GtkWidget *drawingarea, GdkEventButton *event, gpointer user_data)
{
	if (event->button == 1)
	{
		chrasis::Character *c = static_cast<chrasis::Character *>(user_data);

		c->new_stroke();
		c->add_point(event->x, event->y);
		last_x = event->x;
		last_y = event->y;

#ifdef __GDK_CAIRO_H__
		cairo_t *cr = gdk_cairo_create (drawingarea->window);
		cairo_set_source_rgb (cr, 0.3, 0.3, 0.3);
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
		cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
		cairo_set_line_width(cr, LINE_WIDTH);

		cairo_move_to(cr, event->x, event->y);
		cairo_line_to(cr, event->x, event->y);
		cairo_stroke(cr);

		cairo_destroy (cr);
#else
		GdkColor grey;
		grey.red = grey.green = grey.blue = 0x5555;

		GdkGC *gc = gdk_gc_new(drawingarea->window);
		gdk_gc_set_rgb_fg_color(gc, &grey);
		gdk_gc_set_line_attributes(gc, 2, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
		gdk_draw_line(drawingarea->window, gc,
			static_cast<gint>(event->x), static_cast<gint>(event->y),
			static_cast<gint>(event->x), static_cast<gint>(event->y));
		g_object_unref(G_OBJECT(gc));
#endif

		GtkWidget *button = GTK_WIDGET (g_object_get_data (G_OBJECT (drawingarea), "button_candidate_list"));
		GtkWidget *menu = GTK_WIDGET (g_object_get_data (G_OBJECT (button), "menu_candidate_list"));

		guint id = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (menu), "recognize_timeout_id"));
		g_source_remove (id);

		return TRUE;
	}
	return FALSE;
}

static gboolean
drawingarea_mouseup_callback (GtkWidget *drawingarea, GdkEventButton *event, gpointer user_data)
{
	if (event->button == 1)
	{
		chrasis::Character *c = static_cast<chrasis::Character *>(user_data);

		GtkWidget *button = GTK_WIDGET (g_object_get_data (G_OBJECT (drawingarea), "button_candidate_list"));
		GtkWidget *menu = GTK_WIDGET (g_object_get_data (G_OBJECT (button), "menu_candidate_list"));

		_populate_candidate_list (GTK_MENU (menu), GTK_BUTTON (button), *c);

		guint id = g_timeout_add(recognize_delay, recognize_timeout, (gpointer) drawingarea);
		g_object_set_data (G_OBJECT (menu), "recognize_timeout_id", (gpointer) id);

		return TRUE;
	}
	return FALSE;
}

static gboolean
candidate_menu_popup_handler (GtkWidget *menu, GdkEvent *event)
{
	g_return_val_if_fail (menu != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_MENU (menu), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	if (event->type == GDK_BUTTON_PRESS)
	{
		GdkEventButton *event_button = (GdkEventButton *) event;

		if (event_button->button != 1)
			return FALSE;
		if (gtk_menu_get_active (GTK_MENU (menu)) == NULL)
			return FALSE;

		guint id = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (menu), "recognize_timeout_id"));
		if (id) g_source_remove (id);

		gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, event_button->button, event_button->time);

		return TRUE;
	}
	return FALSE;
}

static void
menu_choosen_callback (GtkWidget *menu, gpointer user_data)
{
	GtkWidget *button = GTK_WIDGET(g_object_get_data(G_OBJECT(user_data), "button_candidate_list"));
	GtkWidget *item = gtk_menu_get_active(GTK_MENU(menu));
	GtkWidget *item_incorrect = GTK_WIDGET(g_object_get_data(G_OBJECT(menu), "menu_item_incorrect"));
	GtkWidget *item_clear = GTK_WIDGET(g_object_get_data(G_OBJECT(menu), "menu_item_clear"));

	if (item == item_incorrect)
	{
		GtkWidget *dialog = gtk_dialog_new_with_buttons (
			_("Correct Character"), NULL,
			GTK_DIALOG_MODAL,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
			NULL);
		gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);

		GtkWidget *label = gtk_label_new_with_mnemonic (_("Input correct character:"));
		gtk_widget_show(label);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), label, FALSE, FALSE, 0);

		GtkWidget *entry = gtk_entry_new();
		gtk_widget_show(entry);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), entry, FALSE, FALSE, 0);

		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT &&
		    String (gtk_entry_get_text (GTK_ENTRY (entry))) != "")
		{
			chrasis::Character *c = static_cast<chrasis::Character *>(g_object_get_data(G_OBJECT(menu), "character"));
			chrasis::Character nc(normalize(*c));
			nc.set_name (String (gtk_entry_get_text (GTK_ENTRY (entry))));
			learn(nc);

			_populate_candidate_list (GTK_MENU (menu), GTK_BUTTON (button), *c);

			item = gtk_menu_get_active (GTK_MENU (menu));
			item_incorrect = GTK_WIDGET(g_object_get_data(G_OBJECT(menu), "menu_item_incorrect"));
		}
		else
			gtk_menu_shell_select_first (GTK_MENU_SHELL (menu), TRUE);

		gtk_widget_destroy (dialog);

		if (item == item_incorrect)
		{
			gtk_button_set_label (GTK_BUTTON (button), " ");
			guint id = g_timeout_add(recognize_delay, recognize_timeout, user_data);
			g_object_set_data (G_OBJECT (menu), "recognize_timeout_id", (gpointer) id);
			return;
		}
	}
	else if (item == item_clear)
	{
		gtk_button_set_label (GTK_BUTTON (button), " ");
		recognize_timeout(user_data);	// invoke this to clear drawing area and candidate list
		return;
	}

	GtkWidget *label = gtk_bin_get_child (GTK_BIN (item));
	gtk_button_set_label (GTK_BUTTON (button), gtk_label_get_text (GTK_LABEL (label)));

	guint id = g_timeout_add(recognize_delay, recognize_timeout, user_data);
	g_object_set_data (G_OBJECT (menu), "recognize_timeout_id", (gpointer) id);
}

static void
button_sendkey_pressed_callback (GtkWidget *button, gpointer user_data)
{
	send_key_event(GTK_BUTTON(button));

	guint id = g_timeout_add (600, button_repeat_timeout, button);
	g_object_set_data (G_OBJECT (button), "button_repeat_timeout_id", (gpointer) id);
	g_object_set_data (G_OBJECT (button), "initial_pressed", (gpointer) 1);
}

static void
button_sendkey_released_callback (GtkWidget *button, gpointer user_data)
{
	guint id = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (button), "button_repeat_timeout_id"));
	g_source_remove (id);
	g_object_set_data (G_OBJECT (button), "initial_pressed", (gpointer) 0);
}

static gboolean
main_window_delete_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_window_get_position (GTK_WINDOW (widget), &main_window_xpos, &main_window_ypos);
	return FALSE;
}

static gboolean
main_window_destroy_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	if (g_object_get_data (G_OBJECT (widget), "noquit-on-destroy") == (gpointer) 1)
		return TRUE;

	gtk_main_quit();
}

static void
send_key_event(GtkButton *button)
{
	if (helper_agent.get_connection_number () < 0)
		return;

	uint32 code = static_cast <uint32> (GPOINTER_TO_INT (g_object_get_data (G_OBJECT (button), "element_keycode")));
	uint16 mask = static_cast <uint16> (GPOINTER_TO_INT ((size_t)g_object_get_data (G_OBJECT (button), "element_keymask")));
	KeyEvent key (code, (mask & ~SCIM_KEY_ReleaseMask));
	KeyEvent key_release (code, (mask | SCIM_KEY_ReleaseMask));
        if (!key.empty ())
	{
		helper_agent.send_key_event (-1, "", key);
		helper_agent.send_key_event (-1, "", key_release);
	}
}

static gboolean
button_repeat_timeout (gpointer data)
{
	GtkButton *button = GTK_BUTTON (data);

	send_key_event(button);

	if (g_object_get_data (G_OBJECT (button), "initial_pressed") == (gpointer) 1)
	{
		guint id = g_timeout_add (1000/15, button_repeat_timeout, button);
		g_object_set_data (G_OBJECT (button), "button_repeat_timeout_id", (gpointer) id);
		g_object_set_data (G_OBJECT (button), "initial_pressed", (gpointer) 0);
		return FALSE;
	}
	return TRUE;
}

static inline void
_write_char_to_file (chrasis::Character const & c)
{
	String fn = scim_get_home_dir() + String(SCIM_CHRASIS_CHML_FILE);
	std::ofstream fout(fn.c_str(), std::ios_base::out | std::ios_base::app);

	if (fout)
	{
		fout << "<character name=\"" << c.get_name() << "\">\n";
		for (chrasis::Stroke::const_iterator si = c.strokes_begin();
		     si != c.strokes_end();
		     ++si)
		{
			fout << "  <stroke>\n";
			for (chrasis::Point::const_iterator pi = si->points_begin();
			     pi != si->points_end();
			     ++pi)
				fout << "    <point x=\"" << pi->x() << "\" y=\"" << pi->y() << "\"/>\n";
			fout << "  </stroke>\n";
		}
		fout << "</character>\n";
	}

	fout.close();
}

static gboolean
recognize_timeout (gpointer user_data)
{
	GtkWidget *drawingarea = GTK_WIDGET (user_data);
	GtkWidget *button = GTK_WIDGET(g_object_get_data(G_OBJECT(drawingarea), "button_candidate_list"));
	chrasis::Character *c = static_cast<chrasis::Character *>(g_object_get_data(G_OBJECT(drawingarea), "character"));

	// send text
	gchar const * text = gtk_button_get_label (GTK_BUTTON (button));
	if (text != NULL)
	{
		String s_text(text);
		if (s_text != " ")
		{
			c->set_name(s_text);
			if (save_chml)
				_write_char_to_file (*c);
			if (learn_character)
				learn(normalize(*c));

			helper_agent.commit_string(-1, "", scim::utf8_mbstowcs(text));
		}
	}

	// clear menu
	gtk_button_set_label(GTK_BUTTON(button), " ");
	GtkWidget *menu = GTK_WIDGET (g_object_get_data (G_OBJECT (button), "menu_candidate_list"));
	gtk_container_foreach (GTK_CONTAINER (menu), menu_cleaner, NULL);

	// clear drawingarea
	g_object_set_data(G_OBJECT(drawingarea), "draw_character", (gpointer) 0);
	gtk_widget_queue_draw(drawingarea);
	g_object_set_data(G_OBJECT(drawingarea), "draw_character", (gpointer) 1);

	// clear character buffer
	*c = chrasis::Character();

	return FALSE;
}

static void
button_options_clicked_callback (GtkWidget *button, gpointer user_data)
{
	GtkWidget *dialog = gtk_dialog_new_with_buttons (
		_("Chrasis Options"), NULL,
		GTK_DIALOG_MODAL,
		GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
		NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_CLOSE);
	gtk_container_set_border_width (GTK_CONTAINER (dialog), 8);

	GtkWidget *table = gtk_table_new(5, 2, FALSE);
	gtk_widget_show(table);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), table, FALSE, FALSE, 0);

	GtkWidget *label;

	// Number of Characters
	label = gtk_label_new_with_mnemonic (_("_Writing areas:"));
	gtk_widget_show (label);
	gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
	gtk_misc_set_padding (GTK_MISC (label), 2, 0);

	GtkWidget *spin_button_num_of_chars = gtk_spin_button_new_with_range (1, 8, 1);
	gtk_widget_show (spin_button_num_of_chars);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin_button_num_of_chars), TRUE);
	gtk_spin_button_set_snap_to_ticks (GTK_SPIN_BUTTON (spin_button_num_of_chars), TRUE);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON (spin_button_num_of_chars), 0);
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), spin_button_num_of_chars);

	gtk_table_attach_defaults(GTK_TABLE(table), label,				0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(table), spin_button_num_of_chars,		1, 2, 0, 1);

	// Drawing Area Size
	label = gtk_label_new_with_mnemonic ( _("Writing area _size:"));
	gtk_widget_show (label);
	gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
	gtk_misc_set_padding (GTK_MISC (label), 2, 0);

	GtkWidget *spin_button_drawingarea_size = gtk_spin_button_new_with_range (50, 500, 1);
	gtk_widget_show (spin_button_drawingarea_size);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin_button_drawingarea_size), TRUE);
	gtk_spin_button_set_snap_to_ticks (GTK_SPIN_BUTTON (spin_button_drawingarea_size), TRUE);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON (spin_button_drawingarea_size), 0);
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), spin_button_drawingarea_size);

	gtk_table_attach_defaults(GTK_TABLE(table), label,				0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), spin_button_drawingarea_size,	1, 2, 1, 2);

	// Recognize Delay
	label = gtk_label_new_with_mnemonic (_("Recognize _delay (ms):"));
	gtk_widget_show (label);
	gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
	gtk_misc_set_padding (GTK_MISC (label), 2, 0);

	GtkWidget *spin_button_recognize_delay = gtk_spin_button_new_with_range (100, 60000, 1);
	gtk_widget_show (spin_button_recognize_delay);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin_button_recognize_delay), TRUE);
	gtk_spin_button_set_snap_to_ticks (GTK_SPIN_BUTTON (spin_button_recognize_delay), TRUE);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON (spin_button_recognize_delay), 0);
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), spin_button_recognize_delay);

	gtk_table_attach_defaults(GTK_TABLE(table), label,				0, 1, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(table), spin_button_recognize_delay,	1, 2, 2, 3);
	
	// Save CHML
	GtkWidget *check_button_save_chml = gtk_check_button_new_with_mnemonic(_("Save written character to _file."));
	gtk_widget_show(check_button_save_chml);
	gtk_table_attach_defaults(GTK_TABLE(table), check_button_save_chml,		0, 2, 3, 4);

	// Learn Character
	GtkWidget *check_button_learn_character = gtk_check_button_new_with_mnemonic(_("_Learn recognized character into database."));
	gtk_widget_show(check_button_learn_character);
	gtk_table_attach_defaults(GTK_TABLE(table), check_button_learn_character,	0, 2, 4, 5);

	gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin_button_num_of_chars), num_of_chars);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin_button_drawingarea_size), drawingarea_size);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin_button_recognize_delay), recognize_delay);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button_save_chml), save_chml);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button_learn_character), learn_character);

	gtk_dialog_run (GTK_DIALOG (dialog));

	gint new_num_of_chars = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spin_button_num_of_chars));
	gint new_drawingarea_size = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spin_button_drawingarea_size));
	recognize_delay = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spin_button_recognize_delay));
	save_chml = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check_button_save_chml));
	learn_character = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check_button_learn_character));
		
	if (num_of_chars != new_num_of_chars || drawingarea_size != new_drawingarea_size)
	{
		g_object_set_data(G_OBJECT(main_window), "noquit-on-destroy", (gpointer) 1);
		gtk_window_get_position (GTK_WINDOW (main_window), &main_window_xpos, &main_window_ypos);
		gtk_widget_destroy(main_window);
		g_object_set_data(G_OBJECT(main_window), "noquit-on-destroy", (gpointer) 0);

		num_of_chars = new_num_of_chars;
		drawingarea_size = new_drawingarea_size;

		create_main_window();
	}

	gtk_widget_destroy (dialog);
}

static void
create_main_window()
{
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), _("Chrasis Writing Pad"));
	gtk_window_set_accept_focus(GTK_WINDOW(main_window), FALSE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(main_window), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);
	gtk_window_set_keep_above(GTK_WINDOW(main_window), TRUE);
	gtk_window_stick(GTK_WINDOW(main_window));
	g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(main_window_destroy_callback), NULL);
	g_signal_connect(G_OBJECT(main_window), "delete_event", G_CALLBACK(main_window_delete_callback), NULL);

	GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show(hbox);
	gtk_container_add(GTK_CONTAINER(main_window), hbox);

	chars.resize(num_of_chars);

	for (int i=0;i<num_of_chars;++i)
	{
		GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
		gtk_widget_show(vbox);

		GtkWidget *button = gtk_button_new_with_label(" ");
		gtk_widget_show(button);

		GtkWidget *menu = gtk_menu_new();

		GtkWidget *da = gtk_drawing_area_new();
		gtk_widget_set_size_request(da, drawingarea_size, drawingarea_size);
		gtk_widget_show(da);

		g_signal_connect(G_OBJECT(da), "expose-event", G_CALLBACK(drawingarea_expose_callback), &chars[i]);
		gtk_widget_add_events(da, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
		g_signal_connect(G_OBJECT(da), "button-press-event", G_CALLBACK(drawingarea_mousedown_callback), &chars[i]);
		g_signal_connect(G_OBJECT(da), "button-release-event", G_CALLBACK(drawingarea_mouseup_callback), &chars[i]);
		g_signal_connect(G_OBJECT(da), "motion-notify-event", G_CALLBACK(drawingarea_mousemotion_callback), &chars[i]);
		g_object_set_data(G_OBJECT(da), "draw_character", (gpointer) 1);
		g_object_set_data(G_OBJECT(da), "button_candidate_list", (gpointer) button);
		g_object_set_data(G_OBJECT(da), "character", (gpointer) &chars[i]);

		gtk_menu_shell_set_take_focus (GTK_MENU_SHELL(menu), FALSE);
		g_signal_connect(G_OBJECT(menu), "cancel", G_CALLBACK(menu_choosen_callback), (gpointer) da);
		g_signal_connect(G_OBJECT(menu), "selection-done", G_CALLBACK(menu_choosen_callback), (gpointer) da);
		g_object_set_data(G_OBJECT(menu), "character", (gpointer) &chars[i]);

		g_object_set_data(G_OBJECT(button), "menu_candidate_list", (gpointer) menu);
		g_signal_connect_swapped(button, "button-press-event", G_CALLBACK(candidate_menu_popup_handler), menu);

		gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(vbox), da, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
	}

	GtkWidget *table = gtk_table_new(4, 2, TRUE);
	gtk_widget_show(table);

	// KUSO: copy/paste programming start
	GtkWidget *button_backsp = gtk_button_new_with_label(_("BackSp"));
	KeyEvent key_backsp ("BackSpace");
	g_object_set_data(G_OBJECT(button_backsp), "element_keycode", (gpointer) ((size_t) key_backsp.code));
	g_object_set_data(G_OBJECT(button_backsp), "element_keymask", (gpointer) ((size_t) key_backsp.mask));
	g_signal_connect(G_OBJECT(button_backsp), "pressed", G_CALLBACK(button_sendkey_pressed_callback), NULL);
	g_signal_connect(G_OBJECT(button_backsp), "released", G_CALLBACK(button_sendkey_released_callback), NULL);
	gtk_widget_show(button_backsp);

	GtkWidget *button_del = gtk_button_new_with_label(_("Del"));
	KeyEvent key_del ("Delete");
	g_object_set_data(G_OBJECT(button_del), "element_keycode", (gpointer) ((size_t) key_del.code));
	g_object_set_data(G_OBJECT(button_del), "element_keymask", (gpointer) ((size_t) key_del.mask));
	g_signal_connect(G_OBJECT(button_del), "pressed", G_CALLBACK(button_sendkey_pressed_callback), NULL);
	g_signal_connect(G_OBJECT(button_del), "released", G_CALLBACK(button_sendkey_released_callback), NULL);
	gtk_widget_show(button_del);

	GtkWidget *button_tab = gtk_button_new_with_label(_("Tab"));
	KeyEvent key_tab ("Tab");
	g_object_set_data(G_OBJECT(button_tab), "element_keycode", (gpointer) ((size_t) key_tab.code));
	g_object_set_data(G_OBJECT(button_tab), "element_keymask", (gpointer) ((size_t) key_tab.mask));
	g_signal_connect(G_OBJECT(button_tab), "pressed", G_CALLBACK(button_sendkey_pressed_callback), NULL);
	g_signal_connect(G_OBJECT(button_tab), "released", G_CALLBACK(button_sendkey_released_callback), NULL);
	gtk_widget_show(button_tab);

	GtkWidget *button_enter = gtk_button_new_with_label(_("Enter"));
	KeyEvent key_enter ("Return");
	g_object_set_data(G_OBJECT(button_enter), "element_keycode", (gpointer) ((size_t) key_enter.code));
	g_object_set_data(G_OBJECT(button_enter), "element_keymask", (gpointer) ((size_t) key_enter.mask));
	g_signal_connect(G_OBJECT(button_enter), "pressed", G_CALLBACK(button_sendkey_pressed_callback), NULL);
	g_signal_connect(G_OBJECT(button_enter), "released", G_CALLBACK(button_sendkey_released_callback), NULL);
	gtk_widget_show(button_enter);

	GtkWidget *button_space = gtk_button_new_with_label(_("Space"));
	KeyEvent key_space ("KP_Space");
	g_object_set_data(G_OBJECT(button_space), "element_keycode", (gpointer) ((size_t) key_space.code));
	g_object_set_data(G_OBJECT(button_space), "element_keymask", (gpointer) ((size_t) key_space.mask));
	g_signal_connect(G_OBJECT(button_space), "pressed", G_CALLBACK(button_sendkey_pressed_callback), NULL);
	g_signal_connect(G_OBJECT(button_space), "released", G_CALLBACK(button_sendkey_released_callback), NULL);
	gtk_widget_show(button_space);

	GtkWidget *hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox2);

	GtkWidget *button_left = gtk_button_new_with_label(_("<-"));
	KeyEvent key_left ("KP_Left");
	g_object_set_data(G_OBJECT(button_left), "element_keycode", (gpointer) ((size_t) key_left.code));
	g_object_set_data(G_OBJECT(button_left), "element_keymask", (gpointer) ((size_t) key_left.mask));
	g_signal_connect(G_OBJECT(button_left), "pressed", G_CALLBACK(button_sendkey_pressed_callback), NULL);
	g_signal_connect(G_OBJECT(button_left), "released", G_CALLBACK(button_sendkey_released_callback), NULL);
	gtk_widget_show(button_left);

	GtkWidget *button_right = gtk_button_new_with_label(_("->"));
	KeyEvent key_right ("KP_Right");
	g_object_set_data(G_OBJECT(button_right), "element_keycode", (gpointer) ((size_t) key_right.code));
	g_object_set_data(G_OBJECT(button_right), "element_keymask", (gpointer) ((size_t) key_right.mask));
	g_signal_connect(G_OBJECT(button_right), "pressed", G_CALLBACK(button_sendkey_pressed_callback), NULL);
	g_signal_connect(G_OBJECT(button_right), "released", G_CALLBACK(button_sendkey_released_callback), NULL);
	gtk_widget_show(button_right);
	// KUSO: copy/paste programming end

	GtkWidget *button_options = gtk_button_new_with_label(_("Options..."));
	g_signal_connect(G_OBJECT(button_options), "clicked", G_CALLBACK(button_options_clicked_callback), NULL);
	gtk_widget_show(button_options);

	gtk_table_attach_defaults(GTK_TABLE(table), button_backsp,	0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(table), button_del,		1, 2, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(table), button_tab,		0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), button_enter,	1, 2, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), button_space,	0, 1, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(table), hbox2,		1, 2, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(table), button_options,	0, 2, 3, 4);
	gtk_box_pack_start(GTK_BOX(hbox2), button_left, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), button_right, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), table, FALSE, FALSE, 0);

	gint scrh = gdk_screen_get_height (gtk_widget_get_screen (main_window));
	gint scrw = gdk_screen_get_width  (gtk_widget_get_screen (main_window));

	GtkRequisition requisition;
	gtk_widget_size_request (GTK_WIDGET (main_window), &requisition);

	if (main_window_xpos < 0)
		main_window_xpos = 0;
	else if (main_window_xpos > scrw - requisition.width)
	        main_window_xpos = scrw - requisition.width;

	if (main_window_ypos < 0)
		main_window_ypos  =0;
	else if (main_window_ypos > scrh - requisition.height)
		main_window_ypos = scrh - requisition.height;

	gtk_window_move (GTK_WINDOW (main_window), main_window_xpos, main_window_ypos);
	gtk_widget_show (main_window);
}

void
run (const String &display)
{
	char **argv = new char * [4];
	int argc = 3;

	argv[0] = const_cast<char *>("chrasis");
	argv[1] = const_cast<char *>("--display");
	argv[2] = const_cast<char *> (display.c_str());
	argv[3] = 0;

	setenv ("DISPLAY", display.c_str(), 1);

	gtk_init(&argc, &argv);

	create_main_window();

	helper_agent.signal_connect_exit (slot (slot_exit));
	helper_agent.signal_connect_update_screen (slot (slot_update_screen));
	helper_agent.signal_connect_trigger_property (slot (slot_trigger_property));

	int fd = helper_agent.open_connection (helper_info, display);
	GIOChannel *ch = g_io_channel_unix_new (fd);

	if (fd >= 0 && ch)
	{
		Property prop (
			"/Chrasis",
			_("Chrasis Writing Pad"),
			SCIM_CHRASIS_ICON,
			_("Show/Hide Chinese handwriting recognition pad")
		);
		PropertyList props;
		props.push_back (prop);
		helper_agent.register_properties (props);

		g_io_add_watch (ch, G_IO_IN, helper_agent_input_handler, (gpointer) &helper_agent);
		g_io_add_watch (ch, G_IO_ERR, helper_agent_input_handler, (gpointer) &helper_agent);
		g_io_add_watch (ch, G_IO_HUP, helper_agent_input_handler, (gpointer) &helper_agent);
	}

	gtk_main();

	delete [] argv;
}
