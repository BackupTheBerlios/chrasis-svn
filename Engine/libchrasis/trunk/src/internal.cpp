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

static inline
std::string
_idiotic_hash(Character const & chr)
{
	std::string ret("*");
	for (Stroke::const_iterator si = chr.strokes_begin();
	     si != chr.strokes_end();
	     ++si)
	{
		ret += toString(si->point_count());
		ret += "*";
	}
	return ret;
}

Stroke
_shift(Stroke const & nstr)
{
	if (nstr.point_count() < 2)
	{
		Stroke ret;
		ret.add_point(RESOLUTION/2, RESOLUTION/2);
		return ret;
	}

	/* adjust the stroke to range [0...RESOLUTION) */
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

id_container_t
_get_ids_by_prototype(Stroke const & pstr, Database::OPENDB & odb)
{
	std::string sql(
		"SELECT ss_id FROM stroke_shapes WHERE "
			"pt_cnt IN (" + toString(pstr.point_count()) + ");"
	);

	sqlite3_stmt *res = NULL;

	sqlite3_prepare(odb.db, sql.c_str(), sql.size(), &res, NULL);

	id_container_t ret;

	while (sqlite3_step(res) == SQLITE_ROW)
		ret.push_back( sqlite3_column_int(res, 0) );

	sqlite3_finalize(res);

	return ret;
}

Stroke
_get_stroke_by_id(int id, Database::OPENDB & odb)
{
	std::string sql(
		"SELECT x, y FROM points WHERE "
			"ss_id IN (" + toString(id) + ") "
		"ORDER BY seq ASC;"
	);

	sqlite3_stmt *res = NULL;

	sqlite3_prepare(odb.db, sql.c_str(), sql.size(), &res, NULL);

	Stroke ret;

	while (sqlite3_step(res) == SQLITE_ROW)
		ret.add_point(
			sqlite3_column_int(res, 0),
			sqlite3_column_int(res, 1)
		);

	sqlite3_finalize(res);

	return ret;
}

stroke_possibility_t
_recognize(Stroke const & nstr, Database::OPENDB & odb)
{
	Stroke snstr( _shift(nstr) );

	id_container_t likely_ids = _get_ids_by_prototype(snstr, odb);

	stroke_possibility_t ret;

	for (id_container_t::const_iterator it = likely_ids.begin();
	     it != likely_ids.end();
	     ++it)
	{
		Stroke likely = _get_stroke_by_id(*it, odb);
		possibility_t s_possib = 0.0;
		for (Point::iterator pi = snstr.points_begin(),
		                     lpi = likely.points_begin();
		     pi != snstr.points_end() &&
		     lpi != likely.points_end();
		     ++pi,
		     ++lpi)
			s_possib += std::sqrt(static_cast<possibility_t>(
				(pi->x() - lpi->x()) * (pi->x() - lpi->x()) +
				(pi->y() - lpi->y()) * (pi->y() - lpi->y())
			));
		ret.insert(std::make_pair(*it, s_possib / snstr.point_count()));
	}

	return ret;
}

id_container_t
_get_ids_by_prototype(
	Character const & pchr,
	std::vector< stroke_possibility_t > sptv,
	Database::OPENDB & odb)
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
					"seq IN (" + toString(n) + ") AND "
					"ss_id IN (";
		for (stroke_possibility_t::iterator i = sptv[n].begin();
		     i != sptv[n].end();
		     ++i)
		{
			sql +=			toString(i->first);
			sql +=			",";
		}
		sql = sql.substr(0, sql.size() - 1);
		sql +=			")"
			") ";
	}
	sql +=	";";

	sqlite3_stmt *res = NULL;

	sqlite3_prepare(odb.db, sql.c_str(), sql.size(), &res, NULL);

	id_container_t ret;

	while (sqlite3_step(res) == SQLITE_ROW)
		ret.push_back( sqlite3_column_int(res, 0) );

	sqlite3_finalize(res);

	return ret;
}

