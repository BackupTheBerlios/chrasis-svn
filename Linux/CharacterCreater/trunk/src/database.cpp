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

#include "common.h"

#include "database.h"
#include "database.hpp"

void
sqlite3_callback_norm(sqlite3_context* context, int argc, sqlite3_value** argv)
{
	double v[2];
	for (int i = 0;i < 2;++i)
	{
		switch ( sqlite3_value_type(argv[i]) )
		{
		case SQLITE_INTEGER:
			v[i] = sqlite3_value_int64(argv[i]);
			break;
		case SQLITE_FLOAT:
			v[i] = sqlite3_value_double(argv[i]);
			break;
		default:
			std::string errmsg("NORM: argument " + toString(i+1) + " is neither REAL or INTEGER.");
			sqlite3_result_error(context, errmsg.c_str(), errmsg.size());
			return;
		}
	}
	sqlite3_result_double(context, v[0] * v[0] + v[1] * v[1]);
}

void
sqlite3_callback_sqrt(sqlite3_context* context, int argc, sqlite3_value** argv)
{
	switch ( sqlite3_value_type(argv[0]) )
	{
	case SQLITE_INTEGER:
		sqlite3_result_double(context, std::sqrt(sqlite3_value_int64(argv[0])));
		return;
	case SQLITE_FLOAT:
		sqlite3_result_double(context, std::sqrt(sqlite3_value_double(argv[0])));
		return;
	default:
		std::string errmsg("SQRT: argument 1 is neither REAL or INTEGER.");
		sqlite3_result_error(context, errmsg.c_str(), errmsg.size());
		return;
	}
	sqlite3_result_error(context, "SQRT: shouldn't get here!", 26);
}

template class basic_database< >;
template class basic_resultrow< >;
template class basic_query< Database >;
