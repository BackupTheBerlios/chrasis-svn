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
 * $Id: character.h 53 2007-10-27 23:33:29Z palatis $
 */

#include <chrasis.h>
#include <chrasis/character.h>

#include <iostream>

namespace chrasis
{

Character::Stroke::Point::Point(
	Character::Stroke::Point::value_t x,
	Character::Stroke::Point::value_t y)
:
	x_(x), y_(y)
{
}

Character::Stroke::Point::value_t &
Character::Stroke::Point::x()
{
	return x_;
}

Character::Stroke::Point::value_t &
Character::Stroke::Point::y()
{
	return y_;
}

Character::Stroke::Point::value_t const &
Character::Stroke::Point::x() const
{
	return x_;
}

Character::Stroke::Point::value_t const &
Character::Stroke::Point::y() const
{
	return y_;
}

double
Character::Stroke::Point::arg() const
{
	return std::atan( static_cast<double>(y_) / x_ );
}

double
Character::Stroke::Point::abs() const
{
	return std::sqrt(x_*x_ + y_*y_);
}

Character::Stroke::Point::value_t
Character::Stroke::Point::set_arg(double const a)
{
	value_t old_len = abs();
	x_ = std::acos(a) * old_len;
	y_ = std::asin(a) * old_len;
	return arg();
}

Character::Stroke::Point::value_t
Character::Stroke::Point::set_length(double const l)
{
	double factor = l / abs();
	x_ = x_ * factor;
	y_ = y_ * factor;
	return abs();
}


Character::Stroke::Point &
Character::Stroke::Point::operator += (const Character::Stroke::Point & rhs)
{
	x_ += rhs.x_;
	y_ += rhs.y_;
	return *this;
}

Character::Stroke::Point &
Character::Stroke::Point::operator -= (const Character::Stroke::Point & rhs)
{
	x_ -= rhs.x_;
	y_ -= rhs.y_;
	return *this;
}

void
Character::Stroke::add_point(Character::Stroke::Point const & p)
{
	points_.push_back(p);
}

void
Character::Stroke::add_point(
	Character::Stroke::Point::value_t const x,
	Character::Stroke::Point::value_t const y)
{
	points_.push_back(Character::Stroke::Point(x, y));
}

Character::Stroke::Point::iterator
Character::Stroke::points_begin()
{
	return points_.begin();
}

Character::Stroke::Point::iterator
Character::Stroke::points_end()
{
	return points_.end();
}

Character::Stroke::Point::const_iterator
Character::Stroke::points_begin() const
{
	return points_.begin();
}

Character::Stroke::Point::const_iterator
Character::Stroke::points_end() const
{
	return points_.end();
}

size_t
Character::Stroke::point_count() const
{
	return points_.size();
}

bool
Character::Stroke::point_empty() const
{
	return points_.empty();
}

Character::Stroke::value_t
Character::Stroke::length() const
{
	Point::value_t length = Point::value_t();
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

Character::Character(const std::string & name):
	name_(name)
{
}

void
Character::new_stroke()
{
	strokes_.push_back(Stroke());
}

void
Character::add_stroke(Character::Stroke const & s)
{
	strokes_.push_back(s);
}

size_t
Character::stroke_count() const
{
	return strokes_.size();
}

bool
Character::stroke_empty() const
{
	return strokes_.empty();
}

void
Character::add_point(Character::Stroke::Point const & p)
	{ strokes_.rbegin()->add_point(p); };
void
Character::add_point(
	Character::Stroke::Point::value_t const x,
	Character::Stroke::Point::value_t const y)
{
	strokes_.rbegin()->add_point(x, y);
}

std::string const
Character::get_name() const
{
	return name_;
}

void
Character::set_name(std::string const & n)
{
	name_ = n;
}

Character::Stroke::iterator
Character::strokes_begin()
{
	return strokes_.begin();
}

Character::Stroke::iterator
Character::strokes_end()
{
	return strokes_.end();
}

Character::Stroke::const_iterator
Character::strokes_begin() const
{
	return strokes_.begin();
}

Character::Stroke::const_iterator
Character::strokes_end() const
{
	return strokes_.end();
}

Character::Stroke::Point
operator + (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs)
{
	return Character::Stroke::Point(
		lhs.x() + rhs.x(),
		lhs.y() + rhs.y()
	);
}

Character::Stroke::Point
operator - (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs)
{
	return Character::Stroke::Point(
		lhs.x() - rhs.x(),
		lhs.y() - rhs.y()
	);
}

bool
operator == (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs)
{
	return (lhs.x() == rhs.x() && lhs.y() == rhs.y());
}

bool
operator != (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs)
{
	return (lhs.x() != rhs.x() || lhs.y() != rhs.y());
}

bool
operator < (Character const & lhs, Character const & rhs)
{
	return (lhs.get_name() < rhs.get_name());
}

std::ostream &
operator << (std::ostream & lhs, Character::Stroke::Point const & rhs)
{
	return lhs << "(" << rhs.x() << ", " << rhs.y() << ")";
}

std::ostream &
operator << (std::ostream & lhs, Character::Stroke const & rhs)
{
	for (Character::Stroke::Point::const_iterator pi = rhs.points_begin();
	     pi != rhs.points_end();
	     ++pi)
		lhs << *pi << " ";
	return lhs;
}

std::ostream &
operator << (std::ostream & lhs, Character const & rhs)
{
	lhs << "Character \"" << rhs.get_name() << "\":" << std::endl;
	for (Character::Stroke::const_iterator si = rhs.strokes_begin();
	     si != rhs.strokes_end();
	     ++si)
		lhs << "\t" << *si << std::endl;
	return lhs;
}

} // namespace chrasis

