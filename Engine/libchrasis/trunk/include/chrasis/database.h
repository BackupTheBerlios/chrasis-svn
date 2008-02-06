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
#include <list>

namespace chrasis
{

namespace SQLite
{

class Database;
class Transaction;
class Command;

class CHRASIS_API Database
{
public:	
	friend class Transaction;
	friend class Command;

	Database(std::string const & path);
	~Database();

private:
	struct OPENDB {
		typedef std::list<OPENDB> collection;
		
		OPENDB();

		sqlite3 *db;
		bool busy;
	};
	EXTERN template class CHRASIS_API std::list< OPENDB >;

	Database(Database &);
	Database & operator= (Database &);

	OPENDB* grabdb();
	void freedb(OPENDB & odb);

	std::string database_;
	OPENDB::collection opendbs_;
};

class CHRASIS_API Transaction {
public:
	friend class Command;

	Transaction(Database & db);
	~Transaction();

	void commit();

private:
	Database & db_;
	Database::OPENDB *odb_;
};

class CHRASIS_API Command
{
public:
	Command(Database & db);
	Command(Transaction & t);
	~Command();
	
	void execute_nonquery(std::string const & sql);
	void execute(std::string const & sql);
	
	bool next();

	int column_int(int ncol);
	double column_double(int ncol);
	std::string column_text(int ncol);

	void free_result();
	
	int last_rowid();

	std::string const & get_last_sql() const;

private:
	std::string last_sql_;
	sqlite3_stmt *res_;

	bool transaction_;
	Database & db_;
	Database::OPENDB *odb_;
};

}

} // namespace chrasis

#endif
