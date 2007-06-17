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
 
#include <chrasis.h>
#include <chrasis/internal.h>
#include <chrasis/character.h>

namespace chrasis
{

Database::OPENDB::OPENDB():
	busy(false)
{
}

Database::Database(std::string const & d):
	database_(d)
{
}

Database::~Database()
{
	for (OPENDB::collection::iterator it = opendbs_.begin();
	     it != opendbs_.end();
	     ++it)
		sqlite3_close(it->db);

	while(opendbs_.size())
	{
		OPENDB::collection::iterator it = opendbs_.begin();
		if (it->busy)
			std::cerr << "Database: destroying Database object before Query" << std::endl;
		opendbs_.erase(it);
	}
}

Database::OPENDB*
Database::grabdb()
{
	OPENDB::collection::iterator odb = opendbs_.begin();
	while (odb->busy && odb != opendbs_.end())
		++odb;

	if (odb == opendbs_.end())
	{
		opendbs_.push_back(OPENDB());
		odb = opendbs_.end();
		--odb;

		if (sqlite3_open(database_.c_str(), &odb->db))
		{
			std::cerr << "Database: grabdb(): Couldn't open database: "
				  << sqlite3_errmsg(odb->db) << std::endl;
			sqlite3_close(odb->db);
			opendbs_.erase(odb);
			return NULL;
		}
	}

	odb->busy = true;

	return &(*odb);
}

void
Database::freedb(Database::OPENDB *odb)
{
	if (odb)
		odb->busy = false;
}

bool
Database::execute(std::string const & sql, Database::OPENDB * const xodb)
{
	OPENDB *odb;
	if (xodb == NULL)
		odb = grabdb();
	else
		odb = xodb;

	if (getenv("LIBCHRASIS_DEBUG") != NULL)
		std::cout << sql << std::endl;

	sqlite3_stmt *res = NULL;
	const char *s = NULL;
	int rc = sqlite3_prepare(odb->db, sql.c_str(), sql.size(), &res, &s);

	if (rc != SQLITE_OK)
	{
		std::cerr << "Database: prepare query failed." << std::endl;
		std::cerr << "          sql = [" + sql + "]" << std::endl;
		return false;
	}
	if (!res)
	{
		std::cerr << "Database: query failed." << std::endl;
		return false;
	}

	sqlite3_step(res);
	sqlite3_finalize(res);

	if (xodb == NULL)
		freedb(odb);

	return true;
}

} // namespace chrasis
