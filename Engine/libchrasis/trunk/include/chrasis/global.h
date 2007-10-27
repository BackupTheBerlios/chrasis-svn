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

#ifndef _GLOBAL_H
#define _GLOBAL_H

#ifndef _CHRASIS_H
#warning "shouldn't be including this file directly!"
#warning "try #include <chrasis.h> instead."
#endif

namespace chrasis
{

/// Possibility of character alikeness stores in this format
typedef int				possibility_t;
/**
 * A list of all possible character ordered by how alike the
 * charaters are. Structured like this:
 * - \b node : each node, from .begin() to .end().
 *   - \b ->first : the alikeness (smaller the more similar)
 *   - \b ->second : std::pair<> containing some character informations
 *     - \b ->first : the \a character_id stored in the database
 *     - \b ->second : name of the character
 */
typedef std::multimap<
	possibility_t,
	std::pair< int, std::string > >	character_possibility_t;

typedef std::vector< int >		char_traits_t;
typedef std::multimap<
	possibility_t,
	std::pair< int, int > >		stroke_possibility_t;
typedef std::vector< int >		character_ids_t;
typedef std::vector< int >		stroke_ids_t;

template <typename T>
static inline
std::string
toString(T const & v)
{
	std::ostringstream oss;
	oss << v;
	return oss.str();
}

template <typename T>
static inline
T
fromString(std::string const & v)
{
	std::istringstream iss(v);
	T ret;
	iss >> ret;
	return ret;
}

template <typename T>
static inline
T
abs(const T v)
{
	return (v < T()) ? -v : v;
}

bool
static inline
fexist( std::string const & filename )
{
	struct stat buffer;
	return !( stat(filename.c_str(), &buffer) );
}

} // namespace chrasis

#endif
