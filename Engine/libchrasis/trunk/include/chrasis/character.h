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

#ifndef _CHARACTER_H
#define _CHARACTER_H

#ifndef _CHRASIS_H
#warning "shouldn't be including this file directly!"
#warning "try #include <chrasis.h> instead."
#endif

#include <iostream>
#include <cmath>
#include <vector>

namespace chrasis
{

/**
 * A Character is the basic unit of recognition.
 */

class CHRASIS_API Character
{
public:
	class CHRASIS_API Stroke
	{
	public:
		class CHRASIS_API Point
		{
		public:
			typedef int				value_t;	///< value type of point x, y
			typedef std::vector< Point >		container;	///< container type of points
			/// iterator walks through a collection of points
			typedef container::iterator		iterator;
			/// iterator walks through a collection of const points
			typedef container::const_iterator	const_iterator;

			/**
			 * create a new point (x, y)
			 * @param x x coordinate
			 * @param y y coordinate
			 */
			Point(value_t const x = 0, value_t const y = 0);

			value_t & x();			///< x-coordinate
			value_t & y();			///< y-coordinate
			value_t const & x() const;	///< x-coordinate (const)
			value_t const & y() const;	///< y-coordinate (const)

			double arg() const;		///< argument of the point
			double abs() const;		/// the distance between the point and origin

			/// set argument
			value_t set_arg(double const a);
			/// set the distance between the point and origin
			value_t set_length(double const l);

			/// add two points together
			Point & operator += (Point const & rhs);
			/// minus the point by another
			Point & operator -= (Point const & rhs);
		private:
			value_t x_, y_;
		};

		typedef Point::value_t			value_t;	///< value type of point x, y
		typedef std::vector< Stroke >		container;	///< container type of strokes
		/// iterator walks through a collection of strokes
		typedef container::iterator		iterator;
		/// iterator walks through a collection of const strokes
		typedef container::const_iterator	const_iterator;

		/**
		 * add a new point to the end of the stroke
		 * @param p the point to be added
		 */
		void add_point(Point const & p);
		/**
		 * add a new point to the end of the stroke
		 * @param x x coordinate of the point to be added
		 * @param y y coordinate of the point to be added
		 */
		void add_point(value_t const x, value_t const y);

		/**
		 * get the iterator points to the first point of the stroke
		 * @return iterator of the first point of the stroke
		 */
		Point::iterator points_begin();
		/**
		 * get the iterator points to the pass-the-end point of the stroke
		 * @return iterator of the pass-the-end point of the stroke
		 */
		Point::iterator points_end();
		/**
		 * get the const iterator points to the first point of the stroke
		 * @return const iterator of the first point of the stroke
		 */
		Point::const_iterator points_begin() const;
		/**
		 * get the const iterator points to the pass-the-end point of the stroke
		 * @return const iterator of the pass-the-end point of the stroke
		 */
		Point::const_iterator points_end() const;

		/**
		 * get the number of points of the stroke
		 * @return number of points of the stroke
		 */
		size_t point_count() const;
		
		/**
		 * test if the stroke is empty (contains no point)
		 * @return true if contains no point, false otherwise.
		 */
		bool point_empty() const;
		
		/**
		 * get the length of the stroke
		 * @return length of the stroke
		 */
		value_t length() const;

	private:
		Point::container points_;
	};

	typedef Stroke::Point::value_t		value_t;	///< value type of point x, y
	typedef std::vector< Character >	container;	///< container type of characters
	/// iterator walks through a collection of characters
	typedef container::iterator		iterator;
	/// iterator walks through a collection of const characters
	typedef container::const_iterator	const_iterator;

	/**
	 * create a character with a given name
	 * @param name the name of the character, default to empty string.
	 */
	Character(std::string const & name = std::string());

	/**
	 * add an empty stroke to the end of the character
	 */
	void new_stroke();
	/**
	 * add the given stroke to the end of the character
	 * @param s the stroke to be added
	 */
	void add_stroke(Stroke const & s);

	/**
	 * get the number of strokes of the character
	 * @return number of strokes
	 */
	size_t stroke_count() const;
	/**
	 * test if the character is empty (contains no stroke)
	 * @return true if contains no stroke, false otherwise.
	 */
	bool stroke_empty() const;

	/**
	 * add a point to the last stroke of the character
	 * @param p the point to be added
	 */
	void add_point(Stroke::Point const & p);
	/**
	 * add a point to the last stroke of the character
	 * @param x x coordinate of the point to be added
	 * @param y y coordinate of the point to be added
	 */
	void add_point(value_t const x, value_t const y);

	/**
	 * get the name of the character
	 * @return the name
	 */
	std::string const get_name() const;
	/**
	 * set the name of the character
	 * @param n new name
	 */
	void set_name(std::string const & n);

	/**
	 * get the iterator points to the first stroke of the character
	 * @return iterator of the first stroke of the character
	 */
	Stroke::iterator strokes_begin();
	/**
	 * get the iterator points to the pass-the-end stroke of the character
	 * @return iterator of the last pass-the-end stroke of the character
	 */
	Stroke::iterator strokes_end();
	/**
	 * get the const iterator points to the first stroke of the character
	 * @return const iterator of the first stroke of the character
	 */
	Stroke::const_iterator strokes_begin() const;
	/**
	 * get the const iterator points to the pass-the-end stroke of the character
	 * @return const iterator of the pass-the-end stroke of the character
	 */
	Stroke::const_iterator strokes_end() const;
private:
	Stroke::container strokes_;
	std::string name_;
};

Character::Stroke::Point
operator + (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs);

Character::Stroke::Point
operator - (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs);

bool
operator == (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs);

bool
operator != (	Character::Stroke::Point const & lhs,
		Character::Stroke::Point const & rhs);

bool
operator < (Character const & lhs, Character const & rhs);

std::ostream &
operator << (std::ostream & lhs, Character::Stroke::Point const & rhs);

std::ostream &
operator << (std::ostream & lhs, Character::Stroke const & rhs);

std::ostream &
operator << (std::ostream & lhs, Character const & rhs);

/// alias for Stroke
typedef Character::Stroke		Stroke;
/// alias for Point
typedef Character::Stroke::Point	Point;

} // namespace chrasis

#endif // #ifndef _CHARACTER_H