CharacterImpl
_get_character_by_id(int const & id, Database::OPENDB & odb)
{
	std::string sql(
		"SELECT c_name FROM characters WHERE "
			"c_id IN (" + toString(id) + ")"
		"LIMIT 1;"
	);

	sqlite3_stmt *res = NULL;

	sqlite3_prepare(odb.db, sql.c_str(), sql.size(), &res, NULL);

	sqlite3_step(res);
	const char *tmp_n = (const char *)sqlite3_column_text(res, 0);
	CharacterImpl ret(tmp_n);

	sqlite3_finalize(res);

	sql =	"SELECT ss_id, lt_x, lt_y, rb_x, rb_y FROM strokes WHERE "
			"c_id IN (" + toString(id) + ")"
		"ORDER BY seq ASC;";
	sqlite3_prepare(odb.db, sql.c_str(), sql.size(), &res, NULL);

	while (sqlite3_step(res) == SQLITE_ROW)
		ret.add_stroke(
			sqlite3_column_int(res, 0),
			sqlite3_column_int(res, 1),
			sqlite3_column_int(res, 2),
			sqlite3_column_int(res, 3),
			sqlite3_column_int(res, 4)
		);

	sqlite3_finalize(res);

	return ret;
}

character_possibility_t
_recognize(Character const & nchr, Database::OPENDB & odb)
{
	std::vector< stroke_possibility_t > sptv;
	sptv.reserve(nchr.stroke_count());
	CharacterImpl chr_impl(nchr.get_name());

	for (Stroke::const_iterator si = nchr.strokes_begin();
	     si != nchr.strokes_end();
	     ++si)
	{
		Point lt(*si->points_begin()), rb(*si->points_begin());
		for (Point::const_iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			if (pi->x() < lt.x()) lt.x() = pi->x();
			if (pi->y() < lt.y()) lt.y() = pi->y();
			if (pi->x() > rb.x()) rb.x() = pi->x();
			if (pi->y() > rb.y()) rb.y() = pi->y();
		}
		chr_impl.add_stroke(0, lt, rb);
		sptv.push_back( _recognize(*si, odb) );
	}

	id_container_t likely_ids = _get_ids_by_prototype(nchr, sptv, odb);

	character_possibility_t ret;

	for (id_container_t::const_iterator it = likely_ids.begin();
	     it != likely_ids.end();
	     ++it)
	{
		CharacterImpl likely = _get_character_by_id(*it, odb);

		possibility_t c_possib = 0.0;

		CharacterImpl::StrokeImpl::iterator sii, lsii;
		std::vector< stroke_possibility_t >::iterator sptvi;
		for (sii = chr_impl.strokes_begin(),
		     lsii = likely.strokes_begin(),
		     sptvi = sptv.begin();
		     sii != chr_impl.strokes_end(),
		     lsii != likely.strokes_end(),
		     sptvi != sptv.end();
		     ++sii,
		     ++lsii,
		     ++sptvi)
		{
			Point::value_t
				slt_x = sii->get_lefttop().x(), lslt_x = lsii->get_lefttop().x(),
				slt_y = sii->get_lefttop().y(), lslt_y = lsii->get_lefttop().y(),
				srb_x = sii->get_rightbottom().x(), lsrb_x = lsii->get_rightbottom().x(),
				srb_y = sii->get_rightbottom().y(), lsrb_y = lsii->get_rightbottom().y();
			possibility_t possib =
				(
					std::sqrt(
						(slt_x - lslt_x) * (slt_x - lslt_x) +
						(slt_y - lslt_y) * (slt_y - lslt_y) ) +
					std::sqrt(
						(srb_x - lsrb_x) * (srb_x - lsrb_x) +
						(srb_y - lsrb_y) * (srb_y - lsrb_y) )
				) / 2.0;
			possib *= (*sptvi)[lsii->get_stroke_shape_id()];
		}
		ret.insert(std::make_pair(
			c_possib / likely.stroke_count(),
			std::make_pair(*it, likely.get_name())
		));
	}

	return ret;
}

int
_learn(Stroke const & nstr, Database::OPENDB & odb)
{
	stroke_possibility_t likely = _recognize(nstr, odb);

	if (likely.empty())
		return _remember(nstr, odb);

	int ss_id = likely.begin()->first;
	possibility_t possib = likely.begin()->second;
	for (stroke_possibility_t::const_iterator li = likely.begin();
	     li != likely.end();
	     ++li)
	{
		if (li->second < possib)
		{
			possib = li->second;
			ss_id = li->first;
		}
	}

	if (possib > RESOLUTION * LEARNING_THRESHOLD)
		return _remember(nstr, odb);
	return _reflect(nstr, ss_id, odb);
}

