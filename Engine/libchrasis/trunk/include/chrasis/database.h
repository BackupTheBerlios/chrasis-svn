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
 * $Id$
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

	class Transaction {
	public:
		Transaction(OPENDB & odb):
			odb_(odb)
		{
			sqlite3_exec(odb_.db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
		}

		~Transaction()
		{
			sqlite3_exec(odb_.db, "END TRANSACTION;", NULL, NULL, NULL);
		}
	private:
		OPENDB & odb_;
	};

	Database(std::string const & path): database_(path) { };
	virtual ~Database();

	OPENDB* grabdb();
	void freedb(OPENDB* odb)
	{
		if (odb)
			odb->busy = false;
	}

	bool execute(std::string const & sql, OPENDB * const xodb = NULL);

private:
	Database(const Database &) {};
	Database& operator=(const Database &) { return *this; };

	std::string database_;
	OPENDB::collection opendbs_;
};

} // namespace chrasis

#endif
