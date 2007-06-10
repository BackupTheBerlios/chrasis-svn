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
character_possibility_t
recognize(Character const & chr, Database & db)
{
	character_possibility_t ret;

	Character nrm_chr(_normalize(chr));

	Query q(db);

	// find character
	std::string sql(
		"SELECT c_n,c_id FROM c WHERE "
			"s_cnt IN (" + boost::lexical_cast<std::string>(nrm_chr.stroke_count())
	);
	if (nrm_chr.get_name() != "")
		sql += ") AND c_n IN ('" + nrm_chr.get_name() + "'";
	for (Stroke::const_iterator si = nrm_chr.strokes_begin();
	     si != nrm_chr.strokes_end();
	     ++si)
	{
		sql += ") AND c_id IN ("
		       "	SELECT c_id FROM s WHERE "
		       "		p_cnt IN (" +
		       				boost::lexical_cast<std::string>(si->point_count()) + ") AND "
		       "		seq IN (" +
		       				boost::lexical_cast<std::string>(si - nrm_chr.strokes_begin()) + ")";
	}
	sql += ");";

	// retrieving points for possibility
	std::vector< std::pair< int, std::string > > id_name;
	q.get_result(sql);
	sql = "";
	while (q.more_rows())
	{
		ResultRow r(q.fetch_row());
		id_name.push_back( std::make_pair(
			r.get<int>("c_id"),
			r.get<std::string>("c_n")
		) );
		sql += boost::lexical_cast<std::string>(r.get<int>("c_id")) + ",";
	}
	q.free_result();
	sql = sql.substr(0, sql.size() - 1) +
	sql =	"SELECT p.x,p.y FROM p,s,c WHERE "
			"p.s_id=s.s_id AND "
			"s.c_id = c.c_id AND "
			"c.c_id IN (" + sql + ") "
		"ORDER BY "
			"s.c_id ASC, s.seq ASC, p.seq ASC;";

	// calculate possibility and return the result
	q.get_result(sql);
	for(std::vector< std::pair< int, std::string > >::iterator it = id_name.begin();
	    it != id_name.end();
	    ++it)
	{
		int c_possib = 0;
		for (Stroke::const_iterator si = nrm_chr.strokes_begin();
		     si != nrm_chr.strokes_end();
		     ++si)
		{
			int s_possib = 0;
			for (Point::const_iterator pi = si->points_begin();
			     pi != si->points_end();
			     ++pi)
			{
				if (!q.more_rows())
					return ret;
				ResultRow r(q.fetch_row());
				double xd = r.get<int>("x") - pi->x(),
				       yd = r.get<int>("y") - pi->y();
				s_possib += static_cast<int>(std::sqrt(xd * xd + yd * yd));
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

	if ( likely.size() == 0 ||
	     likely.begin()->first > static_cast<int>(RESOLUTION * LEARNING_THRESHOLD) )
		return _remember(_normalize(chr), db);
	else
		return _reflect(_normalize(chr), likely.begin()->second.first, db);
	
	return false;
}

} // namespace chrasis
