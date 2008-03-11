/** @file scim-chrasis.cpp
 */

/*
 * Smart Common Input Method
 * 
 * Copyright (c) 2005 James Su <suzhe@tsinghua.org.cn>
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
 * $Id: scim_input_pad.cpp,v 1.12 2005/11/21 06:40:31 suzhe Exp $
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

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

using namespace scim;

#define scim_module_init			chrasis_LTX_scim_module_init
#define scim_module_exit			chrasis_LTX_scim_module_exit
#define scim_helper_module_number_of_helpers	chrasis_LTX_scim_helper_module_number_of_helpers
#define scim_helper_module_get_helper_info	chrasis_LTX_scim_helper_module_get_helper_info
#define scim_helper_module_run_helper		chrasis_LTX_scim_helper_module_run_helper

#define SCIM_CONFIG_HELPER_CHRASIS_NUM_OF_CHARS		"/Helper/Chrasis/NumOfChars"
#define SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_XPOS	"/Helper/Chrasis/MainWindowXPos"
#define SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_YPOS	"/Helper/Chrasis/MainWindowYPos"

#define SCIM_CHRASIS_ICON			(SCIM_ICONDIR "/chrasis.png")

static gboolean	main_window_delete_callback (GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean	drawingarea_expose_callback (GtkWidget *drawingarea, GdkEventExpose *event, gpointer user_data);
static void	drawingarea_mousedown_callback (GtkWidget *drawingarea, gpointer user_data);
static void	drawingarea_mouseup_callback (GtkWidget *drawingarea, gpointer user_data);
static gboolean	helper_agent_input_handler (GIOChannel *source, GIOCondition condition, gpointer user_data);
static void	slot_exit (const HelperAgent *, int ic, const String &uuid);
static void	slot_update_screen (const HelperAgent *, int ic, const String &uuid, int screen);
static void	slot_trigger_property (const HelperAgent *agent, int ic, const String &uuid, const String &property);
static void	run (const String &display);

static HelperAgent	helper_agent;
static GtkWidget	*main_window;

static gint num_of_chars	= 4;
static gint main_window_xpos	= 0;
static gint main_window_ypos	= 0;

static HelperInfo	helper_info(	String ("bf0a3c4d-244e-467f-b44e-27d74c840c30"),
					"",
					String (SCIM_CHRASIS_ICON),
					"",
					SCIM_HELPER_STAND_ALONE | SCIM_HELPER_NEED_SCREEN_INFO);

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

		if (uuid == "bf0a3c4d-244e-467f-b44e-27d74c840c30")
		{
			num_of_chars = config->read(
				String(SCIM_CONFIG_HELPER_CHRASIS_NUM_OF_CHARS),
				static_cast<int>(num_of_chars));
			main_window_xpos = config->read(
				String(SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_XPOS),
				static_cast<int>(main_window_xpos));
			main_window_ypos = config->read(
				String(SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_YPOS),
				static_cast<int>(main_window_ypos));

			run(display);

			config->write(
				String(SCIM_CONFIG_HELPER_CHRASIS_NUM_OF_CHARS),
				static_cast<int>(num_of_chars));
			config->write(
				String(SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_XPOS),
				static_cast<int>(main_window_xpos));
			config->write(
				String(SCIM_CONFIG_HELPER_CHRASIS_MAIN_WINDOW_YPOS),
				static_cast<int>(main_window_ypos));
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
	{
		gtk_main_quit();
	}

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
	{
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
}

static gboolean
drawingarea_expose_callback (GtkWidget *drawingarea, GdkEventExpose *event, gpointer user_data)
{
	int white(0xffffff), silver(0xc0c0c0);

	GdkGC *gc_white = gdk_gc_new(drawingarea->window);
	gdk_gc_set_rgb_fg_color(gc_white, reinterpret_cast<GdkColor *>(&white));
	gdk_draw_rectangle(
		drawingarea->window, gc_white,
		TRUE,
		0, 0,
		100, 100);

	GdkGC *gc_silver = gdk_gc_new(drawingarea->window);
	gdk_gc_set_rgb_fg_color(gc_silver, reinterpret_cast<GdkColor *>(&silver));
	gdk_draw_line(
		drawingarea->window, gc_silver,
		50, 0,
		50, 100);
	gdk_draw_line(
		drawingarea->window, gc_silver,
		0, 50,
		100, 50);
}

static gboolean
main_window_delete_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_window_get_position (GTK_WINDOW (widget), &main_window_xpos, &main_window_ypos);
    return FALSE;
}

static void
create_main_window()
{
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_accept_focus(GTK_WINDOW(main_window), FALSE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(main_window), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);
	gtk_window_set_keep_above(GTK_WINDOW(main_window), TRUE);
	gtk_window_stick(GTK_WINDOW(main_window));
	g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(main_window), "delete_event", G_CALLBACK(main_window_delete_callback), NULL);

	GtkWidget *hbox = gtk_hbox_new (FALSE, 4);
	gtk_widget_show(hbox);
	gtk_container_add(GTK_CONTAINER(main_window), hbox);

	for (int i=0;i<num_of_chars;++i)
	{
		GtkWidget *da = gtk_drawing_area_new();
		gtk_drawing_area_size(GTK_DRAWING_AREA(da), 100, 100);
		g_signal_connect(G_OBJECT(da), "expose_event", G_CALLBACK(drawingarea_expose_callback), NULL);
		gtk_widget_show(da);
		gtk_box_pack_start(GTK_BOX(hbox), da, FALSE, FALSE, 0);
	}

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

		g_io_add_watch(ch, G_IO_IN, helper_agent_input_handler, (gpointer) &helper_agent);
		g_io_add_watch (ch, G_IO_ERR, helper_agent_input_handler, (gpointer) &helper_agent);
		g_io_add_watch (ch, G_IO_HUP, helper_agent_input_handler, (gpointer) &helper_agent);
	}

	gtk_main();
}
