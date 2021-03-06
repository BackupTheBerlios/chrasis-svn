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

#include <iostream>
#include <sstream>

namespace chrasis
{

typedef std::vector< int >		id_container_t;

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

} // namespace chrasis

#endif
