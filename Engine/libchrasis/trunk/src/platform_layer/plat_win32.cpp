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

#ifdef _WINDOWS

#include <chrasis.h>

#include <windows.h>
#include <io.h>
#include <direct.h>
#include <sys/stat.h>

namespace chrasis
{

namespace platform
{

// return a registry Value from type REG_SZ;
CHRASIS_INTERNAL
const std::string
RegGetValue(HKEY hKey, LPCWSTR cPath, LPCWSTR cValuetoGet)
{
	char	FoundValue[256]	= {0};
	BYTE	ValueData[1024]	= {0};
	DWORD	Type		= 0;
	DWORD	iValueData	= sizeof(ValueData)-1;
	DWORD	iFoundValue	= sizeof(FoundValue)-1;
	HKEY	hKeyOpen;

	if (ERROR_SUCCESS == RegOpenKeyEx(hKey, cPath, 0, KEY_ALL_ACCESS, &hKeyOpen))
	{
		if(RegQueryValueEx(hKeyOpen, cValuetoGet, NULL,
							&Type, ValueData, &iValueData) == ERROR_SUCCESS)
		{
			RegCloseKey(hKeyOpen);
			if(Type == REG_SZ)
				return std::string((char*)ValueData);
			else
				return "";
		}
		RegCloseKey(hKeyOpen);
	}
	return "";
}

CHRASIS_API
std::string
system_database_path()
{
	static std::string sysdb(
		RegGetValue(
			HKEY_LOCAL_MACHINE,
			reinterpret_cast<LPCWSTR>("Software\\Chrasis"),
			reinterpret_cast<LPCWSTR>("InstallPath")
		) + "\\" DEFAULT_DB_FILE);
	return sysdb;
}

CHRASIS_API
std::string
empty_database_path()
{
	static std::string emptydb(
		RegGetValue(
			HKEY_LOCAL_MACHINE,
			reinterpret_cast<LPCWSTR>("Software\\Chrasis"),
			reinterpret_cast<LPCWSTR>("InstallPath")
		) + "\\" DEFAULT_EMPTYDB_FILE);
	return emptydb;
}

CHRASIS_API
std::string
user_database_path()
{
	std::string userdir = getenv("APPDATA");
	return userdir + "\\Chrasis\\" DEFAULT_DB_FILE;
}

CHRASIS_API
std::string
database_schema_path()
{
	static std::string schema(
		RegGetValue(
			HKEY_LOCAL_MACHINE,
			reinterpret_cast<LPCWSTR>("Software\\Chrasis"),
			reinterpret_cast<LPCWSTR>("InstallPath")
		) + "\\" DEFAULT_SCHEMA_FILE);
	return schema;
}

CHRASIS_API
bool
initialize_userdir()
{
	std::string userdir = getenv("APPDATA");
	userdir += "\\Chrasis";

	if ( _mkdir( userdir.c_str() ) != 0 )
	{
		struct _stat st;
		_stat( userdir.c_str(), &st );

		if ( (st.st_mode & _S_IFMT) == _S_IFDIR )
		{
			std::cerr << "Failed to create \"" << userdir << "\"." << std::endl;
			return false;
		}
	}

	if ( _access( user_database_path().c_str(), 0) != 0 )
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

#endif // ifdef _WINDOWS