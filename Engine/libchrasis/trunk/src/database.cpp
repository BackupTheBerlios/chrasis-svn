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
	{
		OPENDB *p = *it;
		sqlite3_close(p -> db);
	}
	while(opendbs_.size())
	{
		OPENDB::collection::iterator it = opendbs_.begin();
		OPENDB *p = *it;
		if (p -> busy)
			error("destroying Database object before Query object");
		delete p;
		opendbs_.erase(it);
	}
}

Database::OPENDB*
Database::grabdb()
{
	OPENDB *odb = NULL;

	for (OPENDB::collection::iterator it = opendbs_.begin();
	     it != opendbs_.end();
	     ++it)
	{
		if ((*it)->busy)
		{
			odb = *it;
			break;
		}
	}

	if (!odb)
	{
		odb = new OPENDB;
		if (!odb)
		{
			error("grabdb: OPENDB struct couldn't be created.");
			return NULL;
		}
		if (sqlite3_open(database_.c_str(), &odb->db))
		{
			error(std::string("Couldn't open database: ") + sqlite3_errmsg(odb->db));
			sqlite3_close(odb->db);
			delete odb;
			return NULL;
		}
		opendbs_.push_back(odb);
	}
	odb->busy = true;
	return odb;
}

void
Database::freedb(Database::OPENDB *odb)
{
	if (odb)
		odb->busy = false;
}

bool
Database::Connected()
{
	OPENDB *odb = grabdb();
	if (!odb)
		return false;
	freedb(odb);
	return true;
}

std::string
Database::safestr(std::string const & u_str)
{
	std::string s_str;
	for (size_t i = 0;
	     i < u_str.size();
	     ++i)
	{
		switch (u_str[i])
		{
		case '\'':
		case '\\':
		case 34:
			s_str += '\'';
		default:
			s_str += u_str[i];
		}
	}
	return s_str;
}

std::string
Database::xmlsafestr(std::string const & u_str)
{
	std::string s_str;
	for (size_t i = 0;
	     i < u_str.size();
	     ++i)
	{
		switch (u_str[i])
		{
		case '&':
			s_str += "&amp;";
			break;
		case '<':
			s_str += "&lt;";
			break;
		case '>':
			s_str += "&gt;";
			break;
		case '"':
			s_str += "&quot;";
			break;
		case '\'':
			s_str += "&apos;";
			break;
		default:
			s_str += u_str[i];
		}
	}
	return s_str;
}

int64_t
Database::a2bigint(std::string const & str)
{
	int64_t val = 0;
	bool sign = false;
	size_t i = 0;
	if (str[i] == '-')
	{
		sign = true;
		++i;
	}
	for (;i < str.size(); ++i)
	{
		val = val * 10 + (str[i] - 48);
	}
	return sign ? -val : val;
}

uint64_t
Database::a2ubigint(std::string const & str)
{
	uint64_t val = 0;
	for (size_t i = 0;
	     i < str.size();
	     ++i)
	{
		val = val * 10 + (str[i] - 48);
	}
	return val;
}

Query::Query(Database & db):
	db_(db),
	odb_(db.grabdb()),
	res_(NULL)
{
}

Query::Query(database_t & db, std::string const & sql):
	db_(db),
	odb_(db.grabdb()),
	res_(NULL)
{
	execute(sql);
}

Query::~Query()
{
	if (res_)
	{
		error("sqlite3_finalize in destructor.");
		error("last sql = [" + last_query_ + "]");
		sqlite3_finalize(res_);
	}
	if (odb_)
		db_.freedb(odb_);
}

bool
Query::execute(std::string const & sql)
{
	last_query_ = sql;
	if (odb_ && res_)
		error("execute: query busy.");
	if (odb_ && !res_)
	{
		const char *s = NULL;
		int rc = sqlite3_prepare(odb_->db, sql.c_str(), sql.size(), &res_, &s);
		if (rc != SQLITE_OK)
		{
			error("execute: prepare query failed.");
			error("         sql = [" + sql + "]");
			return false;
		}
		if (!res_)
		{
			error("execute: query failed.");
			return false;
		}
		rc = sqlite3_step(res_);	// execute
		sqlite3_finalize(res_);		// free statement
		res_ = NULL;
		switch (rc)
		{
		case SQLITE_BUSY:
			error("execute: database busy.");
			break;
		case SQLITE_DONE:
		case SQLITE_ROW:
			more_rows_ = true;
			return true;
		case SQLITE_ERROR:
			error(sqlite3_errmsg(odb_->db));
			break;
		case SQLITE_MISUSE:
			error("execute: database misuse.");
			break;
		default:
			error("execute: unknow result code.");
		}
	}
	return false;
}

bool
Query::get_result(std::string const & sql)
{
	last_query_ = sql;
	if (odb_ && res_)
		error("get_result: query busy.");
	if (odb_ && !res_)
	{
		const char *s = NULL;
		int rc = sqlite3_prepare(odb_->db, sql.c_str(), sql.size(), &res_, &s);
		if (rc != SQLITE_OK)
		{
			error("get_result: prepare query failed.");
			error("            sql = [" + sql + "]");
			return false;
		}
		if (!res_)
		{
			error("get_result: query failed");
			return false;
		}
		more_rows_ = (sqlite3_step(res_) == SQLITE_ROW);
	}
	return true;
}

void
Query::free_result()
{
	if (odb_ && res_)
	{
		sqlite3_finalize(res_);
		res_ = NULL;
	}
}

sqlite_int64
Query::insert_id()
{
	return (odb_) ? sqlite3_last_insert_rowid(odb_->db) : 0;
}

bool
Query::more_rows()
{
	return (odb_ && res_) ? more_rows_ : false;
}

ResultRow
Query::fetch_row()
{
	if (odb_ && res_)
	{
		ResultRow ret(res_);
		more_rows_ = (sqlite3_step(res_) == SQLITE_ROW);
		return ret;
	}
	return ResultRow (NULL);
}

std::string
Query::GetError()
{
	return odb_ ? sqlite3_errmsg(odb_->db) : std::string();
}

int
Query::GetErrNo()
{
	return odb_ ? sqlite3_errcode(odb_->db) : 0;
}

} // namespace chrasis
