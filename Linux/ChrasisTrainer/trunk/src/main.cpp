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
 * $Id: main.cpp 18 2006-09-19 21:18:42Z palatis $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// for mkdir
#include <sys/stat.h>
#include <sys/types.h>

#include <gtkmm.h>
#include <chrasis.h>

#include <fstream>
#include <string>

#include "trainer.h"

using namespace std;

int main(int argc, char *argv[])
{
#ifdef ENABLE_NLS
	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, CHRASISTRAINER_LOCALEDIR);
	textdomain (GETTEXT_PACKAGE);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif

	// prepare chml directory
	if ( mkdir("raw_data", 0755) )
	{
		struct stat st;
		stat("raw_data", &st);

		if ( !S_ISDIR(st.st_mode) )
		{
			std::cerr << "Failed to create \"raw_data\" dir." << endl;
			return 1;
		}
	}
	
	// prepare database
	if ( !chrasis::fexist("chr_data.db") )
	{
		// copy from default location
		ifstream fin(
			chrasis::settings::default_database().c_str(),
			std::ios_base::binary);
		ofstream fout(
			"chr_data.db",
			std::ios_base::binary);
		fout << fin.rdbuf();
		fin.close();
		fout.close();
	}

	Gtk::Main kit(argc, argv);
	Trainer w;
	Gtk::Main::run(w);

	return 0;
}

