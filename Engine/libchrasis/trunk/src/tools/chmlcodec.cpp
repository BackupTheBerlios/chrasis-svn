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
 * $Id: chmlcodec.cpp 18 2006-09-19 21:18:42Z palatis $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>

#include <chrasis.h>

#include "chmlcodec.h"

using namespace std;

namespace chrasis
{

Character::collection read_chml(std::string ifilename)
{
	Character::collection ret_chrs;

	try {
		xmlpp::TextReader reader(ifilename);

		// characters
		Character tmp_char;
		while(reader.read())
		{
			if (reader.get_name() != "character" ||
			   reader.get_node_type() == xmlpp::TextReader::EndElement)
				continue;

			Character tmp_char(reader.get_attribute("name"));
			// strokes
			while(reader.read())
			{
				if (reader.get_name() == "character")
					break;

				if (reader.get_name() != "stroke" ||
				    reader.get_node_type() == xmlpp::TextReader::EndElement)
					continue;

				// points
				Stroke tmp_stroke;
				while(reader.read())
				{
					if (reader.get_name() == "stroke")
						break;

					if (reader.get_name() != "point")
						continue;

					tmp_stroke.add_point(Point(
						boost::lexical_cast<int>(reader.get_attribute("x")),
						boost::lexical_cast<int>(reader.get_attribute("y"))));
					reader.move_to_element();
				}
				tmp_char.add_stroke(tmp_stroke);
			}
			ret_chrs.push_back(tmp_char);
		}
	}
	catch (const std::exception & ex)
	{
		std::cout << "Exception caught: " << ex.what() << std::endl;
	}
	
	return ret_chrs;
}

void write_chml(Character::collection & chrs, std::string ofilename)
{
	try
	{
		xmlpp::Document document;

		// root node (<characters> ... </characters>)
		xmlpp::Element * chrsNode = document.create_root_node("characters");

		for (Character::iterator ci = chrs.begin();
		     ci != chrs.end();
		     ++ci)
		{
			// character node (<character> ... </character>)
			xmlpp::Element * cNode = chrsNode->add_child("character");
			cNode->set_attribute("name", ci->get_name());
			
			for (Stroke::iterator si = ci->strokes_begin();
			     si != ci->strokes_end();
			     ++si)
			{
				// stroke node (<stroke> ... </stroke>)
				xmlpp::Element * sNode = cNode->add_child("stroke");
				
				for (Point::iterator pi = si->points_begin();
				     pi != si->points_end();
				     ++pi)
				{
					// point node (<point x="..." y="..." />)
					xmlpp::Element * pNode = sNode->add_child("point");

					pNode->set_attribute("x", boost::lexical_cast<std::string>(pi->x()));
					pNode->set_attribute("y", boost::lexical_cast<std::string>(pi->y()));
				}
			}
		}
		
		document.write_to_file_formatted(ofilename);
	}
	catch (const std::exception & ex)
	{
		std::cout << "Exception caught: " << ex.what() << std::endl;
	}
}

}
