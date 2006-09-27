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
 * $Id: character.cpp 18 2006-09-19 21:18:42Z palatis $
 */

#include "character.h"

std::ostream &
operator<<(std::ostream & lhs, Point const & rhs)
{
	lhs << "(" << rhs.x_ << ", " << rhs.y_ << ")";
	return lhs;
}

std::ostream &
operator<<(std::ostream & lhs, Stroke const & rhs)
{
	for (Stroke::point_collection::const_iterator i = rhs.points_.begin();
		i != rhs.points_.end();
		++i)
		lhs << *i << " ";
	return lhs;
}

std::ostream &
operator<<(std::ostream & lhs, Character const & rhs)
{
	for (Character::stroke_collection::const_iterator i = rhs.strokes_.begin();
		i != rhs.strokes_.end();
		++i)
		lhs << *i << std::endl;
	return lhs;
}
