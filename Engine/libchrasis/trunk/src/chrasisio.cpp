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

	Point::value_t const distance = std::max(
		abs(lt.x() - rb.x()),
		abs(lt.y() - rb.y())
	);
	lt.x() = (lt.x() + rb.x() - distance) / 2;
	lt.y() = (lt.y() + rb.y() - distance) / 2;
	
	// walk through each strokes and simplize them
	int distance_threshold = static_cast<int>(std::sqrt(distance*distance) * DIST_THRESHOLD_RATIO);
	for (Stroke::const_iterator si = chr.strokes_begin();
	     si != chr.strokes_end();
	     ++si)
		ret.add_stroke(_normalize(*si, distance_threshold));

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
character_possibility_t
recognize(Character const & nchr, Database & db)
{
	character_possibility_t ret;

	char_traits_t t;
	for (Stroke::const_iterator si = nchr.strokes_begin();
	     si != nchr.strokes_end();
	     ++si)
		t.push_back(si->point_count());
	
	character_ids_t likely_ids = _get_cids_by_traits(t, nchr.get_name(), db);

	// calculate possibility and return the result
	for(character_ids_t::iterator it = likely_ids.begin();
	    it != likely_ids.end();
	    ++it)
	{
		Character likely = _get_char_by_id(*it, db);
		int c_possib = 0;
		for (Stroke::const_iterator
			si	= nchr.strokes_begin(),
			lsi	= likely.strokes_begin();
		     si != nchr.strokes_end() &&
		     lsi != likely.strokes_end();
		     ++si,
		     ++lsi)
		{
			int s_possib = 0;
			for (Point::const_iterator
				pi	= si->points_begin(),
				lpi	= lsi->points_begin();
			     pi != si->points_end() &&
			     lpi != lsi->points_end();
			     ++pi,
			     ++lpi)
			{
				s_possib += static_cast<int>(
					std::sqrt(
						(lpi->x() - pi->x()) * (lpi->x() - pi->x()) +
						(lpi->y() - pi->y()) * (lpi->y() - pi->y())
					)
				);
			}
			c_possib += s_possib / si->point_count();
		}
		ret.insert(
			std::make_pair(
				c_possib / nchr.stroke_count(),
				std::make_pair(
					*it,
					likely.get_name()
				)
			)
		);
	}

	return ret;
}

CHRASIS_API
bool
learn(Character const & nchr, Database & db)
{
	character_possibility_t likely = recognize(nchr, db);

	if ( likely.size() == 0 ||
	     likely.begin()->first > static_cast<int>(RESOLUTION * LEARNING_THRESHOLD) )
		return _remember(nchr, db);
	else
		return _reflect(nchr, likely.begin()->second.first, db);
	
	return false;
}


namespace settings
{

CHRASIS_API
std::string
system_database_path()
{
	return CHRASIS_DATADIR "/" DEFAULT_DB_FILE;
}

CHRASIS_API
std::string
empty_database_path()
{
	return CHRASIS_DATADIR "/" DEFAULT_EMPTYDB_FILE;
}

CHRASIS_API
std::string
user_database_path()
{
	std::string userdir = getenv("HOME");
	return userdir + "/.chrasis/" DEFAULT_DB_FILE;
}

CHRASIS_API
std::string
database_schema_path()
{
	return CHRASIS_DATADIR "/" DEFAULT_SCHEMA_FILE;
}

CHRASIS_API
bool
initialize_userdir()
{
	std::string userdir = getenv("HOME");
	userdir += "/.chrasis";

	if ( mkdir( userdir.c_str(), 0755 ) != 0 )
	{
		struct stat st;
		stat( userdir.c_str(), &st );

		if ( !S_ISDIR(st.st_mode) )
		{
			std::cerr << "Failed to create \"" << userdir << "\"." << std::endl;
			return false;
		}
	}

	if ( access( user_database_path().c_str(), W_OK) != 0 )
	{
		std::ifstream fin(
			empty_database_path().c_str(),
			std::ios_base::binary);
		std::ofstream fout(
			user_database_path().c_str(),
			std::ios_base::binary);
		fout << fin.rdbuf();
		fin.close();
		fout.close();
	}

	return true;
}

} // namespace settings

} // namespace chrasis
