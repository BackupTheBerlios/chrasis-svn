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
#include "learner.h"
#include "database.h"
#include "recognizer.h"

void
basic_learner::prepare_database()
{
	Query q(db_);
	
	q.execute("BEGIN TRANSACTION;");
	q.execute(
		"CREATE TABLE IF NOT EXISTS points ("
		"	pt_id		INTEGER	PRIMARY KEY	AUTOINCREMENT	NOT NULL,"
		"	stroke_id	INTEGER					NOT NULL,"
		"	sequence	INTEGER					NOT NULL,"
		"	x		REAL					NOT NULL,"
		"	y		REAL					NOT NULL"
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

void
basic_learner::learn(Character const & chr)
{
	Recognizer &rec = Recognizer::Instance();
	Recognizer::character_possibility_t likely = rec.recognize(chr, db_);

	if (likely.size() == 0 ||
	    likely.begin()->first > 0.15) // the number (0.15) is a magic number.
	    				 // i'm still testing for a good enough threshold.
		remember(chr);
	else
		reflect(chr, likely.begin()->second.first);
}

basic_learner::CharacterMemories
basic_learner::recall(std::string const & n)
{
	CharacterMemories ret;
	Query qc(db_), qs(db_), qp(db_);

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

void
Learner::remember(Character const & chr)
{
	Query q(db_);

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
}

void
basic_learner::reflect(Character const & chr, int const chr_id)
{
	Query q(db_);
	std::string s_chr_id = boost::lexical_cast<std::string>(chr_id);

	q.get_result("SELECT sample_count FROM characters WHERE character_id = " + s_chr_id + ";");
	std::string sample_cnt;
	if (q.more_rows())
		sample_cnt = boost::lexical_cast<std::string>(q.fetch_row().get<long long>("sample_count") + 1);
	else
		return;
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
		return;
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
}
