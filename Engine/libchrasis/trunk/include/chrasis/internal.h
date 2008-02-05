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

#ifndef _CHRASIS_INTERNAL_H
#define _CHRASIS_INTERNAL_H

#include "chrasis.h"

namespace chrasis
{

enum CHRASIS_DEBUG_FACILITIES
{
	CHRASIS_DEBUG_SQL,
	CHRASIS_DEBUG_RECOGNIZE,
	CHRASIS_DEBUG_LEARN,
	CHRASIS_DEBUG_SIZE
};

static inline
std::vector< std::string >
_initialize_facilities()
{
	std::vector< std::string > ret(CHRASIS_DEBUG_SIZE);
	
	ret[CHRASIS_DEBUG_SQL] = "sql";
	ret[CHRASIS_DEBUG_RECOGNIZE] = "recognize";
	ret[CHRASIS_DEBUG_LEARN] = "learn";
	
	return ret;
}

static inline
void
debug_print(std::string const & dbgstr, CHRASIS_DEBUG_FACILITIES const & fac)
{
#ifdef _DEBUG
	static std::vector< std::string > facility( _initialize_facilities() );

	char * c_debug_env = getenv("LIBCHRASIS_DEBUG");
	if (c_debug_env == NULL)
		return;

	std::string debug_env( c_debug_env );
	if (debug_env.find(facility[fac], 0) != std::string::npos)
		std::cerr << dbgstr;
#endif
}

CHRASIS_INTERNAL
Stroke
_shift(Stroke const & nstr);

// used by recognize()

// these numbers are basically magics...
static const double	ANGLE_THRESHOLD		= 30.0 / 180.0 * M_PI;	//< 30 deg
static const double	DIST_THRESHOLD_RATIO	= 1.0 / 15.0;		//< 1/15 of diagonal line
static const int	RESOLUTION		= 10000;		//< sampling resolution

CHRASIS_INTERNAL
id_container_t
_get_ids_by_prototype(Stroke const & pstr, SQLite::Command & cmd);

CHRASIS_INTERNAL
Stroke
_get_stroke_by_id(int id, SQLite::Command & cmd);

CHRASIS_INTERNAL
ItemPossibilityList
_recognize(Stroke const & nstr, SQLite::Command & cmd);

CHRASIS_INTERNAL
id_container_t
_get_ids_by_prototype(Character const & pchr, SQLite::Command & cmd);

CHRASIS_INTERNAL
Character
_get_character_by_id(int const & id, SQLite::Command & cmd);

CHRASIS_INTERNAL
ItemPossibilityList
_recognize(Character const & nchr, SQLite::Command & cmd);

// used by learn()

// these numbers a basically magics...
static const double	LEARNING_THRESHOLD	= 0.15;		//< 15% of sampling resolution

CHRASIS_INTERNAL
int
_learn(Stroke const & nstr, SQLite::Command & cmd);

CHRASIS_INTERNAL
int
_remember(Stroke const & nstr, SQLite::Command & cmd);

CHRASIS_INTERNAL
int
_reflect(Stroke const & nstr, int ssid, SQLite::Command & cmd);

CHRASIS_INTERNAL
int
_learn(Character const & nchr, SQLite::Command & cmd);

CHRASIS_INTERNAL
int
_remember(Character const & nchr, SQLite::Command & cmd);

CHRASIS_INTERNAL
int
_reflect(Character const & nchr, int const cid, SQLite::Command & cmd);

} // namespace ::chrasis

#endif
