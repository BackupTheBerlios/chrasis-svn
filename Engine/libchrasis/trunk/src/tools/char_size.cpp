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

#include "chmlcodec.h"

using namespace chrasis;
using namespace std;

Point::value_t
char_size(const Character & chr)
{
	if (chr.stroke_count() == 0 ||
	    chr.strokes_begin()->point_count() == 0)
		return 0;

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

	return std::max(
		abs(lt.x() - rb.x()),
		abs(lt.y() - rb.y())
	);
}

int
main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cerr	<< "Usage: " << argv[0] << " filename.chml" << endl;
		return 0;
	}

	Character::container in = read_chml(argv[1]);
	long double sum_size = 0;

	for (Character::const_iterator ci = in.begin();
	     ci != in.end();
	     ++ci)
		sum_size += char_size(*ci);

	cout << "Size: " << sum_size << "/" << in.size() << " = " << sum_size / in.size() << endl;

	return 0;
}
