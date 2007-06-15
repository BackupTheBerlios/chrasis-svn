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

Stroke
_normalize( const Stroke & orig_stroke, int const dist_threshold )
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

character_ids_t
_get_cids_by_traits(
	char_traits_t const & t,
	std::string const & n,
	Database & db)
{
	character_ids_t ret;

	Database::OPENDB *odb = db.grabdb();
	sqlite3_stmt *res = NULL;

	std::string sql(
		"SELECT c_id FROM c WHERE "
			"s_cnt IN (" + toString(t.size())
	);
	if (n != "")
		sql += ") AND c_n IN ('" + n + "'";
	for (char_traits_t::const_iterator i = t.begin();
	     i != t.end();
	     ++i)
		sql +=	") AND c_id IN ("
				"SELECT c_id FROM s WHERE "
					"p_cnt=" + toString(*i) + " AND "
					"seq=" + toString(i - t.begin());
	sql += ");";

	const char *s = NULL;
	int rc;

	rc = sqlite3_prepare(odb->db, sql.c_str(), sql.size(), &res, &s);
	if (rc != SQLITE_OK)
	{
		std::cerr << "Database: prepare query failed." << std::endl;
		std::cerr << "          sql = [" + sql + "]" << std::endl;
		return ret;
	}
	if (!res)
	{
		std::cerr << "get_result: query failed" << std::endl;
		return ret;
	}

	while (sqlite3_step(res) == SQLITE_ROW)
		ret.push_back( sqlite3_column_int(res, 0) );
	sqlite3_finalize(res);
	res = NULL;

	db.freedb(odb);

	return ret;
}

Character
_get_char_by_id(int const cid, Database & db)
{
	Character ret;

	Database::OPENDB *odb = db.grabdb();
	sqlite3_stmt *res = NULL;

	std::string sql(
		"SELECT s.seq,p.x,p.y FROM p,s,c WHERE "
			"p.s_id=s.s_id AND "
			"s.c_id=c.c_id AND "
			"c.c_id=" + toString(cid) + " "
		"ORDER BY "
			"s.c_id ASC, s.seq ASC, p.seq ASC;"
	);

	const char *s = NULL;
	int rc;

	rc = sqlite3_prepare(odb->db, sql.c_str(), sql.size(), &res, &s);
	if (rc != SQLITE_OK)
	{
		std::cerr << "Database: prepare query failed." << std::endl;
		std::cerr << "          sql = [" + sql + "]" << std::endl;
		return ret;
	}
	if (!res)
	{
		std::cerr << "get_result: query failed" << std::endl;
		return ret;
	}

	int oseq = -1, seq;
	while (sqlite3_step(res) == SQLITE_ROW)
	{
		if ( oseq != (seq = sqlite3_column_int(res, 0)) )
		{
			ret.new_stroke();
			oseq = seq;
		}

		ret.add_point(
			sqlite3_column_int(res, 1),
			sqlite3_column_int(res, 2)
		);
	}
	sqlite3_finalize(res);
	res = NULL;

	sql = "SELECT c_n FROM c WHERE c_id=" + toString(cid) + " LIMIT 1;";
	rc = sqlite3_prepare(odb->db, sql.c_str(), sql.size(), &res, &s);
	if (rc != SQLITE_OK)
	{
		std::cerr << "Database: prepare query failed." << std::endl;
		std::cerr << "          sql = [" + sql + "]" << std::endl;
		return ret;
	}
	if (!res)
	{
		std::cerr << "get_result: query failed" << std::endl;
		return ret;
	}
	if (sqlite3_step(res) == SQLITE_ROW)
	{
		const char *tmp_n = (const char *)sqlite3_column_text(res, 0);
		ret.set_name(tmp_n);
	}
	sqlite3_finalize(res);
	res = NULL;

	db.freedb(odb);

	return ret;
}

