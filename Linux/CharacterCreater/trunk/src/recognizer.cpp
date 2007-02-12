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
 * $Id: chmlcodec.h 18 2006-09-19 21:18:42Z palatis $
 */

#include "common.h"


#include "common.h"
#include "recognizer.h"

#include <iterator>
#include <algorithm>

Recognizer & Recognizer::Instance()
{
	static Recognizer ret;
	return ret;
}

// !!! FIXME !!! 
// !!! Magic Numbers !!!
static const double ANGLE_THRESHOLD = 30.0 / 180.0 * M_PI;	//< 30 deg
static const double DIST_THRESHOLD = 1.0 / 10.0;		//< 1/10 of the total length
Stroke
Recognizer::normalize( const Stroke & orig_stroke ) const
{
	Point::collection normalized;
	copy(orig_stroke.points_begin(), orig_stroke.points_end(), back_inserter(normalized));

	// find threshold;
	Point::value_t dist_threshold = orig_stroke.length() * DIST_THRESHOLD;

	// get rid of useless points
	bool erased_something = true;
	while (erased_something)
	{
		erased_something = false;

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
				double angle = std::min(
					abs((*pf - *pm).arg() - (*pm - *pl).arg()),
					abs((*pf - *pm).arg() + (*pm - *pl).arg()));
				if (angle < ANGLE_THRESHOLD)
				{
					erased_something = erased_angle = true;
					pi = normalized.erase(pm);
				}
				else
					++pi;
			}
		}

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
		// snap to origional stroke
		for (Point::iterator pi = normalized.begin();
		     pi != normalized.end();
		     ++pi)
		{
			Point p(*pi);
			double min_distance = (p - *(orig_stroke.points_begin())).abs();
			for (Point::const_iterator ppi = orig_stroke.points_begin();
			     ppi != orig_stroke.points_end();
			     ++ppi)
			{
				if ((p - *ppi).abs() < min_distance)
				{
					min_distance = (p - *ppi).abs();
					*pi = *ppi;
				}
			}
		}
	}

	Stroke ret;
	for (Point::iterator pi = normalized.begin();
	     pi != normalized.end();
	     ++pi)
		ret.add_point(*pi);
	return ret;
}

Character
Recognizer::normalize(const Character & character) const
{
	if (character.stroke_count() == 0 ||
	    character.strokes_begin()->point_count() == 0)
		return character;

	Character ret(character.get_name());

	for (Stroke::const_iterator si = character.strokes_begin();
	     si != character.strokes_end();
	     ++si)
	{
		ret.add_stroke(normalize(*si));
	}

	// find left-top and right-bottom point
	Point lt = *(ret.strokes_begin()->points_begin()),
	      rb = *(ret.strokes_begin()->points_begin());
	for (Stroke::const_iterator si = ret.strokes_begin();
	     si != ret.strokes_end();
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

	// okay, find our need info:
	//   adjusted left-top point
	//   distance
	Point::value_t const distance = std::max(
		abs(lt.x() - rb.x()),
		abs(lt.y() - rb.y())
	);
	lt.x() = (lt.x() + rb.x() - distance) / 2;
	lt.y() = (lt.y() + rb.y() - distance) / 2;

	// walk through the character
	for (Stroke::iterator si = ret.strokes_begin();
	     si != ret.strokes_end();
	     ++si)
	{
		for (Point::iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			*pi -= lt;
			pi->x() /= distance;
			pi->y() /= distance;
		}
	}

	return ret;
}

Recognizer::character_possibility_t
Recognizer::recognize(Character const & chr, Database & db) const
{
	character_possibility_t ret;

	Query q(db);

	// find character
	std::string sql(
		"SELECT character_name, character_id FROM characters WHERE"
		"	stroke_count IN (" + toString(chr.stroke_count())
	);
	if (chr.get_name() != "")
		sql += ") AND character_name IN ('" + chr.get_name() + "'";
	for (Stroke::const_iterator si = chr.strokes_begin();
	     si != chr.strokes_end();
	     ++si)
	{
		sql += ") AND character_id IN ("
		       "	SELECT character_id FROM strokes WHERE "
		       "		pt_count IN (" + toString(si->point_count()) + ") AND "
		       "		sequence IN (" + toString(si - chr.strokes_begin()) + ")";
	}
	sql += ");";

	// collecting results
	std::map< int, std::string > id_name;
	q.get_result(sql);
	while (q.more_rows())
	{
		ResultRow r(q.fetch_row());
		id_name[r.get_int("character_id")] = r.get_text("character_name");
	}
	q.free_result();

	// retrieving points for possibility
	std::string ids;
	for(std::map< int, std::string >::iterator it = id_name.begin();
	    it != id_name.end();
	    ++it)
		ids += toString(it->first) + ", ";
	ids = ids.substr(0, ids.size() - 2);
	sql =	"SELECT points.x, points.y FROM points, strokes, characters WHERE"
		"	points.stroke_id = strokes.stroke_id AND"
		"	strokes.character_id = characters.character_id AND"
		"	characters.character_id IN (" + ids + ") "
		"ORDER BY"
		"	strokes.character_id ASC, strokes.sequence ASC, points.sequence ASC;";

	// calculate possibility and return the result
	q.get_result(sql);
	for(std::map< int, std::string >::iterator it = id_name.begin();
	    it != id_name.end();
	    ++it)
	{
		double c_possib = 0;
		for (Stroke::const_iterator si = chr.strokes_begin();
		     si != chr.strokes_end();
		     ++si)
		{
			double s_possib = 0;
			for (Point::const_iterator pi = si->points_begin();
			     pi != si->points_end();
			     ++pi)
			{
				if (!q.more_rows())
					return ret;
				ResultRow r(q.fetch_row());
				double xd = r.get_real("x") - pi->x(),
				       yd = r.get_real("y") - pi->y();
				s_possib += std::sqrt(xd * xd + yd * yd);
			}
			c_possib += s_possib / si->point_count();
		}
		ret[ c_possib / chr.stroke_count() ] = std::make_pair(it->first, it->second);
	}
	q.free_result();

	return ret;
}