int
_remember(Stroke const & nstr, Database::OPENDB & odb)
{
	std::string sql(
		"INSERT INTO stroke_shapes(pt_cnt) VALUES (" +
			toString(nstr.point_count()) +
		");"
	);
	sqlite3_exec(odb.db, sql.c_str(), NULL, NULL, NULL);

	int sshape_id = sqlite3_last_insert_rowid(odb.db);

	sql = "";
	Stroke snstr( _shift(nstr) );
	for (Point::iterator pi = snstr.points_begin();
	     pi != snstr.points_end();
	     ++pi)
	{
		sql += 	"INSERT INTO points(seq,ss_id,x,y) VALUES (" +
				toString(pi - snstr.points_begin()) + "," +
				toString(sshape_id) + "," +
				toString(pi->x()) + "," +
				toString(pi->y()) +
			");";
	}
	sqlite3_exec(odb.db, sql.c_str(), NULL, NULL, NULL);

	return sshape_id;
}

int
_reflect(Stroke const & nstr, int id, Database::OPENDB & odb)
{
	std::string sql(
		"UPDATE stroke_shapes SET "
			"smp_cnt = smp_cnt + 1 "
		"WHERE "
			"ss_id IN (" + toString(id) + ")"
		";"
	);

	sqlite3_stmt *res = NULL;
	
	sqlite3_exec(odb.db, sql.c_str(), NULL, NULL, NULL);
	
	sql =	"SELECT smp_cnt FROM stroke_shapes WHERE "
			"ss_id IN (" + toString(id) + ") "
		"LIMIT 1;";
	sqlite3_prepare(odb.db, sql.c_str(), sql.size(), &res, NULL);

	int smp_cnt(0);
	if (sqlite3_step(res) == SQLITE_ROW)
		smp_cnt = sqlite3_column_int(res, 0);
	sqlite3_finalize(res);

	sql = "";
	Stroke snstr( _shift(nstr) );
	for (Point::iterator pi = snstr.points_begin();
	     pi != snstr.points_end();
	     ++pi)
	{
		sql += 	"UPDATE points SET "
				"x = x + (" + toString(pi->x()) + " - x) / " + toString(smp_cnt) + ","
				"y = y + (" + toString(pi->y()) + " - y) / " + toString(smp_cnt) + " "
			"WHERE "
				"ss_id = " + toString(id) + " AND "
				"seq = " + toString(pi - snstr.points_begin()) +
			";";
	}
	sqlite3_exec(odb.db, sql.c_str(), NULL, NULL, NULL);

	sql =	"SELECT MIN(x), MIN(y), MAX(x), MAX(y) FROM points WHERE "
			"ss_id IN (" + toString(id) + ");";
	sqlite3_prepare(odb.db, sql.c_str(), sql.size(), &res, NULL);

	int lt_x(0), lt_y(0), rb_x(0), rb_y(0);
	if (sqlite3_step(res) == SQLITE_ROW)
	{
		lt_x = sqlite3_column_int(res, 0);
		lt_y = sqlite3_column_int(res, 1);
		rb_x = sqlite3_column_int(res, 2);
		rb_y = sqlite3_column_int(res, 3);
	}
	sqlite3_finalize(res);

	Point::value_t const distance =
		std::max(abs(lt_x - rb_x), abs(lt_y - rb_y));
	double const ratio = static_cast<double>(RESOLUTION) / distance;
	lt_x = (lt_x + rb_x - distance) / 2;
	lt_y = (lt_y + rb_y - distance) / 2;

	sql =	"UPDATE points SET "
			"x = (x - " + toString(lt_x) + ") * " + toString(ratio) + ","
			"y = (y - " + toString(lt_y) + ") * " + toString(ratio) + " "
		"WHERE "
			"ssid IN (" + toString(id) + ");";
	sqlite3_exec(odb.db, sql.c_str(), NULL, NULL, NULL);

	return id;
}

