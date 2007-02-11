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

template < typename VALUE_T, template < typename > class CONT >
typename basic_point< VALUE_T, CONT >::value_t
basic_point< VALUE_T, CONT >::set_arg(value_t const a)
{
	value_t orig_len = abs();	//< remember current length
	x() = std::acos(a) * orig_len;
	y() = std::asin(a) * orig_len;
	return arg();
}

template < typename VALUE_T, template < typename > class CONT >
typename basic_point< VALUE_T, CONT >::value_t
basic_point< VALUE_T, CONT >::set_length(value_t const l)
{
	value_t factor = l / abs();
	x() *= factor;
	y() *= factor;
	return abs();
}

template < typename VALUE_T, template < typename > class CONT >
std::ostream &
operator << (std::ostream & lhs, basic_point< VALUE_T, CONT > const & rhs)
{
	return lhs << "(" << rhs.x() << ", " << rhs.y() << ")";
}

template < typename VALUE_T, template < typename > class CONT >
basic_point< VALUE_T, CONT >
operator - (basic_point< VALUE_T, CONT > const & lhs, basic_point< VALUE_T, CONT> const & rhs)
{
	return basic_point< VALUE_T, CONT >(
		lhs.x() - rhs.x(),
		lhs.y() - rhs.y() );
}

template < typename VALUE_T, template < typename > class CONT >
bool
operator != (basic_point< VALUE_T, CONT > const & lhs, basic_point< VALUE_T, CONT > const & rhs)
{
	return lhs.x() != rhs.x() || lhs.y() != rhs.y();
}

template < typename POINT_T, template < typename > class CONT >
typename basic_stroke< POINT_T, CONT >::value_t
basic_stroke< POINT_T, CONT >::length() const
{
	typename POINT_T::value_t length = 0.0;
	for (typename POINT_T::const_iterator pi = points_.begin();
	     pi != points_.end() - 1;
	     ++pi)
		length += (*pi - *(pi+1)).abs();
	return length;
}

template < typename POINT_T, template < typename > class CONT >
std::ostream &
operator<<(std::ostream & lhs, basic_stroke< POINT_T, CONT > const & rhs)
{
	for (typename POINT_T::const_iterator i = rhs.points_begin();
		i != rhs.points_end();
		++i)
		lhs << *i << " ";
	return lhs;
}

template < typename STROKE_T, template < typename > class CONT >
std::ostream &
operator<<(std::ostream & lhs, basic_character< STROKE_T, CONT > const & rhs)
{
	lhs << rhs.get_name() << ": " << std::endl;
	for (typename basic_character< STROKE_T, CONT >::stroke_t::const_iterator i = rhs.strokes_begin();
		i != rhs.strokes_end();
		++i)
		lhs << *i << std::endl;
	return lhs;
}

template < typename STROKE_T, template < typename > class CONT >
bool
operator<(basic_character< STROKE_T, CONT > const & lhs, basic_character< STROKE_T, CONT > const & rhs)
{
	return (lhs.get_name() < rhs.get_name()) ? true : false ;
}
