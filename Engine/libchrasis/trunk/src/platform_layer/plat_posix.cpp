/*
 * libchrasis
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
 * $Id: chmlcodec.h 18 2006-09-19 21:18:42Z palatis $
 */

#ifdef _POSIX

#include <unistd.h>

namespace chrasis
{

namespace platform
{

CHRASIS_API
std::string
system_database_path()
{
	return CHRASIS_DATADIR "/" DEFAULT_DB_FILE;
}

CHRASIS_API
std::string
empty_database_path()
{
	return CHRASIS_DATADIR "/" DEFAULT_EMPTYDB_FILE;
}

CHRASIS_API
std::string
user_database_path()
{
	std::string userdir = getenv("HOME");
	return userdir + "/.chrasis/" DEFAULT_DB_FILE;
}

CHRASIS_API
std::string
database_schema_path()
{
	return CHRASIS_DATADIR "/" DEFAULT_SCHEMA_FILE;
}

CHRASIS_API
bool
initialize_userdir()
{
	std::string userdir = getenv("HOME");
	userdir += "/.chrasis";

	if ( mkdir( userdir.c_str(), 0755 ) != 0 )
	{
		struct stat st;
		stat( userdir.c_str(), &st );

		if ( !S_ISDIR(st.st_mode) )
		{
			std::cerr << "Failed to create \"" << userdir << "\"." << std::endl;
			return false;
		}
	}

	if ( access( user_database_path().c_str(), W_OK) != 0 )
	{
		std::ifstream fin(
			empty_database_path().c_str(),
			std::ios_base::binary);
		std::ofstream fout(
			user_database_path().c_str(),
			std::ios_base::binary);
		fout << fin.rdbuf();
		fin.close();
		fout.close();
	}

	return true;
}

} // namespace platform

} // namespace chrasis

#endif // ifdef _POSIX