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

namespace chrasis
{

namespace SQLite
{

Database::Database(std::string const & path):
	database_(path)
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

Database::OPENDB::OPENDB():
	busy(false)
{
}

Database::OPENDB*
Database::grabdb()
{
	OPENDB::collection::iterator odb = opendbs_.begin();
	while (odb != opendbs_.end() && odb->busy)
		++odb;

	if (odb == opendbs_.end())
	{
		opendbs_.push_front(OPENDB());
		odb = opendbs_.begin();

		if (sqlite3_open(database_.c_str(), &odb->db) != SQLITE_OK)
		{
			debug_print(
				"Database::grabdb(): Couldn't open database \"" + database_ + "\".\n" +
				"\tErrMsg: " + sqlite3_errmsg(odb->db) + "\n",
				CHRASIS_DEBUG_SQL
			);
			sqlite3_close(odb->db);
			opendbs_.erase(odb);
			return NULL;
		}
	}

	odb->busy = true;

	return &(*odb);
}

void
Database::freedb(OPENDB & odb)
{
	odb.busy = false;
}

Transaction::Transaction(Database & db):
	db_(db), odb_(db.grabdb())
{
	sqlite3_exec(odb_->db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
}

Transaction::~Transaction()
{
	sqlite3_exec(odb_->db, "END TRANSACTION;", NULL, NULL, NULL);
	db_.freedb(*odb_);
}

void
Transaction::commit()
{
	sqlite3_exec(odb_->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

Command::Command(Database & db):
	db_(db), odb_(db.grabdb()), last_sql_(), transaction_(false)
{
}

Command::Command(Transaction & t):
	db_(t.db_), odb_(t.odb_), last_sql_(), transaction_(true)
{
}

Command::~Command()
{
	if (!transaction_)
		db_.freedb(*odb_);
}

void
Command::execute_nonquery(std::string const & sql)
{
	debug_print(
		"Command::execute(): sql = [" + sql + "]\n",
		CHRASIS_DEBUG_SQL
	);

	char *errmsg;
	last_sql_ = sql;
	if (sqlite3_exec(odb_->db, sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK)
	{
		debug_print(
			"Command::execute_nonquery(): Couldn't execute sql \"" + sql + "\".\n" +
			"\tErrMsg: " + errmsg + "\n",
			CHRASIS_DEBUG_SQL
		);
	}
}

void
Command::execute(std::string const & sql)
{
	debug_print(
		"Command::execute(): sql = [" + sql + "]\n",
		CHRASIS_DEBUG_SQL
	);

	last_sql_ = sql;
	if (sqlite3_prepare_v2(odb_->db, sql.c_str(), sql.size(), &res_, NULL) != SQLITE_OK)
	{
		debug_print(std::string(
			"Command::execute(): Prepare query failed!\n") +
			"\tErrMsg: " + sqlite3_errmsg(odb_->db) + "\n",
			CHRASIS_DEBUG_SQL
		);
	}
}

bool
Command::next()
{
	return (sqlite3_step(res_) == SQLITE_ROW);
}

int
Command::column_int(int ncol)
{
	return sqlite3_column_int(res_, ncol);
}

double
Command::column_double(int ncol)
{
	return sqlite3_column_double(res_, ncol);
}

std::string
Command::column_text(int ncol)
{
	const char * s = reinterpret_cast<const char *>(sqlite3_column_text(res_, ncol));
	return s;
}

void
Command::free_result()
{
	sqlite3_finalize(res_);
}

int
Command::last_rowid()
{
	return sqlite3_last_insert_rowid(odb_->db);
}

std::string const &
Command::get_last_sql() const
{
	return last_sql_;
}

} // namespace ::chrasis::SQLite

} // namespace ::chrasis
