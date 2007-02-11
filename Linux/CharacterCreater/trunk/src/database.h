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
 * $Id: chmlcodec.h 18 2006-09-19 21:18:42Z palatis $
 */

#ifndef _DATABASE_H
#define _DATABASE_H

#include "common.h"
#include <sqlite3.h>

#include "errorhandler.h"

// sqlite3 callbacks
void sqlite3_callback_norm(sqlite3_context*,int,sqlite3_value**);
void sqlite3_callback_sqrt(sqlite3_context*,int,sqlite3_value**);

template <typename EH = DefaultErrorHandler> class basic_database;
template <typename EH = DefaultErrorHandler> class basic_resultrow;
template <typename DATABASE_T > class basic_query;

typedef basic_database< > Database;
typedef basic_resultrow< > ResultRow;
typedef basic_query< Database > Query;

template <typename EH>
class basic_database {
public:
	typedef EH errorhandler_t;

	struct OPENDB {
		typedef std::list<OPENDB *> collection;

		OPENDB();
		sqlite3 *db;
		bool busy;
	};

	basic_database(std::string const &);
	virtual ~basic_database();

	bool Connected();

	void error(std::string const & err) {
		std::ostringstream serr;
		serr << "basic_database: " << err;
		EH::error(serr.str());
	};

	OPENDB *grabdb();
	void freedb(OPENDB *odb);

	static std::string safestr(std::string const &);
	static std::string xmlsafestr(std::string const &);

	static int64_t a2bigint(std::string const &);
	static uint64_t a2ubigint(std::string const &);

private:
	basic_database(const basic_database &) {};
	basic_database& operator=(const basic_database &) { return *this; };

	std::string database_;
	typename OPENDB::collection opendbs_;
};

template <typename EH>
class basic_resultrow
{
public:
	typedef EH errorhandler_t;

	const long long get_int(std::string const & idx)
	{ return storage_int_[ nmap_[idx] ]; }
	const double get_real(std::string const & idx)
	{ return storage_real_[ nmap_[idx] ]; }
	const std::string get_text(std::string const & idx)
	{ return storage_text_[ nmap_[idx] ]; }

	void push_back(const std::string & idx, const long long v)
	{ nmap_[idx] = storage_int_.size(); storage_int_.push_back(v); }
	void push_back(const std::string & idx, const double v)
	{ nmap_[idx] = storage_real_.size(); storage_real_.push_back(v); };
	void push_back(const std::string & idx, const std::string v)
	{ nmap_[idx] = storage_text_.size(); storage_text_.push_back(v); };

private:
	std::map< std::string, int > nmap_;
	std::vector< long long > storage_int_;
	std::vector< double > storage_real_;
	std::vector< std::string > storage_text_;

	void error(std::string const & errmsg) { errorhandler_t::error("basic_resultrow: " + errmsg); };
};

template < typename DB_T >
class basic_query {
public:
	typedef DB_T	database_t;

	basic_query(database_t &);
	basic_query(database_t &, const std::string &);
	~basic_query();

	void error(const std::string & errstr) {
		db_.error(std::string("basic_query: ") + errstr);
	};

	bool Connected();
	database_t & GetDatabase() const { return db_; };
	const std::string & GetLastQuery() const { return last_query_; };


	bool execute(const std::string &);	//< query, no result
	bool get_result(const std::string &);	//< query, with result
	void free_result();

	sqlite_int64 insert_id();
	
	bool more_rows();
	ResultRow fetch_row();
	long long last_insert_rowid() { return sqlite3_last_insert_rowid(odb_->db); };

	std::string GetError();
	int GetErrNo();

private:
	basic_query(const basic_query & q): db_(q.GetDatabase()) {}
	basic_query & operator=(const basic_query&) { return *this; }

	database_t& db_;			//< Reference to database object
	typename database_t::OPENDB *odb_;	//< Connection pool handle
	sqlite3_stmt *res_;			//< Stored result
	std::string last_query_;		//< Last query
	bool more_rows_;			//< true for more results
};

#ifndef USE_EXPORT
//# include "database.hpp"
#endif

#endif
