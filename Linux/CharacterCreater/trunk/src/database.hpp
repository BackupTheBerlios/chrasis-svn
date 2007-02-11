/*
 * Character Creater
 *
 * Copyright (c) 2006 Victor Tseng <palatis@gentoo.tw>
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
# include <config.h>
#endif

#include <sqlite3.h>

#include "common.h"

template <typename EH>
basic_database<EH>::OPENDB::OPENDB():
	busy(false)
{
	sqlite3_create_function(
		db, "NORM", 2, SQLITE_UTF8, NULL,
		&sqlite3_callback_norm, NULL, NULL
	);
	sqlite3_create_function(
		db, "SQRT", 1, SQLITE_UTF8, NULL,
		&sqlite3_callback_sqrt, NULL, NULL
	);
}

template <typename EH>
basic_database<EH>::basic_database(std::string const & d):
	database_(d)
{
}

template <typename EH>
basic_database<EH>::~basic_database()
{
	for (typename OPENDB::collection::iterator it = opendbs_.begin();
	     it != opendbs_.end();
	     ++it)
	{
		OPENDB *p = *it;
		sqlite3_close(p -> db);
	}
	while(opendbs_.size())
	{
		typename OPENDB::collection::iterator it = opendbs_.begin();
		OPENDB *p = *it;
		if (p -> busy)
			error("destroying basic_database object before Query object");
		delete p;
		opendbs_.erase(it);
	}
}

template <typename EH>
typename basic_database<EH>::OPENDB*
basic_database<EH>::grabdb()
{
	OPENDB *odb = NULL;

	for (typename OPENDB::collection::iterator it = opendbs_.begin();
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

template <typename EH>
void
basic_database<EH>::freedb(basic_database::OPENDB *odb)
{
	if (odb)
		odb->busy = false;
}

template <typename EH>
bool
basic_database<EH>::Connected()
{
	OPENDB *odb = grabdb();
	if (!odb)
		return false;
	freedb(odb);
	return true;
}

template <typename EH>
std::string
basic_database<EH>::safestr(std::string const & u_str)
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

template <typename EH>
std::string
basic_database<EH>::xmlsafestr(std::string const & u_str)
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

template <typename EH>
int64_t
basic_database<EH>::a2bigint(std::string const & str)
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

template <typename EH>
uint64_t
basic_database<EH>::a2ubigint(std::string const & str)
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

template <typename DB_T>
basic_query<DB_T>::basic_query(DB_T & db):
	db_(db),
	odb_(db.grabdb()),
	res_(NULL)
{
}

template <typename DB_T>
basic_query<DB_T>::basic_query(DB_T & db, std::string const & sql):
	db_(db),
	odb_(db.grabdb()),
	res_(NULL)
{
	execute(sql);
}

template <typename DB_T>
basic_query<DB_T>::~basic_query()
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

template <typename DB_T>
bool
basic_query<DB_T>::execute(std::string const & sql)
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

template <typename DB_T>
bool
basic_query<DB_T>::get_result(std::string const & sql)
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

template <typename DB_T>
void
basic_query<DB_T>::free_result()
{
	if (odb_ && res_)
	{
		sqlite3_finalize(res_);
		res_ = NULL;
	}
}

template <typename DB_T>
sqlite_int64
basic_query<DB_T>::insert_id()
{
	return (odb_) ? sqlite3_last_insert_rowid(odb_->db) : 0;
}

template <typename DB_T>
bool
basic_query<DB_T>::more_rows()
{
	return (odb_ && res_) ? more_rows_ : false;
}


template <typename DB_T>
ResultRow
basic_query<DB_T>::fetch_row()
{
	ResultRow ret;
	if (odb_ && res_)
	{
		int i = 0;
		const char *p;
		while (p = sqlite3_column_name(res_, i))
		{
			const char *tmp;
			switch ( sqlite3_column_type(res_, i) )
			{
			case SQLITE_INTEGER:
				ret.push_back( p, sqlite3_column_int64(res_, i) );
				break;
			case SQLITE_FLOAT:
				ret.push_back( p, sqlite3_column_double(res_, i) );
				break;
			case SQLITE_TEXT:
				tmp = (const char *)sqlite3_column_text(res_, i);
				ret.push_back( p, tmp ? tmp : "" );
				break;
			default:
				error(
					"fetch_row: unhandled type [" + 
					toString(sqlite3_column_decltype(res_, i)) + 
					"]."
				);
			}
			++i;
		}
		more_rows_ = (sqlite3_step(res_) == SQLITE_ROW);
	}
	return ret;
}

template <typename DB_T>
std::string
basic_query<DB_T>::GetError()
{
	return odb_ ? sqlite3_errmsg(odb_->db) : std::string();
}

template <typename DB_T>
int
basic_query<DB_T>::GetErrNo()
{
	return odb_ ? sqlite3_errcode(odb_->db) : 0;
}
