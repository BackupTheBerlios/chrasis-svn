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

#ifndef _CHRASIS_INTERNAL_H
#define _CHRASIS_INTERNAL_H

namespace chrasis
{

// database related

#define DEFAULT_DB_FILE		"chr_data.db"
#define DEFAULT_EMPTYDB_FILE	"empty_db.db"
#define DEFAULT_SCHEMA_FILE	"db_schema.sql"

// used by recognize()

// these numbers are basically magics...
static const double	ANGLE_THRESHOLD		= 30.0 / 180.0 * M_PI;	//< 30 deg
static const double	DIST_THRESHOLD_RATIO	= 1.0 / 15.0;		//< 1/15 of diagonal line
static const int	RESOLUTION		= 10000;		//< sampling resolution

bool
_recognize(Character const &, Database &, character_possibility_t &);

Stroke
_normalize(Stroke const &, int const);

character_ids_t
_get_cids_by_prototype(Character const &, Database &);

Character
_get_char_by_id(int const, Database &);


// used by learn()

// these numbers a basically magics...
static const double	LEARNING_THRESHOLD	= 0.15;			//< 15% of sampling resolution

bool
_learn(Character const &, Database &);

bool
_remember(Character const &, Database &);

bool
_reflect(Character const &, int const, Database &);

} // namespace chrasis

#endif
