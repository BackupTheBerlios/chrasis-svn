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

#include <chrasis.h>
#include <chrasis/internal.h>

namespace chrasis
{

// !!! FIXME !!! 
// !!! Magic Numbers !!!
static const double ANGLE_THRESHOLD		= 30.0 / 180.0 * M_PI;	//< 30 deg
static const double DIST_THRESHOLD_RATIO	= 1.0 / 15.0;		//< 1/15 of diagonal line

Stroke
_normalize( const Stroke & orig_stroke, double const dist_threshold )
{
	Point::collection normalized;
	copy(orig_stroke.points_begin(), orig_stroke.points_end(), back_inserter(normalized));

	// get rid of useless points
	bool erased_something;
	do
	{
		// erase segment with too-close angles
		bool erased_angle;
		do
		{
			erased_angle = false;
			for (Point::iterator pi = normalized.begin();
			     pi != normalized.end();)
			{
				Point::iterator pf = pi, pm = pi+1, pl = pi+2;
				if (pm == normalized.end() || pl == normalized.end())
					break;
				double angle = std::min(
					abs((*pf - *pm).arg() - (*pm - *pl).arg()),
					abs((*pf - *pm).arg() + (*pm - *pl).arg())
				);
				if (angle < ANGLE_THRESHOLD)
				{
					erased_angle = true;
					pi = normalized.erase(pm);
				}
				else
					++pi;
			}
		}
		while (erased_angle);
		erased_something = erased_angle;

		// erase segment with too-short distance
		int norm_size = normalized.size();
		for (Point::iterator pi = normalized.begin();
		     pi != normalized.end();)
		{
			Point::iterator pf = pi, pl = pi + 1;
			if (pl == normalized.end())
				break;
			if ((*pf - *pl).abs() < dist_threshold)
			{
				Point pm((pf->x() + pl->x()) / 2, (pf->y() + pl->y()) / 2);
				erased_something = true;
				*pf = pm;
				pi = normalized.erase(pl);
			}
			else
				++pi;
		}
	}
	while (erased_something);

	Stroke ret;
	for (Point::iterator pi = normalized.begin();
	     pi != normalized.end();
	     ++pi)
		ret.add_point(*pi);
	return ret;
}

Character
_normalize(const Character & chr)
{
	if (chr.stroke_count() == 0 ||
	    chr.strokes_begin()->point_count() == 0)
		return chr;

	Character ret(chr.get_name());

	// find left-top and right-bottom point
	Point lt = *(chr.strokes_begin()->points_begin()),
	      rb = *(chr.strokes_begin()->points_begin());
	for (Stroke::const_iterator si = chr.strokes_begin();
	     si != chr.strokes_end();
	     ++si)
	{
		for (Point::const_iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			if (pi->x() < lt.x())
				lt.x() = pi->x();
			if (pi->y() < lt.y())
				lt.y() = pi->y();
			if (pi->x() > rb.x())
				rb.x() = pi->x();
			if (pi->y() > rb.y())
				rb.y() = pi->y();
		}
	}

	// okay, find our needed info:
	//   adjusted left-top point
	//   distance
	Point::value_t const distance = std::max(
		abs(lt.x() - rb.x()),
		abs(lt.y() - rb.y())
	);
	lt.x() = (lt.x() + rb.x() - distance) / 2;
	lt.y() = (lt.y() + rb.y() - distance) / 2;

	// walk through each strokes, simplize each strokes
	for (Stroke::const_iterator si = chr.strokes_begin();
	     si != chr.strokes_end();
	     ++si)
	{
		ret.add_stroke(
			_normalize(
				*si,
				static_cast<int>(std::sqrt(distance*distance) * DIST_THRESHOLD_RATIO)
			)
		);
	}

	// walk through the character and adjust the point to range [0...40000)
	// because:
	// 	sqrt(2^31 - 1) ~= 46,340
	// this way we avoid the case that (x * x + y * y) > INT_MAX
	for (Stroke::iterator si = ret.strokes_begin();
	     si != ret.strokes_end();
	     ++si)
	{
		for (Point::iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			*pi -= lt;
			pi->x() = static_cast<int>((static_cast<double>(pi->x()) / distance) * 40000);
			pi->y() = static_cast<int>((static_cast<double>(pi->y()) / distance) * 40000);
		}
	}

	return ret;
}


} // namespace chrasis
