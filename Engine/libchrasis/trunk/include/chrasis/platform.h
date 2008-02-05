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

#ifndef _PLATFORM_H
#define _PLATFORM_H

#ifndef _CHRASIS_H
#warning "shouldn't be including this file directly!"
#warning "try #include <chrasis.h> instead."
#endif

namespace chrasis
{

namespace platform
{

/**
 * path for system database.
 * @return path points to the system database
 */
CHRASIS_API
std::string
system_database_path();

/**
 * path for empty database
 * @return path points to an empty database
 */
CHRASIS_API
std::string
empty_database_path();

/**
 * path to the user's database
 * @return path points to current user's database
 */
CHRASIS_API
std::string
user_database_path();

/**
 * path to the empty database schema
 * @return path points to an empty database schema
 */
CHRASIS_API
std::string
database_schema_path();

/**
 * function to setup the user's data directory
 * @return true for success, false for failed.
 */
CHRASIS_API
bool
initialize_userdir();

} // namespace platform

} // namespace chrasis

#endif
