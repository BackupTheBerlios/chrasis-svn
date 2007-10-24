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

#ifndef _CHARACTER_H
#define _CHARACTER_H

#ifndef _CHRASIS_H
#warning "shouldn't be including this file directly!"
#warning "try #include <chrasis.h> instead."
#endif

namespace chrasis
{

template < typename VALUE_T >
class basic_point
{
public:
	typedef VALUE_T					value_t;
	typedef basic_point< VALUE_T >			point_t;

	typedef std::vector< point_t >			collection;
	typedef typename collection::iterator		iterator;
	typedef typename collection::const_iterator	const_iterator;

	basic_point(const value_t x, const value_t y): num_(x, y) {};

	value_t & x() { return num_.real(); };			//< x-coordinate
	value_t & y() { return num_.imag(); };			//< y-coordinate
	value_t const & x() const { return num_.real(); };	//< const x-coordinate
	value_t const & y() const { return num_.imag(); };	//< const y-coordinate

	double arg() const
	{
		return std::arg( std::complex< double >(num_.real(), num_.imag()) );
	}							//< the argument of the point
	value_t abs() const { return std::abs(num_); };
								//< the distance between the point and origin

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

template < typename VALUE_T >
std::ostream & operator<<( std::ostream &, basic_point< VALUE_T > const &);

template < typename VALUE_T >
basic_point< VALUE_T >
operator - (basic_point< VALUE_T > const &, basic_point< VALUE_T > const &);

template < typename VALUE_T >
bool
operator != (basic_point< VALUE_T > const &, basic_point< VALUE_T > const &);

template < typename POINT_T >
class basic_stroke
{
public:
	typedef typename POINT_T::value_t		value_t;
	typedef POINT_T					point_t;
	typedef basic_stroke< POINT_T >			stroke_t;

	typedef std::vector< stroke_t >			collection;
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

template < typename POINT_T >
std::ostream & operator<<(std::ostream & lhs, basic_stroke< POINT_T > const &);

template < typename STROKE_T >
class basic_character
{
public:
	typedef typename STROKE_T::point_t		point_t;
	typedef typename point_t::value_t		value_t;
	typedef STROKE_T				stroke_t;
	typedef basic_character< STROKE_T >		character_t;

	typedef std::vector< character_t >		collection;
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

template < typename STROKE_T >
std::ostream & operator<<(std::ostream &, basic_character< STROKE_T > const &);

template < typename STROKE_T >
bool operator<(basic_character< STROKE_T > const &, basic_character< STROKE_T > const &);

template < typename VALUE_T >
typename basic_point< VALUE_T >::value_t
basic_point< VALUE_T >::set_arg(value_t const a)
{
	value_t orig_len = abs();	//< remember current length
	x() = std::acos(a) * orig_len;
	y() = std::asin(a) * orig_len;
	return arg();
}

template < typename VALUE_T >
typename basic_point< VALUE_T >::value_t
basic_point< VALUE_T >::set_length(value_t const l)
{
	value_t factor = l / abs();
	x() *= factor;
	y() *= factor;
	return abs();
}

template < typename VALUE_T >
std::ostream &
operator << (std::ostream & lhs, basic_point< VALUE_T > const & rhs)
{
	return lhs << "(" << rhs.x() << ", " << rhs.y() << ")";
}

template < typename VALUE_T >
basic_point< VALUE_T >
operator - (basic_point< VALUE_T > const & lhs, basic_point< VALUE_T > const & rhs)
{
	return basic_point< VALUE_T >(
		lhs.x() - rhs.x(),
		lhs.y() - rhs.y() );
}

template < typename VALUE_T >
bool
operator != (basic_point< VALUE_T > const & lhs, basic_point< VALUE_T > const & rhs)
{
	return lhs.x() != rhs.x() || lhs.y() != rhs.y();
}

template < typename POINT_T >
typename basic_stroke< POINT_T >::value_t
basic_stroke< POINT_T >::length() const
{
	typename POINT_T::value_t length = 0.0;
	for (typename POINT_T::const_iterator pi = points_.begin();
	     pi != points_.end() - 1;
	     ++pi)
		length += (*pi - *(pi+1)).abs();
	return length;
}

template < typename POINT_T >
std::ostream &
operator<<(std::ostream & lhs, basic_stroke< POINT_T > const & rhs)
{
	for (typename POINT_T::const_iterator i = rhs.points_begin();
		i != rhs.points_end();
		++i)
		lhs << *i << " ";
	return lhs;
}

template < typename STROKE_T >
std::ostream &
operator<<(std::ostream & lhs, basic_character< STROKE_T > const & rhs)
{
	lhs << rhs.get_name() << ": " << std::endl;
	for (typename basic_character< STROKE_T >::stroke_t::const_iterator i = rhs.strokes_begin();
		i != rhs.strokes_end();
		++i)
		lhs << *i << std::endl;
	return lhs;
}

template < typename STROKE_T >
bool
operator<(basic_character< STROKE_T > const & lhs, basic_character< STROKE_T > const & rhs)
{
	return (lhs.get_name() < rhs.get_name()) ? true : false ;
}

typedef basic_point< int > Point;
typedef basic_stroke< Point > Stroke;
typedef basic_character< Stroke > Character;

} // namespace chrasis

#endif // #ifndef _CHARACTER_H
