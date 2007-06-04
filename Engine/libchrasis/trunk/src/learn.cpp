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

bool
_remember(Character const & chr, Database & db)
{
	Query q(db);

	q.execute("BEGIN TRANSACTION");

	q.execute(
		"INSERT INTO characters (character_name, stroke_count) VALUES ("
		"	'" + chr.get_name() + "', "
		"	" + boost::lexical_cast<std::string>(chr.stroke_count()) + ");"
	);
	std::string chr_id = boost::lexical_cast<std::string>(q.last_insert_rowid());
	for (Stroke::const_iterator si = chr.strokes_begin();
	     si != chr.strokes_end();
	     ++si)
	{
		q.execute(
			"INSERT INTO strokes (sequence, character_id, pt_count) VALUES ("
			"	" + boost::lexical_cast<std::string>(si - chr.strokes_begin()) + ", "
			"	" + chr_id + ", "
			"	" + boost::lexical_cast<std::string>(si->point_count()) + ");"
		);
		std::string stroke_id = boost::lexical_cast<std::string>(q.last_insert_rowid());
		for (Point::const_iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			q.execute(
				"INSERT INTO points (sequence, stroke_id, x, y) VALUES ("
				"	" + boost::lexical_cast<std::string>(pi - si->points_begin()) + ", "
				"	" + stroke_id + ", "
				"	" + boost::lexical_cast<std::string>(pi->x()) + ", "
				"	" + boost::lexical_cast<std::string>(pi->y()) +	");"
			);
		}
	}

	q.execute("END TRANSACTION;");
	
	return true;
}

bool
_reflect(Character const & chr, int const chr_id, Database & db)
{
	Query q(db);
	std::string s_chr_id = boost::lexical_cast<std::string>(chr_id);

	q.get_result("SELECT sample_count FROM characters WHERE character_id = " + s_chr_id + ";");
	std::string sample_cnt;
	if (q.more_rows())
		sample_cnt = boost::lexical_cast<std::string>(q.fetch_row().get<long long>("sample_count") + 1);
	else
		return false;
	q.free_result();

	q.get_result("SELECT stroke_id FROM strokes WHERE character_id = " + s_chr_id + ";");
	std::vector< std::string > s_stroke_ids;
	if (q.more_rows())
		do
			s_stroke_ids.push_back(
				boost::lexical_cast<std::string>(q.fetch_row().get<long long>("stroke_id"))
			);
		while (q.more_rows());
	else
		return false;
	q.free_result();

	q.execute("BEGIN TRANSACTION;");

	for (Stroke::const_iterator si = chr.strokes_begin();
	     si != chr.strokes_end();
	     ++si)
	{
		for (Point::const_iterator pi = si->points_begin();
		     pi != si->points_end();
		     ++pi)
		{
			q.execute(
				"UPDATE points SET"
				"	x = x + ( " + boost::lexical_cast<std::string>(pi->x()) + " - x ) / " + sample_cnt + ", "
				"	y = y + ( " + boost::lexical_cast<std::string>(pi->y()) + " - y ) / " + sample_cnt + " "
				"WHERE"
				"	stroke_id = " + s_stroke_ids[si - chr.strokes_begin()] + " AND"
				"	sequence = " + boost::lexical_cast<std::string>(pi - si->points_begin()) + ";"
			);
		}
	}
	q.execute(
		"UPDATE characters SET"
		"	sample_count = sample_count + 1 "
		"WHERE"
		"	character_id = " + s_chr_id + ";"
	);

	q.execute("END TRANSACTION;");
	
	return true;
}


character_memories_t
_recall(std::string const & n, Database & db)
{
	character_memories_t ret;
	Query qc(db), qs(db), qp(db);

	qc.get_result("SELECT character_id FROM characters WHERE character_name = '" + n + "';");
	while (qc.more_rows())
	{
		ResultRow rc = qc.fetch_row();

		Character chr(n);
		qs.get_result(
			"SELECT stroke_id FROM strokes WHERE"
			"	character_id = " +
					boost::lexical_cast<std::string>(rc.get<long long>("character_id")) + " "
			"ORDER BY sequence ASC;"
		);
		while (qs.more_rows())
		{
			ResultRow rs = qs.fetch_row();

			Stroke s;
			qp.get_result(
				"SELECT x, y FROM points WHERE"
				"	stroke_id = " +
						boost::lexical_cast<std::string>(rs.get<long long>("stroke_id")) + " "
				"ORDER BY sequence ASC;"
			);
			while (qp.more_rows())
			{
				ResultRow rp = qp.fetch_row();
				s.add_point(rp.get<double>("x"), rp.get<double>("y"));
			}
			qp.free_result();

			chr.add_stroke(s);
		}
		qs.free_result();

		ret[rc.get<long long>("character_id")] = chr;
	}
	qc.free_result();

	return ret;
}

} // namespace chrasis
