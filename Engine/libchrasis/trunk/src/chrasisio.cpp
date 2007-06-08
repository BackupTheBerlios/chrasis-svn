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

#define DEFAULT_DB_FILE	"chr_data.db"

namespace chrasis
{

CHRASIS_API
character_possibility_t
recognize(Character const & chr, Database & db)
{
	character_possibility_t ret;

	Character nrm_chr(_normalize(chr));

	Query q(db);

	// find character
	std::string sql(
		"SELECT character_name, character_id FROM characters WHERE"
		"	stroke_count IN (" + boost::lexical_cast<std::string>(nrm_chr.stroke_count())
	);
	if (nrm_chr.get_name() != "")
		sql += ") AND character_name IN ('" + nrm_chr.get_name() + "'";
	for (Stroke::const_iterator si = nrm_chr.strokes_begin();
	     si != nrm_chr.strokes_end();
	     ++si)
	{
		sql += ") AND character_id IN ("
		       "	SELECT character_id FROM strokes WHERE "
		       "		pt_count IN (" +
		       				boost::lexical_cast<std::string>(si->point_count()) + ") AND "
		       "		sequence IN (" +
		       				boost::lexical_cast<std::string>(si - nrm_chr.strokes_begin()) + ")";
	}
	sql += ");";

	// retrieving points for possibility
	std::vector< std::pair< int, std::string > > id_name;
	std::string ids;
	q.get_result(sql);
	while (q.more_rows())
	{
		ResultRow r(q.fetch_row());
		id_name.push_back( std::make_pair(
			r.get<int>("character_id"),
			r.get<std::string>("character_name")
		) );
		ids += boost::lexical_cast<std::string>(r.get<int>("character_id")) + ", ";
	}
	q.free_result();
	ids = ids.substr(0, ids.size() - 2);
	sql =	"SELECT points.x, points.y FROM points, strokes, characters WHERE"
		"	points.stroke_id = strokes.stroke_id AND"
		"	strokes.character_id = characters.character_id AND"
		"	characters.character_id IN (" + ids + ") "
		"ORDER BY"
		"	strokes.character_id ASC, strokes.sequence ASC, points.sequence ASC;";

	// calculate possibility and return the result
	q.get_result(sql);
	for(std::vector< std::pair< int, std::string > >::iterator it = id_name.begin();
	    it != id_name.end();
	    ++it)
	{
		double c_possib = 0;
		for (Stroke::const_iterator si = nrm_chr.strokes_begin();
		     si != nrm_chr.strokes_end();
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
				double xd = r.get<int>("x") - pi->x(),
				       yd = r.get<int>("y") - pi->y();
				s_possib += std::sqrt(xd * xd + yd * yd);
			}
			c_possib += s_possib / si->point_count();
		}
		ret[ c_possib / nrm_chr.stroke_count() ] = *it;
	}
	q.free_result();

	return ret;
}

CHRASIS_API
bool
learn(Character const & chr, Database & db)
{
	character_possibility_t likely = recognize(chr, db);

	if (likely.size() == 0 ||
	    likely.begin()->first > 40000 * 0.15)	// the number (0.15) is a magic number.
							// i'm still testing for a good enough threshold.
		return _remember(_normalize(chr), db);
	else
		return _reflect(_normalize(chr), likely.begin()->second.first, db);
	
	return false;
}

CHRASIS_API
void
initialize_database(Database & db)
{
	Query q(db);
	q.execute("BEGIN TRANSACTION;");
	q.execute(
		"CREATE TABLE IF NOT EXISTS points (\n"
		"	pt_id		INTEGER	PRIMARY KEY	AUTOINCREMENT	NOT NULL,\n"
		"	stroke_id	INTEGER					NOT NULL,\n"
		"	sequence	INTEGER					NOT NULL,\n"
		"	x		INTEGER					NOT NULL,\n"
		"	y		INTEGER					NOT NULL\n"
		");"
	);
	q.execute(
		"CREATE TABLE IF NOT EXISTS strokes (\n"
		"	stroke_id	INTEGER	PRIMARY KEY	AUTOINCREMENT	NOT NULL,\n"
		"	character_id	INTEGER					NOT NULL,\n"
		"	pt_count	INTEGER					NOT NULL,\n"
		"	sequence	INTEGER					NOT NULL\n"
		");"
	);
	q.execute(
		"CREATE TRIGGER IF NOT EXISTS strokes_id_change AFTER UPDATE OF stroke_id ON strokes\n"
		"BEGIN\n"
		"	UPDATE points SET stroke_id = new.stroke_id WHERE\n"
		"		stroke_id = old.stroke_id;\n"
		"END;"
	);
	q.execute(
		"CREATE TABLE IF NOT EXISTS characters (\n"
		"	character_id	INTEGER PRIMARY KEY	AUTOINCREMENT	NOT NULL,\n"
		"	character_name	TEXT					NOT NULL,\n"
		"	stroke_count	INTEGER					NOT NULL,\n"
		"	sample_count	INTEGER	 DEFAULT 1			NOT NULL\n"
		");"
	);
	q.execute(
		"CREATE TRIGGER IF NOT EXISTS characters_id_change AFTER UPDATE OF character_id ON characters\n"
		"BEGIN\n"
		"	UPDATE strokes SET character_id = new.character_id WHERE\n"
		"		character_id = old.character_id;\n"
		"END;"
	);
	q.execute(
		"CREATE VIEW IF NOT EXISTS chr_pts AS\n"
		"	SELECT\n"
		"		characters.character_id	AS character_id,\n"
		"		strokes.stroke_id	AS stroke_id,\n"
		"		strokes.sequence	AS stroke_seq,\n"
		"		points.sequence		AS point_seq,\n"
		"		points.x 		AS x,\n"
		"		points.y 		AS y\n"
		"	FROM points, strokes, characters WHERE\n"
		"		points.stroke_id = strokes.stroke_id AND\n"
		"		strokes.character_id = characters.character_id\n"
		"	ORDER BY\n"
		"		strokes.character_id ASC,\n"
		"		strokes.sequence ASC,\n"
		"		points.sequence ASC;"
	);
	q.execute(
		"CREATE VIEW IF NOT EXISTS stats AS\n"
		"	SELECT\n"
		"		SUM(sample_count)		 AS total_sample_count,\n"
		"		COUNT(DISTINCT character_name)	 AS total_different_characters\n"
		"	FROM characters;"
	);
	q.execute(
		"CREATE INDEX IF NOT EXISTS stroke_seq ON strokes (sequence);"
	);
	q.execute(
		"CREATE INDEX IF NOT EXISTS stroke_pcnt ON strokes (pt_count);"
	);
	q.execute(
		"CREATE INDEX IF NOT EXISTS pt_seq ON points (sequence);"
	);
	q.execute(
		"CREATE INDEX IF NOT EXISTS chr_scnt ON characters (stroke_count);"
	);
	q.execute(
		"CREATE INDEX IF NOT EXISTS chr_name ON characters (character_name);"
	);
	q.execute("END TRANSACTION;");
}

} // namespace chrasis
