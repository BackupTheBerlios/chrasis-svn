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

#ifndef _CHRASIS_H
#define _CHRASIS_H

#ifndef CHRASIS_API
# define CHRASIS_API
#endif

#include <complex>
#include <cmath>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <iterator>
#include <iostream>
#include <sstream>
#include <fstream>

#include <sys/stat.h>
#include <unistd.h>

#include <chrasis/character.h>
#include <chrasis/global.h>
#include <chrasis/settings.h>
#include <chrasis/database.h>

namespace chrasis
{

typedef std::vector< int >		char_traits_t;
typedef int				possibility_t;
typedef std::multimap<
	possibility_t,
	std::pair< int, std::string > >	character_possibility_t;
typedef std::vector< int >		character_ids_t;

CHRASIS_API
Stroke
normalize(Stroke const &, int const);

CHRASIS_API
Character
normalize(Character const &);

CHRASIS_API
character_possibility_t
recognize(Character const &);

CHRASIS_API
character_possibility_t
recognize(Character const &, Database &);

CHRASIS_API
bool
learn(Character const &);

CHRASIS_API
bool
learn(Character const &, Database &);

} // namespace chrasis

#endif
