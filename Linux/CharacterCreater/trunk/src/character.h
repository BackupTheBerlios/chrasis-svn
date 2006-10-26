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

#include <iostream>
#include <vector>
#include <complex>

static const double MATH_PI = 3.14159265359;

class Point
{
public:
	typedef double					value_t;

	typedef std::vector<Point>			collection;
	typedef std::vector<Point>::iterator		iterator;
	typedef std::vector<Point>::const_iterator	const_iterator;

	friend std::ostream & operator<<(std::ostream &, Point const &);

	Point(const value_t x, const value_t y): num_(x, y) {};

	value_t & x() { return num_.real(); };			//< x-coordinate
	value_t & y() { return num_.imag(); };			//< y-coordinate
	value_t const & x() const { return num_.real(); };	//< const x-coordinate
	value_t const & y() const { return num_.imag(); };	//< const y-coordinate

	value_t arg() const { return std::arg(num_); };		//< the argument (angle) of the point
	value_t norm() const { return std::norm(num_); };	//< the norm of the point
	value_t abs() const { return std::abs(num_); };		//< the distance between the point and origin
private:
	std::complex< value_t > num_;
};

Point operator - (const Point &, const Point &);
bool operator != (const Point &, const Point &);

class Stroke
{
public:
	typedef std::vector<Stroke>			collection;
	typedef std::vector<Stroke>::iterator		iterator;
	typedef std::vector<Stroke>::const_iterator	const_iterator;

	friend std::ostream & operator<<(std::ostream & lhs, Stroke const &);

	void add_point(Point const & p) { points_.push_back(p); };
	void add_point(const Point::value_t x, const Point::value_t y)
		{ points_.push_back(Point(x, y)); };

	Point::iterator points_begin() { return points_.begin(); };
	Point::iterator points_end() { return points_.end(); };

	size_t size() { return points_.size(); }

	Stroke normalize();
private:
	Point::collection points_;
};

class Character
{
public:
	typedef std::vector<Character>			collection;
	typedef std::vector<Character>::iterator	iterator;
	typedef std::vector<Character>::const_iterator	const_iterator;

	friend std::ostream & operator<<(std::ostream &, Character const &);

	Character(): name_() {};
	Character(const std::string & n): name_(n) {};

	void add_stroke(Stroke & s) { strokes_.push_back(s); };

	void new_stroke() { strokes_.push_back(Stroke()); };
	void add_point(Point const & p) { strokes_.rbegin()->add_point(p); };
	void add_point(const Point::value_t x, const Point::value_t y) { strokes_.rbegin()->add_point(x, y); };

	const std::string get_name() const { return name_; };
	void set_name(const std::string & n) { name_ = n; };

	Stroke::iterator strokes_begin() { return strokes_.begin(); };
	Stroke::iterator strokes_end() { return strokes_.end(); };
private:
	Stroke::collection strokes_;
	std::string name_;
};

bool operator<(Character const &, Character const &);

#endif
