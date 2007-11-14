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

#include <limits>

namespace chrasis
{

void
ItemPossibility::add_item(
	ItemPossibility::possibility_t p,
	int i,
	std::string n)
{
	items_.push_back( Item(p, i, n) );
}

void
ItemPossibility::sort(ItemPossibility::SORTING_POLICY const sp)
{
	switch (sp)
	{
	case SORTING_POSSIBILITY:
		std::sort(items_.begin(), items_.end(), PossibilityComparer());
		break;
	case SORTING_ID:
		std::sort(items_.begin(), items_.end(), IdComparer());
		break;
	case SORTING_NAME:
		std::sort(items_.begin(), items_.end(), NameComparer());
		break;
	}
}

ItemPossibility::iterator
ItemPossibility::begin()
{
	return items_.begin();
}

ItemPossibility::iterator
ItemPossibility::end()
{
	return items_.end();
}

ItemPossibility::const_iterator
ItemPossibility::begin() const
{
	return items_.begin();
}

ItemPossibility::const_iterator
ItemPossibility::end() const
{
	return items_.end();
}

size_t
ItemPossibility::empty() const
{
	return items_.empty();
}

size_t
ItemPossibility::size() const
{
	return items_.size();
}

ItemPossibility &
ItemPossibility::operator += (ItemPossibility const & rhs)
{
	std::copy(rhs.items_.begin(), rhs.items_.end(),
		std::back_inserter< container >(items_));
	return *this;
}

ItemPossibility::Item::Item(
	ItemPossibility::possibility_t p,
	int i,
	std::string n)
:
	possibility(p), id(i), name(n)
{
}

bool
ItemPossibility::PossibilityComparer::operator() (
	ItemPossibility::Item const & lhs,
	ItemPossibility::Item const & rhs)
{
	return lhs.possibility < rhs.possibility;
}

bool
ItemPossibility::IdComparer::operator() (
	ItemPossibility::Item const & lhs,
	ItemPossibility::Item const & rhs)
{
	return lhs.id < rhs.id;
}

bool
ItemPossibility::NameComparer::operator() (
	ItemPossibility::Item const & lhs,
	ItemPossibility::Item const & rhs)
{
	return lhs.name < rhs.name;
}

CHRASIS_INTERNAL
Stroke
_shift(Stroke const & nstr)
{
	if (nstr.point_count() < 2)
	{
		Stroke ret;
		ret.add_point(RESOLUTION/2, RESOLUTION/2);
		return ret;
	}

	// adjust the stroke to range [0...RESOLUTION)
	Point lt( *(nstr.points_begin()) ), rb( *(nstr.points_begin()) );
	for (Point::const_iterator pi = nstr.points_begin();
	     pi != nstr.points_end();
	     ++pi)
	{
		if (pi->x() < lt.x()) lt.x() = pi->x();
		if (pi->y() < lt.y()) lt.y() = pi->y();
		if (pi->x() > rb.x()) rb.x() = pi->x();
		if (pi->y() > rb.y()) rb.y() = pi->y();
	}
	Point::value_t const distance = std::max(
		abs(lt.x() - rb.x()),
		abs(lt.y() - rb.y())
	);
	lt.x() = (lt.x() + rb.x() - distance) / 2;
	lt.y() = (lt.y() + rb.y() - distance) / 2;

	Stroke ret(nstr);
	for (Point::iterator pi = ret.points_begin();
	     pi != ret.points_end();
	     ++pi)
	{
		*pi -= lt;
		pi->x() = static_cast<Point::value_t>(
			(static_cast<double>(pi->x()) / distance) * RESOLUTION);
		pi->y() = static_cast<Point::value_t>(
			(static_cast<double>(pi->y()) / distance) * RESOLUTION);
	}

	return ret;
}

CHRASIS_INTERNAL
id_container_t
_get_ids_by_prototype(Stroke const & pstr, SQLite::Command & cmd)
{
	std::string sql(
		"SELECT ss_id FROM stroke_shapes WHERE "
			"pt_cnt IN (" + toString(pstr.point_count()) + ");"
	);
	cmd.execute(sql);

	id_container_t ret;
	while (cmd.next())
		ret.push_back( cmd.column_int(0) );
	cmd.free_result();

	return ret;
}

CHRASIS_INTERNAL
Stroke
_get_stroke_by_id(int id, SQLite::Command & cmd)
{
	std::string sql(
		"SELECT x, y FROM points WHERE "
			"ss_id IN (" + toString(id) + ") "
		"ORDER BY seq ASC;"
	);
	cmd.execute(sql);

	Stroke ret;
	while (cmd.next())
		ret.add_point(cmd.column_int(0), cmd.column_int(1));
	cmd.free_result();

	return ret;
}

CHRASIS_INTERNAL
ItemPossibility
_recognize(Stroke const & nstr, SQLite::Command & cmd)
{
	Stroke snstr( _shift(nstr) );

	id_container_t likely_ids = _get_ids_by_prototype(snstr, cmd);

	ItemPossibility ret;

	for (id_container_t::const_iterator it = likely_ids.begin();
	     it != likely_ids.end();
	     ++it)
	{
		Stroke likely = _get_stroke_by_id(*it, cmd);
		ItemPossibility::possibility_t s_possib = 0.0;
		for (Point::iterator pi = snstr.points_begin(),
		                     lpi = likely.points_begin();
		     pi != snstr.points_end() &&
		     lpi != likely.points_end();
		     ++pi,
		     ++lpi)
			s_possib += std::sqrt(static_cast<ItemPossibility::possibility_t>(
				(pi->x() - lpi->x()) * (pi->x() - lpi->x()) +
				(pi->y() - lpi->y()) * (pi->y() - lpi->y())
			));
		ret.add_item(s_possib / snstr.point_count(), *it);
	}

	ret.sort(ItemPossibility::SORTING_POSSIBILITY);
	return ret;
}

CHRASIS_INTERNAL
id_container_t
_get_ids_by_prototype(Character const & pchr, SQLite::Command & cmd)
{
	std::string sql(
		"SELECT c_id FROM characters WHERE "
			"s_cnt IN (" + toString(pchr.stroke_count()) + ") "
	);
	if (pchr.get_name() != "")
		sql +=	"AND c_name IN (\"" + pchr.get_name() + "\") ";
	for (Stroke::const_iterator si = pchr.strokes_begin();
	     si != pchr.strokes_end();
	     ++si)
	{
		int n = si - pchr.strokes_begin();
		sql +=	"AND c_id IN ( "
				"SELECT c_id FROM strokes WHERE "
					"seq IN (" + toString(n) + ") "
					"AND ss_id IN ("
						"SELECT ss_id FROM stroke_shapes WHERE "
							"pt_cnt IN (" + toString(si->point_count()) + ")"
					")"
			") ";
	}
	sql +=	";";
	cmd.execute(sql);

	id_container_t ret;
	while (cmd.next())
		ret.push_back( cmd.column_int(0) );
	cmd.free_result();

	return ret;
}

CHRASIS_INTERNAL
Character
_get_character_by_id(int const & id, SQLite::Command & cmd)
{
	std::string sql(
		"SELECT c_name FROM characters WHERE "
			"c_id IN (" + toString(id) + ")"
		"LIMIT 1;"
	);
	cmd.execute(sql);
	Character ret;
	if (cmd.next())
		ret.set_name( cmd.column_text(0) );
	cmd.free_result();

	sql =	"SELECT ss_id, lt_x, lt_y, rb_x, rb_y FROM strokes WHERE "
			"c_id IN (" + toString(id) + ")"
		"ORDER BY seq ASC;";
	cmd.execute(sql);
	
	std::vector< std::pair< Point, Point > > lt_rb_v;
	id_container_t ssid_v;
	while (cmd.next())
	{
		ssid_v.push_back(cmd.column_int(0));
		lt_rb_v.push_back(
			std::make_pair(
				Point(cmd.column_int(1), cmd.column_int(2)),
				Point(cmd.column_int(3), cmd.column_int(4))
			)
		);
	}

	id_container_t::const_iterator it;
	std::vector< std::pair< Point, Point > >::const_iterator it2;
	for (it = ssid_v.begin(),
	     it2 = lt_rb_v.begin();
	     it != ssid_v.end() &&
	     it2 != lt_rb_v.end();
	     ++it,
	     ++it2)
	{
		Stroke tmp_str( _get_stroke_by_id(*it, cmd) );
		
		// shift and scale the stroke back to its original position
		Point::value_t lt_x( tmp_str.points_begin()->x() ),
		               lt_y( tmp_str.points_begin()->y() );
		for (Point::const_iterator pi = tmp_str.points_begin();
		     pi != tmp_str.points_end();
		     ++pi)
		{
			if (lt_x > pi->x()) lt_x = pi->x();
			if (lt_y > pi->y()) lt_y = pi->y();
		}
		Point::value_t distance = std::max(
			abs(it2->first.x() - it2->second.x()),
			abs(it2->first.y() - it2->second.y())
		);
		if (distance == 0)
			distance = std::numeric_limits< Point::value_t >::max();
		for (Point::iterator pi = tmp_str.points_begin();
		     pi != tmp_str.points_end();
		     ++pi)
		{
			pi->x() = (pi->x() - lt_x) * RESOLUTION / distance + it2->first.x();
			pi->y() = (pi->y() - lt_y) * RESOLUTION / distance + it2->first.y();
		}
		
		ret.add_stroke( tmp_str );
	}

	return ret;
}

CHRASIS_INTERNAL
ItemPossibility
_recognize(Character const & nchr, SQLite::Command & cmd)
{
	id_container_t likely_ids = _get_ids_by_prototype(nchr, cmd);

	ItemPossibility ret;

	for (id_container_t::const_iterator it = likely_ids.begin();
	     it != likely_ids.end();
	     ++it)
	{
		Character likely = _get_character_by_id(*it, cmd);

		ItemPossibility::possibility_t c_possib = 0.0;

		for (Stroke::const_iterator
			si = nchr.strokes_begin(),
			lsi = likely.strokes_begin();
		     si != nchr.strokes_end() &&
		     lsi != likely.strokes_begin();
		     ++si,
		     ++lsi)
		{
			ItemPossibility::possibility_t s_possib = 0.0;
			for (Point::const_iterator
				pi = si->points_begin(),
				lpi = lsi->points_begin();
			     pi != si->points_end() &&
			     lpi != lsi->points_end();
			     ++pi,
			     ++lpi)
			{
				s_possib += std::sqrt(
					(pi->x() - lpi->x()) * (pi->x() - lpi->x()) +
					(pi->y() - lpi->y()) * (pi->y() - lpi->y())
				);
			}
			c_possib += s_possib / si->point_count();
		}
		
		ret.add_item(
			c_possib / nchr.stroke_count(),
			*it,
			likely.get_name()
		);
	}

	ret.sort(ItemPossibility::SORTING_POSSIBILITY);
	return ret;
}

CHRASIS_INTERNAL
int
_learn(Stroke const & nstr, SQLite::Command & cmd)
{
	ItemPossibility likely = _recognize(nstr, cmd);

	if (likely.empty() ||
	    likely.begin()->possibility > RESOLUTION * LEARNING_THRESHOLD)
		return _remember(nstr, cmd);
	return _reflect(nstr, likely.begin()->id, cmd);
}

CHRASIS_INTERNAL
int
_remember(Stroke const & nstr, SQLite::Command & cmd)
{
	Stroke const snstr( _shift(nstr) );

	std::string sql(
		"INSERT INTO stroke_shapes (pt_cnt) "
		"VALUES (" + toString(snstr.point_count()) + ");"
	);
	cmd.execute_nonquery(sql);
	
	int ssid = cmd.last_rowid();

	for (Point::const_iterator pi = snstr.points_begin();
	     pi != snstr.points_end();
	     ++pi)
	{
		sql =	"INSERT INTO points (ss_id, seq, x, y) "
			"VALUES (" +
				toString(ssid) + ", " +
				toString(pi - snstr.points_begin()) + ", " +
				toString(pi->x()) + ", " +
				toString(pi->y()) +
			")";
		cmd.execute_nonquery(sql);
	}
	
	return ssid;
}

CHRASIS_INTERNAL
int
_reflect(Stroke const & nstr, int ssid, SQLite::Command & cmd)
{
	// update and get sample count
	std::string sql(
		"UPDATE stroke_shapes SET "
			"smp_cnt = smp_cnt + 1 "
		"WHERE "
			"ss_id IN (" + toString(ssid) + ")"
		";"
	);
	cmd.execute_nonquery(sql);
	
	sql =	"SELECT smp_cnt FROM stroke_shapes WHERE "
			"ss_id IN (" + toString(ssid) + ") "
		"LIMIT 1;";
	cmd.execute(sql);
	int smp_cnt(0);
	if (cmd.next())
		smp_cnt = cmd.column_int(0);
	cmd.free_result();

	// update each point
	Stroke snstr( _shift(nstr) );
	for (Point::iterator pi = snstr.points_begin();
	     pi != snstr.points_end();
	     ++pi)
	{
		sql = 	"UPDATE points SET "
				"x = x + (" + toString(pi->x()) + " - x) / " + toString(smp_cnt) + ","
				"y = y + (" + toString(pi->y()) + " - y) / " + toString(smp_cnt) + " "
			"WHERE "
				"ss_id = " + toString(ssid) + " AND "
				"seq = " + toString(pi - snstr.points_begin()) +
			";";
		cmd.execute_nonquery(sql);
	}

	// adjust the range from (0, 0) to (RES, RES)
	sql =	"SELECT MIN(x), MIN(y), MAX(x), MAX(y) FROM points WHERE "
			"ss_id IN (" + toString(ssid) + ");";
	cmd.execute(sql);
	int lt_x(0), lt_y(0), rb_x(0), rb_y(0);
	if (cmd.next())
	{
		lt_x = cmd.column_int(0);
		lt_y = cmd.column_int(1);
		rb_x = cmd.column_int(2);
		rb_y = cmd.column_int(3);
	}
	cmd.free_result();

	Point::value_t const distance =
		std::max(abs(lt_x - rb_x), abs(lt_y - rb_y));
	double const ratio = static_cast<double>(RESOLUTION) / distance;
	lt_x = (lt_x + rb_x - distance) / 2;
	lt_y = (lt_y + rb_y - distance) / 2;

	sql =	"UPDATE points SET "
			"x = (x - " + toString(lt_x) + ") * " + toString(ratio) + ","
			"y = (y - " + toString(lt_y) + ") * " + toString(ratio) + " "
		"WHERE "
			"ss_id IN (" + toString(ssid) + ");";
	cmd.execute_nonquery(sql);

	return ssid;
}

CHRASIS_INTERNAL
int
_learn(Character const & nchr, SQLite::Command & cmd)
{
	if (nchr.get_name() == "")
		return 0;

	ItemPossibility likely = _recognize(nchr, cmd);

	if (likely.empty() ||
	    likely.begin()->possibility > RESOLUTION * LEARNING_THRESHOLD)
		return _remember(nchr, cmd);
	return _reflect(nchr, likely.begin()->id, cmd);
}

CHRASIS_INTERNAL
int
_remember(Character const & nchr, SQLite::Command & cmd)
{
	std::string sql(
		"INSERT INTO characters(c_name,s_cnt) VALUES ("
			"'" + nchr.get_name() + "'," +
			toString(nchr.stroke_count()) +
		");"
	);
	cmd.execute_nonquery(sql);
	int c_id = cmd.last_rowid();

	for (Stroke::const_iterator si = nchr.strokes_begin();
	     si != nchr.strokes_end();
	     ++si)
	{
		// find boundary
		int	lt_x( si->points_begin()->x() ),
			lt_y( si->points_begin()->y() ),
			rb_x( si->points_begin()->x() ),
			rb_y( si->points_begin()->y() );
		for (Point::const_iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			if (lt_x > pi->x()) lt_x = pi->x();
			if (lt_y > pi->y()) lt_y = pi->y();
			if (rb_x < pi->x()) rb_x = pi->x();
			if (rb_y < pi->y()) rb_y = pi->y();
		}

		sql =	"INSERT INTO strokes(c_id,seq,ss_id,lt_x,lt_y,rb_x,rb_y) VALUES (" +
				toString(c_id) + "," +
				toString(si - nchr.strokes_begin()) + "," +
				toString( _learn(*si, cmd) ) + "," +
				toString( lt_x ) + "," + toString( lt_y ) + "," +
				toString( rb_x ) + "," + toString( rb_y ) +
			");";
		cmd.execute_nonquery(sql);
	}

	return c_id;
}

CHRASIS_INTERNAL
int
_reflect(Character const & nchr, int const cid, SQLite::Command & cmd)
{
	std::string sql(
		"UPDATE characters SET "
			"smp_cnt = smp_cnt + 1 "
		"WHERE "
			"c_id IN (" + toString(cid) + ")"
		";"
	);
	cmd.execute_nonquery(sql);

	sql =	"SELECT smp_cnt FROM characters WHERE "
			"c_id IN (" + toString(cid) + ") "
		"LIMIT 1;";
	cmd.execute(sql);
	int smp_cnt(0);
	if (cmd.next())
		smp_cnt = cmd.column_int(0);
	cmd.free_result();

	for (Character::Stroke::const_iterator si = nchr.strokes_begin();
	     si != nchr.strokes_end();
	     ++si)
	{
		// find boundary
		int	lt_x( si->points_begin()->x() ),
			lt_y( si->points_begin()->y() ),
			rb_x( si->points_begin()->x() ),
			rb_y( si->points_begin()->y() );
		for (Point::const_iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			if (lt_x > pi->x()) lt_x = pi->x();
			if (lt_y > pi->y()) lt_y = pi->y();
			if (rb_x < pi->x()) rb_x = pi->x();
			if (rb_y < pi->y()) rb_y = pi->y();
		}
		
		sql = 	"UPDATE points SET "
				"lt_x = lt_x + (" + toString(lt_x) + " - lt_x) / " + toString(smp_cnt) + ","
				"lt_y = lt_y + (" + toString(lt_y) + " - lt_y) / " + toString(smp_cnt) + ","
				"rb_x = rb_x + (" + toString(rb_x) + " - rb_x) / " + toString(smp_cnt) + ","
				"rb_y = rb_y + (" + toString(rb_y) + " - rb_y) / " + toString(smp_cnt) + " "
			"WHERE "
				"c_id = " + toString(cid) + " AND "
				"seq = " + toString(si - nchr.strokes_begin()) +
			";";
		cmd.execute_nonquery(sql);
	}

	return cid;
}

} // namespace chrasis
