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

class Point
{
public:
	friend std::ostream & operator<<(std::ostream &, Point const &);

	Point(const double x, const double y): x_(x), y_(y) {};
	
	double const x() { return x_; };
	double const y() { return y_; };
private:
	double x_;
	double y_;
};

class Stroke
{
public:
	typedef std::vector<Point> point_collection;
	typedef std::vector<Point>::iterator points_iterator;
	typedef std::vector<Point>::const_iterator points_const_iterator;

	friend std::ostream & operator<<(std::ostream & lhs, Stroke const &);

	void add_point(Point const & p) { points_.push_back(p); };
	void add_point(const double x, const double y) { points_.push_back(Point(x, y)); };

	points_iterator points_begin() { return points_.begin(); };
	points_iterator points_end() { return points_.end(); };
private:
	point_collection points_;
};

class Character
{
public:
	typedef std::vector<Stroke> stroke_collection;
	typedef std::vector<Stroke>::iterator strokes_iterator;
	typedef std::vector<Stroke>::const_iterator strokes_const_iterator;

	friend std::ostream & operator<<(std::ostream &, Character const &);

	Character(): name_() {};
	Character(const std::string & n): name_(n) {};

	void add_stroke(Stroke & s) { strokes_.push_back(s); };

	void new_stroke() { strokes_.push_back(Stroke()); };
	void add_point(Point const & p) { strokes_.rbegin()->add_point(p); };
	void add_point(const double x, const double y) { strokes_.rbegin()->add_point(x, y); };

	const std::string get_name() const { return name_; };
	void set_name(const std::string & n) { name_ = n; };

	strokes_iterator strokes_begin() { return strokes_.begin(); };
	strokes_iterator strokes_end() { return strokes_.end(); };
private:
	stroke_collection strokes_;
	std::string name_;
};

bool operator<(Character const &, Character const &);

typedef std::vector<Character> character_collection;
typedef std::vector<Character>::iterator characters_iterator;
typedef std::vector<Character>::const_iterator characters_const_iterator;

#endif
