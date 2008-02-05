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

#include <iterator>

namespace chrasis
{

CHRASIS_API
Stroke
normalize( const Stroke & orig_stroke, int const dist_threshold )
{
	Point::container normalized;
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

CHRASIS_API
Character
normalize(const Character & chr)
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
			if (pi->x() < lt.x()) lt.x() = pi->x();
			if (pi->y() < lt.y()) lt.y() = pi->y();
			if (pi->x() > rb.x()) rb.x() = pi->x();
			if (pi->y() > rb.y()) rb.y() = pi->y();
		}
	}
	Point::value_t const distance = std::max(
		abs(lt.x() - rb.x()), abs(lt.y() - rb.y())
	);
	lt.x() = (lt.x() + rb.x() - distance) / 2;
	lt.y() = (lt.y() + rb.y() - distance) / 2;

	// walk through each strokes and simplize them
	for (Stroke::const_iterator si = chr.strokes_begin();
	     si != chr.strokes_end();
	     ++si)
		ret.add_stroke(normalize(*si, distance * DIST_THRESHOLD_RATIO));

	// walk through the character and adjust the point to range [0...RESOLUTION)
	for (Stroke::iterator si = ret.strokes_begin();
	     si != ret.strokes_end();
	     ++si)
		for (Point::iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			*pi -= lt;
			pi->x() = static_cast<int>((static_cast<double>(pi->x()) / distance) * RESOLUTION);
			pi->y() = static_cast<int>((static_cast<double>(pi->y()) / distance) * RESOLUTION);
		}

	return ret;
}

CHRASIS_API
ItemPossibilityList
recognize(Character const & nchr)
{
	static SQLite::Database
		sys_db( platform::system_database_path() ),
		usr_db( platform::user_database_path() );

	SQLite::Command
		sys_cmd( sys_db ),
		usr_cmd( usr_db );

	// TODO: parallel with threads?
	ItemPossibilityList ret;
	ret = _recognize(nchr, sys_cmd),
	ret += _recognize(nchr, usr_cmd);
	ret.sort(ItemPossibilityList::SORTING_POSSIBILITY);

	return ret;
}

CHRASIS_API
ItemPossibilityList
recognize(Character const & nchr, SQLite::Command & cmd)
{
	return _recognize(nchr, cmd);
}

CHRASIS_API
bool
learn(Character const & nchr)
{
	static SQLite::Database usr_db( platform::user_database_path() );
	SQLite::Command usr_cmd( usr_db );

	return _learn(nchr, usr_cmd);
}

CHRASIS_API
bool
learn(Character const & nchr, SQLite::Command & cmd)
{
	return _learn(nchr, cmd);
}

} // namespace chrasis
