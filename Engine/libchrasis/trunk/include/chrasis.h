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

#include <chrasis/global.h>

namespace chrasis
{

typedef double				possibility_t;
typedef std::map<
	possibility_t,
	std::pair< int, std::string > >	character_possibility_t;
typedef std::map< int, Character >	character_memories_t;


CHRASIS_API
character_possibility_t
recognize(Character const &, Database &);

CHRASIS_API
bool
learn(Character const &, Database &);

CHRASIS_API
void
initialize_database(Database &);

} // namespace chrasis

#endif
