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

namespace chrasis
{

class Database {
public:

	struct OPENDB {
		typedef std::list<OPENDB *> collection;

		OPENDB();
		sqlite3 *db;
		bool busy;
	};

	Database(std::string const &);
	virtual ~Database();

	bool Connected();

	void error(std::string const & err) {
		std::cerr << "Database: " << err;
	};

	OPENDB *grabdb();
	void freedb(OPENDB *odb);

	static std::string safestr(std::string const &);
	static std::string xmlsafestr(std::string const &);

	static int64_t a2bigint(std::string const &);
	static uint64_t a2ubigint(std::string const &);

private:
	Database(const Database &) {};
	Database& operator=(const Database &) { return *this; };

	std::string database_;
	OPENDB::collection opendbs_;
};

class ResultRow
{
public:
	typedef Database		database_t;
	typedef boost::variant <
		int,
		double,
		std::string >		value_t;

	ResultRow (sqlite3_stmt *res)
	{
		if (res == NULL)
			return;

		int i = 0;
		const char *p;
		while (p = sqlite3_column_name(res, i))
		{
			const char *tmp;
			switch ( sqlite3_column_type(res, i) )
			{
			case SQLITE_INTEGER:
				values_[ p ] = sqlite3_column_int(res, i);
				break;
			case SQLITE_FLOAT:
				values_[ p ] = sqlite3_column_double(res, i);
				break;
			case SQLITE_TEXT:
				tmp = (const char *)sqlite3_column_text(res, i);
				values_[ p ] = tmp ? tmp : "";
				break;
			default:
				error(
					"c-tor: unhandled type [" + 
					std::string(sqlite3_column_decltype(res, i)) + 
					"]."
				);
			}
			++i;
		}
	}

	template <typename T>
	T get(std::string const & idx)
	{
		try
		{
			return boost::get<T>(values_[idx]);
		}
		catch (boost::bad_get & ex)
		{
			std::cerr << ex.what() << std::endl;
			std::cerr << idx << std::endl;
		}
	}
private:
	std::map< std::string, value_t > values_;

	void error(std::string const & errmsg) { std::cerr << "Resultrow: " << errmsg; };
};

class Query {
public:
	typedef Database database_t;

	Query(database_t &);
	Query(database_t &, const std::string &);
	~Query();

	void error(const std::string & errstr) { db_.error(std::string("Query: ") + errstr); };

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
	Query(const Query & q): db_(q.GetDatabase()) {}
	Query & operator=(const Query&) { return *this; }

	database_t& db_;			//< Reference to database object
	database_t::OPENDB *odb_;	//< Connection pool handle
	sqlite3_stmt *res_;			//< Stored result
	std::string last_query_;		//< Last query
	bool more_rows_;			//< true for more results
};

} // namespace chrasis

#endif
