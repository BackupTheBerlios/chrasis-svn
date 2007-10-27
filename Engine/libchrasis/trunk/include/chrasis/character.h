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

#include <cmath>

namespace chrasis
{

class Character
{
public:
	class Stroke
	{
	public:
		class Point
		{
		public:
			typedef int				value_t;
			typedef std::vector< Point >		collection;
			typedef collection::iterator		iterator;
			typedef collection::const_iterator	const_iterator;

			Point(value_t const x, value_t const y):
				x_(x), y_(y) {}

			value_t & x() { return x_; }			//< x-coordinate
			value_t & y() { return y_; }			//< y-coordinate
			value_t const & x() const { return x_; }	//< x-coordinate (const)
			value_t const & y() const { return y_; }	//< y-coordinate (const)

			/// argument of the point
			double arg() const
			{ return std::atan( static_cast<double>(x_) / y_ ); }
			/// the distance between the point and origin
			double abs() const { return std::sqrt(x_*x_ + y_*y_); }

			/// set argument
			value_t set_arg(double const a)
			{
				value_t old_len = abs();
				x_ = std::acos(a) * old_len;
				y_ = std::asin(a) * old_len;
				return arg();
			}
			/// set the distance between the point and origin
			value_t set_length(double const l)
			{
				double factor = l / abs();
				x_ = x_ * factor;
				y_ = y_ * factor;
				return abs();
			}

			Point & operator += (const Point & rhs)
			{
				x_ += rhs.x_;
				y_ += rhs.y_;
				return *this;
			}

			Point & operator -= (const Point & rhs)
			{
				x_ -= rhs.x_;
				y_ -= rhs.y_;
				return *this;
			}
		private:
			value_t x_;
			value_t y_;
		};

		typedef Point::value_t			value_t;
		typedef std::vector< Stroke >		collection;
		typedef collection::iterator		iterator;
		typedef collection::const_iterator	const_iterator;

		/**
		 * add a new point to the end of the stroke
		 * @param p the point to be added
		 */
		void add_point(Point const & p) { points_.push_back(p); };
		/**
		 * add a new point to the end of the stroke
		 * @param x x coordinate of the point to be added
		 * @param y y coordinate of the point to be added
		 */
		void add_point(value_t const x, value_t const y)
			{ points_.push_back(Point(x, y)); };

		/**
		 * get the iterator points to the first point of the stroke
		 * @return iterator of the first point of the stroke
		 */
		Point::iterator points_begin() { return points_.begin(); };
		/**
		 * get the iterator points to the last point of the stroke
		 * @return iterator of the last point of the stroke
		 */
		Point::iterator points_end() { return points_.end(); };
		/**
		 * get the const iterator points to the first point of the stroke
		 * @return const iterator of the first point of the stroke
		 */
		Point::const_iterator points_begin() const { return points_.begin(); };
		/**
		 * get the const iterator points to the last point of the stroke
		 * @return const iterator of the last point of the stroke
		 */
		Point::const_iterator points_end() const { return points_.end(); };

		/**
		 * get the number of points of the stroke
		 * @return number of points of the stroke
		 */
		size_t point_count() const { return points_.size(); }
		
		/**
		 * get the length of the stroke
		 * @return length of the stroke
		 */
		value_t length() const
		{
			Point::value_t length = 0.0;
			for (Point::const_iterator
				pi = points_.begin(),
				pii = points_.begin() + 1;
			     pi != points_.end() &&
			     pii != points_.end();
			     ++pi,
			     ++pii)
			{
				length += std::sqrt(
					(pi->x() - pii->x()) * (pi->x() - pii->x()) +
					(pi->y() - pii->y()) * (pi->y() - pii->y())
				);
			}
			return length;
		}

	private:
		Point::collection points_;
	};

	typedef Stroke::Point::value_t		value_t;
	typedef std::vector< Character >	collection;
	typedef collection::iterator		iterator;
	typedef collection::const_iterator	const_iterator;

	Character(const std::string & n = std::string()):
		name_(n) {}

	void new_stroke() { strokes_.push_back(Stroke()); };
	void add_stroke(const Stroke & s) { strokes_.push_back(s); };

	int stroke_count() const { return strokes_.size(); };

	void add_point(Stroke::Point const & p) { strokes_.rbegin()->add_point(p); };
	void add_point(const value_t x, const value_t y)
		{ strokes_.rbegin()->add_point(x, y); };

	const std::string get_name() const { return name_; };
	void set_name(const std::string & n) { name_ = n; };

	Stroke::iterator strokes_begin() { return strokes_.begin(); };
	Stroke::iterator strokes_end() { return strokes_.end(); };
	Stroke::const_iterator strokes_begin() const { return strokes_.begin(); };
	Stroke::const_iterator strokes_end() const { return strokes_.end(); };
private:
	Stroke::collection strokes_;
	std::string name_;
};

static inline
Character::Stroke::Point
operator + (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs)
{
	return Character::Stroke::Point(
		lhs.x() + rhs.x(),
		lhs.y() + rhs.y()
	);
}

static inline
Character::Stroke::Point
operator - (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs)
{
	return Character::Stroke::Point(
		lhs.x() - rhs.x(),
		lhs.y() - rhs.y()
	);
}

static inline
bool
operator == (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs)
{
	return (lhs.x() == rhs.x() && lhs.y() == rhs.y());
}

static inline
bool
operator != (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs)
{
	return (lhs.x() != rhs.x() || lhs.y() != rhs.y());
}

static inline
bool
operator < (Character const & lhs, Character const & rhs)
{
	return (lhs.get_name() < rhs.get_name());
}

typedef Character::Stroke		Stroke;
typedef Character::Stroke::Point	Point;

} // namespace chrasis

#endif // #ifndef _CHARACTER_H
