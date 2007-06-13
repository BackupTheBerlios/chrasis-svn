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

#ifndef _DATABASE_H
#define _DATABASE_H

#ifndef _CHRASIS_H
#warning "shouldn't be including this file directly!"
#warning "try #include <chrasis.h> instead."
#endif

#include <sqlite3.h>
#include <chrasis.h>

namespace chrasis
{

class Database
{
public:
	struct OPENDB {
		typedef std::list<OPENDB> collection;

		OPENDB();
		sqlite3 *db;
		bool busy;
	};

	Database(std::string const &);
	virtual ~Database();

	OPENDB* grabdb();
	void freedb(OPENDB*);
	
private:
	Database(const Database &) {};
	Database& operator=(const Database &) { return *this; };

	std::string database_;
	OPENDB::collection opendbs_;
};

} // namespace chrasis

#endif
