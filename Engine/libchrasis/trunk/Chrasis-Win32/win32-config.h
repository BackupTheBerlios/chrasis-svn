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

// CHRASIS_API defined depends on export status
#ifdef LIBCHRASISWIN32_EXPORTS
# define CHRASIS_API __declspec(dllexport)
#else
# define CHRASIS_API __declspec(dllimport)
#endif

// there's no "__attribute__ ((visibility("hidden")))" on Win32
#define CHRASIS_INTERNAL

// for M_PI and such
#define _USE_MATH_DEFINES

// specify the db files
#define DEFAULT_DB_FILE		"chr_data.db"
#define DEFAULT_EMPTYDB_FILE	"empty_db.db"
#define DEFAULT_SCHEMA_FILE	"database_schema.sql"