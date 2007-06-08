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
	std::vector< std::pair< long long, std::string > > id_name;
	std::string ids;
	q.get_result(sql);
	while (q.more_rows())
	{
		ResultRow r(q.fetch_row());
		id_name.push_back( std::make_pair(
			r.get<long long>("character_id"),
			r.get<std::string>("character_name")
		) );
		ids += boost::lexical_cast<std::string>(r.get<long long>("character_id")) + ", ";
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
	for(std::vector< std::pair< long long, std::string > >::iterator it = id_name.begin();
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
				double xd = r.get<double>("x") - pi->x(),
				       yd = r.get<double>("y") - pi->y();
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
	    likely.begin()->first > 0.15) // the number (0.15) is a magic number.
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
		"CREATE TABLE IF NOT EXISTS points ("
		"	pt_id		INTEGER	PRIMARY KEY	AUTOINCREMENT	NOT NULL,"
		"	stroke_id	INTEGER					NOT NULL,"
		"	sequence	INTEGER					NOT NULL,"
		"	x		INTEGER					NOT NULL,"
		"	y		INTEGER					NOT NULL"
		");"
	);
	q.execute(
		"CREATE TABLE IF NOT EXISTS strokes ("
		"	stroke_id	INTEGER	PRIMARY KEY	AUTOINCREMENT	NOT NULL,"
		"	character_id	INTEGER					NOT NULL,"
		"	pt_count	INTEGER					NOT NULL,"
		"	sequence	INTEGER					NOT NULL"
		");"
	);
	q.execute(
		"CREATE TRIGGER IF NOT EXISTS strokes_id_change AFTER UPDATE OF stroke_id ON strokes "
		"BEGIN"
		"	UPDATE points SET stroke_id = new.stroke_id WHERE"
		"		stroke_id = old.stroke_id;"
		"END;"
	);
	q.execute(
		"CREATE TABLE IF NOT EXISTS characters ("
		"	character_id	INTEGER PRIMARY KEY	AUTOINCREMENT	NOT NULL,"
		"	character_name	TEXT					NOT NULL,"
		"	stroke_count	INTEGER					NOT NULL,"
		"	sample_count	INTEGER DEFAULT 1			NOT NULL"
		");"
	);
	q.execute(
		"CREATE TRIGGER IF NOT EXISTS characters_id_change AFTER UPDATE OF character_id ON characters "
		"BEGIN"
		"	UPDATE strokes SET character_id = new.character_id WHERE"
		"		character_id = old.character_id;"
		"END;"
	);
	q.execute(
		"CREATE VIEW IF NOT EXISTS chr_pts AS"
		"	SELECT"
		"		characters.character_id	AS character_id,"
		"		strokes.stroke_id	AS stroke_id,"
		"		strokes.sequence	AS stroke_seq,"
		"		points.sequence		AS point_seq,"
		"		points.x 		AS x,"
		"		points.y 		AS y"
		"	FROM points, strokes, characters WHERE"
		"		points.stroke_id = strokes.stroke_id AND"
		"		strokes.character_id = characters.character_id"
		"	ORDER BY"
		"		strokes.character_id ASC,"
		"		strokes.sequence ASC,"
		"		points.sequence ASC;"
	);
	q.execute(
		"CREATE VIEW IF NOT EXISTS stats AS"
		"	SELECT"
		"		SUM(sample_count)		 AS total_sample_count,"
		"		COUNT(DISTINCT character_name)	 AS total_different_characters"
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
