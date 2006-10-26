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

#include <iostream>
#include <vector>

#include "character.h"
#include "chmlcodec.h"

using namespace std;

Character::collection read_chml(Glib::ustring ifilename)
{
	Character::collection ret_chars;

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
						static_cast<int>(Glib::Ascii::strtod(reader.get_attribute("x"))),
						static_cast<int>(Glib::Ascii::strtod(reader.get_attribute("y")))));
					reader.move_to_element();
				}
				tmp_char.add_stroke(tmp_stroke);
			}
			ret_chars.push_back(tmp_char);
		}
	}
	catch (const std::exception & ex)
	{
		std::cout << "Exception caught: " << ex.what() << std::endl;
	}
	
	return ret_chars;
}

void write_chml(Character::collection & chars, Glib::ustring ofilename)
{
	try
	{
		xmlpp::Document document;

		// root node (<characters> ... </characters>)
		xmlpp::Element * charsNode = document.create_root_node("characters");

		for (Character::iterator chars_i = chars.begin();
		     chars_i != chars.end();
		     ++chars_i)
		{
			// character node (<character> ... </character>)
			xmlpp::Element * charNode = charsNode->add_child("character");
			charNode->set_attribute("name", chars_i->get_name());
			
			for (Stroke::iterator strokes_i = chars_i->strokes_begin();
			     strokes_i != chars_i->strokes_end();
			     ++strokes_i)
			{
				// stroke node (<stroke> ... </stroke>)
				xmlpp::Element * strokeNode = charNode->add_child("stroke");
				
				for (Point::iterator points_i = strokes_i->points_begin();
				     points_i != strokes_i->points_end();
				     ++points_i)
				{
					// point node (<point x="..." y="..." />)
					xmlpp::Element * pointNode = strokeNode->add_child("point");

					pointNode->set_attribute("x", Glib::Ascii::dtostr(points_i->x()));
					pointNode->set_attribute("y", Glib::Ascii::dtostr(points_i->y()));
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

