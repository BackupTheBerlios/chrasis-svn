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
 * $Id: character.h 18 2006-09-19 21:18:42Z palatis $
 */

#ifndef _CHARACTER_H
#define _CHARACTER_H

#include "common.h"

EXPORT
template <
	typename VALUE_T,
	template < typename > class CONT = std::vector >
class basic_point
{
public:
	typedef VALUE_T					value_t;
	typedef basic_point< VALUE_T, CONT >		point_t;

	typedef CONT< point_t >				collection;
	typedef typename collection::iterator		iterator;
	typedef typename collection::const_iterator	const_iterator;

	basic_point(const value_t x, const value_t y): num_(x, y) {};

	value_t & x() { return num_.real(); };			//< x-coordinate
	value_t & y() { return num_.imag(); };			//< y-coordinate
	value_t const & x() const { return num_.real(); };	//< const x-coordinate
	value_t const & y() const { return num_.imag(); };	//< const y-coordinate

	value_t arg() const { return std::arg(num_); };		//< the argument (angle) of the point
	value_t abs() const { return std::abs(num_); };		//< the distance between the point and origin

	value_t set_arg(value_t const);				//< set argument
	value_t set_length(value_t const);			//< set length

	point_t & operator -= (const point_t & rhs)
	{
		num_.real() -= rhs.x();
		num_.imag() -= rhs.y();
		return *this;
	}
private:
	std::complex< value_t > num_;
};

template < typename VALUE_T, template < typename > class CONT >
std::ostream & operator<<( std::ostream &, basic_point< VALUE_T, CONT > const &);

EXPORT template < typename VALUE_T, template < typename > class CONT >
basic_point< VALUE_T, CONT >
operator - (basic_point< VALUE_T, CONT > const &, basic_point< VALUE_T, CONT > const &);

EXPORT template < typename VALUE_T, template < typename > class CONT >
bool
operator != (basic_point< VALUE_T, CONT > const &, basic_point< VALUE_T, CONT > const &);

EXPORT
template <
	typename POINT_T,
	template < typename > class CONT = std::vector >
class basic_stroke
{
public:
	typedef typename POINT_T::value_t		value_t;
	typedef POINT_T					point_t;
	typedef basic_stroke< POINT_T, CONT >		stroke_t;

	typedef CONT< stroke_t >			collection;
	typedef typename collection::iterator		iterator;
	typedef typename collection::const_iterator	const_iterator;

	void add_point(point_t const & p) { points_.push_back(p); };
	void add_point(value_t const x, value_t const y)
		{ points_.push_back(point_t(x, y)); };

	typename point_t::iterator points_begin() { return points_.begin(); };
	typename point_t::iterator points_end() { return points_.end(); };
	typename point_t::const_iterator points_begin() const { return points_.begin(); };
	typename point_t::const_iterator points_end() const { return points_.end(); };

	size_t point_count() const { return points_.size(); }
	value_t length() const;

private:
	typename point_t::collection points_;
};

template < typename POINT_T, template < typename > class CONT >
std::ostream & operator<<(std::ostream & lhs, basic_stroke< POINT_T, CONT > const &);

EXPORT
template <
	typename STROKE_T,
	template < typename > class CONT = std::vector >
class basic_character
{
public:
	typedef typename STROKE_T::point_t		point_t;
	typedef typename point_t::value_t		value_t;
	typedef STROKE_T				stroke_t;
	typedef basic_character< STROKE_T, CONT >	character_t;

	typedef CONT< character_t >			collection;
	typedef typename collection::iterator		iterator;
	typedef typename collection::const_iterator	const_iterator;


	basic_character(): name_() {};
	basic_character(const std::string & n): name_(n) {};

	void new_stroke() { strokes_.push_back(stroke_t()); };
	void add_stroke(const STROKE_T & s) { strokes_.push_back(s); };

	int stroke_count() const { return strokes_.size(); };

	void add_point(const point_t & p) { strokes_.rbegin()->add_point(p); };
	void add_point(const typename point_t::value_t x, const typename point_t::value_t y)
		{ strokes_.rbegin()->add_point(x, y); };

	const std::string get_name() const { return name_; };
	void set_name(const std::string & n) { name_ = n; };

	typename stroke_t::iterator strokes_begin() { return strokes_.begin(); };
	typename stroke_t::iterator strokes_end() { return strokes_.end(); };
	typename stroke_t::const_iterator strokes_begin() const { return strokes_.begin(); };
	typename stroke_t::const_iterator strokes_end() const { return strokes_.end(); };
private:
	typename stroke_t::collection strokes_;
	std::string name_;
};

template < typename STROKE_T, template < typename > class CONT >
std::ostream & operator<<(std::ostream &, basic_character< STROKE_T, CONT > const &);

EXPORT template < typename STROKE_T, template < typename > class CONT >
bool operator<(basic_character< STROKE_T, CONT > const &, basic_character< STROKE_T, CONT > const &);

typedef basic_point< double > Point;
typedef basic_stroke< Point > Stroke;
typedef basic_character< Stroke > Character;

#ifndef USE_EXPORT
# include "character.hpp"
#endif

#endif