bool
_remember(Character const & chr, Database & db)
{
	Database::OPENDB *odb = db.grabdb();

	db.execute("BEGIN TRANSACTION;", odb);

	db.execute(
		"INSERT INTO c(c_n,s_cnt) VALUES ('" +
			chr.get_name() + "'," +
			toString(chr.stroke_count()) +
		");",
		odb
	);

	std::string chr_id = toString(sqlite3_last_insert_rowid(odb->db));
	for (Stroke::const_iterator si = chr.strokes_begin();
	     si != chr.strokes_end();
	     ++si)
	{
		db.execute(
			"INSERT INTO s(seq,c_id,p_cnt) VALUES (" +
				toString(si - chr.strokes_begin()) + "," +
				chr_id + "," +
				toString(si->point_count()) +
			");",
			odb
		);

		std::string stroke_id = toString(sqlite3_last_insert_rowid(odb->db));
		for (Point::const_iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			db.execute(
				"INSERT INTO p(seq,s_id,x,y) VALUES (" +
					toString(pi - si->points_begin()) + "," +
					stroke_id + "," +
					toString(pi->x()) + "," +
					toString(pi->y()) +
				");",
				odb
			);
		}
	}

	db.execute("END TRANSACTION;", odb);

	db.freedb(odb);

	return true;
}

bool
_reflect(Character const & chr, int const chr_id, Database & db)
{
	Database::OPENDB *odb = db.grabdb();
	sqlite3_stmt *res = NULL;

	const char *s = NULL;
	int rc;

	std::string s_chr_id = toString(chr_id);
	std::string sql;

	sql = "SELECT smp_cnt FROM c WHERE c_id IN (" + s_chr_id + ") LIMIT 1;";
	rc = sqlite3_prepare(odb->db, sql.c_str(), sql.size(), &res, &s);
	if (rc != SQLITE_OK)
	{
		std::cerr << "Database: prepare query failed." << std::endl;
		std::cerr << "          sql = [" + sql + "]" << std::endl;
		return false;
	}
	if (!res)
	{
		std::cerr << "Database: query failed." << std::endl;
		return false;
	}
	std::string smp_cnt;
	if (sqlite3_step(res) == SQLITE_ROW)
		smp_cnt = toString(sqlite3_column_int(res, 0) + 1);
	sqlite3_finalize(res);
	res = NULL;
	if (smp_cnt.size() == 0)
		return false;

	sql = "SELECT s_id FROM s WHERE c_id = " + s_chr_id + " LIMIT " +
		toString(chr.stroke_count()) + ";";
	rc = sqlite3_prepare(odb->db, sql.c_str(), sql.size(), &res, &s);
	if (rc != SQLITE_OK)
	{
		std::cerr << "Database: prepare query failed." << std::endl;
		std::cerr << "          sql = [" + sql + "]" << std::endl;
		return false;
	}
	if (!res)
	{
		std::cerr << "Database: query failed." << std::endl;
		return false;
	}
	std::vector< std::string > s_stroke_ids;
	s_stroke_ids.reserve(chr.stroke_count());
	while (sqlite3_step(res) == SQLITE_ROW)
		s_stroke_ids.push_back(
			toString(sqlite3_column_int(res, 0))
		);
	sqlite3_finalize(res);
	res = NULL;
	if (s_stroke_ids.size() != chr.stroke_count())
		return false;

	db.execute("BEGIN TRANSACTION;", odb);

	for (Stroke::const_iterator si = chr.strokes_begin();
	     si != chr.strokes_end();
	     ++si)
	{
		
		for (Point::const_iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			db.execute(
				"UPDATE p SET "
					"x = x + (" + 
						toString(pi->x()) + " - x"
					") / " + smp_cnt + ","
					"y = y + (" +
						toString(pi->y()) + " - y"
					") / " + smp_cnt + " "
				"WHERE "
					"s_id = " + s_stroke_ids[si - chr.strokes_begin()] + " AND "
					"seq = " + toString(pi - si->points_begin()) +
				";",
				odb
			);
		}
	}

	db.execute("END TRANSACTION;", odb);

	db.freedb(odb);

	return true;
}

} // namespace chrasis
