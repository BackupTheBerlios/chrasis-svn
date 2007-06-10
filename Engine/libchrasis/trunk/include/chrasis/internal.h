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

#define DEFAULT_DB_FILE "chr_data.db"

Database &
_default_db();


// used by recognize()

// these numbers are basically magics...
static const double	ANGLE_THRESHOLD		= 30.0 / 180.0 * M_PI;	//< 30 deg
static const double	DIST_THRESHOLD_RATIO	= 1.0 / 15.0;		//< 1/15 of diagonal line
static const int	RESOLUTION		= 10000;		//< sampling resolution

/**
 *  get rid of redundent points of a stroke
 */
Stroke
_normalize(Stroke const &);

/**
 *  get rid of redundent points of a character
 *  (iteratively calls_normalize(Stroke const &))
 */
Character
_normalize(Character const &);


// used by learn()

// these numbers a basically magics...
static const double	LEARNING_THRESHOLD	= 0.15;			//< 15% of sampling resolution

/**
 *  try to remember a new character
 */
bool
_remember(Character const &, Database &);

/**
 *  update and adjust existing character
 */
bool
_reflect(Character const &, int const, Database &);

#if 0
/**
 *  try to recall how a character is written
 */
character_memories_t
_recall(std::string const &, Database &);
#endif

} // namespace chrasis

#endif
