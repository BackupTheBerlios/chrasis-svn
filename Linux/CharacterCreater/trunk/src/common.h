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

#ifndef _COMMON_H
#define _COMMON_H

// standard headers
#include <complex>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>

#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>

// platform dependent header
#include <sys/stat.h>

#ifdef USE_EXPORT
# define EXPORT export
#else
# define EXPORT
#endif

EXPORT
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
	if ( stat( filename.c_str(), &buffer ) )
		return false;
	return true;
}

#endif
