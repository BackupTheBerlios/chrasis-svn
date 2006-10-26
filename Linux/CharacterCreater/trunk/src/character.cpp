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

#include <iomanip>
#include "character.h"

std::ostream &
operator << (std::ostream & lhs, Point const & rhs)
{
	lhs << "(" << rhs.x() << ", " << rhs.y() << ")";
	return lhs;
}

Point
operator - (const Point & lhs, const Point & rhs)
{
	return Point(rhs.x() - lhs.x(), rhs.y() - lhs.y());
}

bool
operator != (const Point & lhs, const Point & rhs)
{
	return lhs.x() != rhs.x() || lhs.y() != rhs.y();
}

std::ostream &
operator<<(std::ostream & lhs, Stroke const & rhs)
{
	for (Point::const_iterator i = rhs.points_.begin();
		i != rhs.points_.end();
		++i)
		lhs << *i << " ";
	return lhs;
}

std::ostream &
operator<<(std::ostream & lhs, Character const & rhs)
{
	for (Stroke::const_iterator i = rhs.strokes_.begin();
		i != rhs.strokes_.end();
		++i)
		lhs << *i << std::endl;
	return lhs;
}

bool
operator<(Character const & lhs, Character const & rhs)
{
	return (lhs.get_name() < rhs.get_name())?true:false;
}

template <typename T>
T _abs(T _num)
{
	return (_num<T())?-_num:_num;
}

// !!! FIXME !!! 
// !!! Magic Numbers !!!
static const double ANGLE_THRESHOLD = 45.0 / 180.0 * MATH_PI;	//< 45 deg
static const double DIST_THRESHOLD = 1.0 / 7.0;			//< 1/7 of the diagonal line
Stroke Stroke::normalize()
{
	Point::collection normalized(points_);

	// find threshold;
	Point l_u(*(points_.begin())),
	      r_b(*(points_.begin()));
	for (Point::iterator pi = points_.begin();
	     pi != points_.end() - 1;
	     ++pi)
	{
		if (l_u.x() > pi->x())
			l_u.x() = pi->x();
		if (l_u.y() > pi->y())
			l_u.y() = pi->y();
		if (r_b.x() < pi->x())
			r_b.x() = pi->x();
		if (r_b.y() < pi->y())
			r_b.y() = pi->y();
	}
	int dist_threshold = (l_u - r_b).abs() * DIST_THRESHOLD;

	// get rid of useless points
	bool erased_something = true;
	while (erased_something)
	{
		erased_something = false;

		// erase segment with too-short distance
		bool erased_dist = true;
		while (erased_dist)
		{
			erased_dist = false;
			int norm_size = normalized.size();
			for (Point::iterator pi = normalized.begin();
			     pi != normalized.end();)
			{
				Point::iterator pf = pi, pl = pi + 1;
				if (pl == normalized.end())
					break;
				double distance = (*pf - *pl).abs();

				if (distance < dist_threshold)
				{
					Point pm((pf->x() + pl->x()) / 2, (pf->y() + pl->y()) / 2);
					erased_something = erased_dist = true;
					*pf = pm;
					pi = normalized.erase(pl);
				}
				else
					++pi;
			}
		}
		// head-tail fix
		Point::iterator pfrom, pto;
		pfrom = points_.begin();
		pto = normalized.begin();
		if (*pfrom != *pto)
		{
			double distance = (*pfrom - *pto).abs();
			if (distance < dist_threshold)
			{
				erased_something = true;
				*pto = *pfrom;
			}
			else
				normalized.insert(pto, *pfrom);
		}
		pfrom = points_.end() - 1;
		pto = normalized.end() - 1;
		if (*pfrom != *pto)
		{
			double distance = (*pfrom - *pto).abs();
			if (distance < dist_threshold)
			{
				erased_something = true;
				*pto = *pfrom;
			}
			else
				normalized.push_back(*pfrom);
		}
		
		// erase segment with too-close angles
		bool erased_angle = true;
		while (erased_angle)
		{
			erased_angle = false;
			for (Point::iterator pi = normalized.begin();
			     pi != normalized.end();)
			{
				Point::iterator pf = pi, pm = pi+1, pl = pi+2;
				if (pm == normalized.end() || pl == normalized.end())
					break;
				double diff1 = (*pf - *pm).arg(),
				       diff2 = (*pm - *pl).arg();
				if (_abs(diff1 - diff2) < ANGLE_THRESHOLD)
				{
					erased_something = erased_angle = true;
					pi = normalized.erase(pm);
				}
				else
					++pi;
			}
		}
	}

	// snap to origional stroke
	for (Point::iterator pi = normalized.begin() + 1;
	     pi != normalized.end() - 1;
	     ++pi)
	{
		Point p(*pi);
		double min_distance = (p - *(points_.begin())).abs();
		for (Point::iterator ppi = points_.begin();
		     ppi != points_.end();
		     ++ppi)
		{
			double distance = (p - *ppi).abs();
			if (distance < min_distance)
			{
				min_distance = distance;
				*pi = *ppi;
			}
		}
	}

	Stroke ret;
	ret.points_ = normalized;
	return ret;
/*
 * abandond due to stupidity...
 *
	// Polygon approximation
	//
	// Suppose a line pass through (x1, y1), (x2, y2). What is the
	// perpendicular distance d of point (x0, y0) from the line?
	//
	//       |  (x1-x0)(y2-y1) - (y1-y0)(x2-x1)  |
	// d   = | --------------------------------- |
	//       |   sqrt( (x2-x1)^2 + (y2-y1)^2 )   |
	//
	//        ( (x1-x0)(y2-y1) - (y1-y0)(x2-x1) )^2
	// d^2 = ---------------------------------------
	//               (x2-x1)^2 + (y2-y1)^2
	//
	// threshold = 1/4 * [diagonal line of covered rectangle]
	//
	//  o----------------+
	//  | p              |   p: points of the stroke
	//  |p               |   o: the 2 angle of the rectangle
	//  |p               |
	//  |p               |
	//  p                p
	//  p                p
	//  |ppppp    ppppppp|
	//  +-----pppp-------o
	//

	// calculate the threshold
	Point l_u(*(points_.begin())),
	      r_b(*(points_.begin()));
	for (Point::iterator pi = points_.begin();
	     pi != points_.end() - 1;
	     ++pi)
	{
		if (l_u.x() > pi->x())
			l_u.x() = pi->x();
		if (l_u.y() > pi->y())
			l_u.y() = pi->y();
		if (r_b.x() < pi->x())
			r_b.x() = pi->x();
		if (r_b.y() < pi->y())
			r_b.y() = pi->y();
	}
	int threshold = (l_u - r_b).abs() / 4;

	// approximate each points
	Stroke::collection ret;

	bool within_range = false;
	for (int i=1;!within_range;++i)
	{
		// prepare seperated strokes
		ret = Stroke::collection(i);
		ret[0].add_point(points_[0]);

		for(int adv = 1;adv < i;++adv)
		{
			ret[adv-1].add_point(points_[adv * points_.size() / i]);
			if (adv < ret.size())
				ret[adv].add_point(points_[adv * points_.size() / i]);
		}
		ret[i-1].add_point(*(points_.rbegin()));

		within_range = (i==2);
	}

	return ret;
*/
}