int
_learn(Character const & nchr, Database::OPENDB & odb)
{
	id_container_t shape_ids;
	shape_ids.reserve(nchr.stroke_count());

	for (Stroke::const_iterator si = nchr.strokes_begin();
	     si != nchr.strokes_end();
	     ++si)
		shape_ids.push_back( _learn(*si, odb) );

	CharacterImpl chr_impl(nchr.get_name());
	for (Stroke::const_iterator si = nchr.strokes_begin();
	     si != nchr.strokes_end();
	     ++si)
	{
		Point lt(*si->points_begin()), rb(*si->points_begin());
		for (Point::const_iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			if (pi->x() < lt.x()) lt.x() = pi->x();
			if (pi->y() < lt.y()) lt.y() = pi->y();
			if (pi->x() > rb.x()) rb.x() = pi->x();
			if (pi->y() > rb.y()) rb.y() = pi->y();
		}
		chr_impl.add_stroke(0, lt, rb);
	}

	if (nchr.get_name() == "")
		return 0;

	character_possibility_t likely = _recognize(nchr, odb);

	if (likely.empty() ||
	    likely.begin()->first > RESOLUTION * LEARNING_THRESHOLD)
		return _remember(chr_impl, shape_ids, odb);
	return _reflect(chr_impl, likely.begin()->second.first, odb);
}

int
_remember(CharacterImpl const & chr_impl, id_container_t const & shape_ids, Database::OPENDB & odb)
{
	std::string sql(
		"INSERT INTO characters(c_name,s_cnt) VALUES ("
			"'" + chr_impl.get_name() + "'," +
			toString(chr_impl.stroke_count()) +
		");"
	);
	sqlite3_exec(odb.db, sql.c_str(), NULL, NULL, NULL);

	int c_id = sqlite3_last_insert_rowid(odb.db);

	sql = "";
	CharacterImpl::StrokeImpl::const_iterator si;
	id_container_t::const_iterator ii;
	for (si = chr_impl.strokes_begin(),
	     ii = shape_ids.begin();
	     si != chr_impl.strokes_end(),
	     ii != shape_ids.end();
	     ++si,
	     ++ii)
	{
		sql += 	"INSERT INTO strokes(c_id,seq,ss_id,lt_x,lt_y,rb_x,rb_y) VALUES (" +
				toString(c_id) + "," +
				toString(si - chr_impl.strokes_begin()) + "," +
				toString(*ii) + "," +
				toString(si->get_lefttop().x()) + "," +
				toString(si->get_lefttop().y()) + "," +
				toString(si->get_rightbottom().x()) + "," +
				toString(si->get_rightbottom().y()) +
			");";
	}
	sqlite3_exec(odb.db, sql.c_str(), NULL, NULL, NULL);

	return c_id;
}

int
_reflect(CharacterImpl const & chr_impl, int const id, Database::OPENDB & odb)
{
	std::string sql(
		"UPDATE characters SET "
			"smp_cnt = smp_cnt + 1 "
		"WHERE "
			"c_id IN (" + toString(id) + ")"
		";"
	);

	sqlite3_stmt *res = NULL;
	
	sqlite3_exec(odb.db, sql.c_str(), NULL, NULL, NULL);

	sql =	"SELECT smp_cnt FROM characters WHERE "
			"c_id IN (" + toString(id) + ") "
		"LIMIT 1;";
	sqlite3_prepare(odb.db, sql.c_str(), sql.size(), &res, NULL);

	int smp_cnt(0);
	if (sqlite3_step(res) == SQLITE_ROW)
		smp_cnt = sqlite3_column_int(res, 0);
	sqlite3_finalize(res);
	
	sql = "";
	for (CharacterImpl::StrokeImpl::const_iterator si = chr_impl.strokes_begin();
	     si != chr_impl.strokes_end();
	     ++si)
	{
		sql = 	"UPDATE points SET "
				"lt_x = lt_x + (" + toString(si->get_lefttop().x()) + " - lt_x) / " + toString(smp_cnt) + ","
				"lt_y = lt_y + (" + toString(si->get_lefttop().y()) + " - lt_y) / " + toString(smp_cnt) + ","
				"rb_x = rb_x + (" + toString(si->get_rightbottom().x()) + " - rb_x) / " + toString(smp_cnt) + ","
				"rb_y = rb_y + (" + toString(si->get_rightbottom().y()) + " - rb_y) / " + toString(smp_cnt) + " "
			"WHERE "
				"c_id = " + toString(id) + " AND "
				"seq = " + toString(si - chr_impl.strokes_begin()) +
			";";
	}
	sqlite3_exec(odb.db, sql.c_str(), NULL, NULL, NULL);

	return id;
}

} // namespace chrasis
