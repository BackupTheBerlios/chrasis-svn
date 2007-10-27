/*
 * Character Creater
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
 * $Id: chmlcodec.cpp 18 2006-09-19 21:18:42Z palatis $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include <chrasis.h>

#include "chmlcodec.h"

using namespace std;

namespace chrasis
{

Character::container
read_chml(std::string ifilename)
{
	Character::container ret_chrs;

	xmlTextReaderPtr reader = xmlReaderForFile(ifilename.c_str(), NULL, 0);
	if (reader == NULL)
		return ret_chrs;

	for (int cret = xmlTextReaderRead(reader);
	     cret == 1;
	     cret = xmlTextReaderRead(reader))
	{
		if (xmlTextReaderNodeType(reader) == 16)
			continue;

		std::string name(reinterpret_cast<const char *>(xmlTextReaderConstName(reader)));
		if (name != "character")
			continue;

		Character tmp_char(reinterpret_cast<const char *>(
			xmlTextReaderGetAttribute(reader,
				reinterpret_cast<const xmlChar *>("name"))));

		for (int sret = xmlTextReaderRead(reader);
		     sret == 1;
		     sret = xmlTextReaderRead(reader))
		{
			if (xmlTextReaderNodeType(reader) == 16)
				continue;

			std::string name(reinterpret_cast<const char *>(xmlTextReaderConstName(reader)));
			if (name == "character")
				break;
			if (name != "stroke")
				continue;
			
			Stroke tmp_stroke;
			for (int sret = xmlTextReaderRead(reader);
			     sret == 1;
			     sret = xmlTextReaderRead(reader))
			{
				if (xmlTextReaderNodeType(reader) == 16)
					continue;

				std::string name(reinterpret_cast<const char *>(xmlTextReaderConstName(reader)));
				if (name == "stroke")
					break;
				if (name != "point")
					continue;
				
				int x = fromString<int>(reinterpret_cast<const char*>(
					xmlTextReaderGetAttribute(reader,
						reinterpret_cast<const xmlChar *>("x"))
				));
				int y = fromString<int>(reinterpret_cast<const char*>(
					xmlTextReaderGetAttribute(reader,
						reinterpret_cast<const xmlChar *>("y"))
				));
				xmlTextReaderMoveToElement(reader);
				tmp_stroke.add_point(x, y);
			}
			tmp_char.add_stroke(tmp_stroke);
		}
		ret_chrs.push_back(tmp_char);
	}
	xmlFreeTextReader(reader);

	return ret_chrs;
}

void
write_chml(Character::container & chrs, std::string ofilename)
{
	xmlTextWriterPtr writer = xmlNewTextWriterFilename(ofilename.c_str(), 0);
	if (writer == NULL)
		return;

	xmlTextWriterSetIndent(writer, 1);

	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	xmlTextWriterStartElement(writer, reinterpret_cast<const xmlChar *>("characters"));

	for(Character::iterator ci = chrs.begin();
	    ci != chrs.end();
	    ++ci)
	{
		xmlTextWriterStartElement(writer, reinterpret_cast<const xmlChar *>("character"));
		xmlTextWriterWriteAttribute(writer,
			reinterpret_cast<const xmlChar *>("name"),
			reinterpret_cast<const xmlChar *>(ci->get_name().c_str()));

		for (Stroke::iterator si = ci->strokes_begin();
		     si != ci->strokes_end();
		     ++si)
		{
			xmlTextWriterStartElement(writer, reinterpret_cast<const xmlChar *>("stroke"));

			for (Point::iterator pi = si->points_begin();
			     pi != si->points_end();
			     ++pi)
			{
				xmlTextWriterStartElement(writer, reinterpret_cast<const xmlChar *>("point"));

				xmlTextWriterWriteAttribute(writer,
					reinterpret_cast<const xmlChar *>("x"),
					reinterpret_cast<const xmlChar *>(toString(pi->x()).c_str()));
				xmlTextWriterWriteAttribute(writer,
					reinterpret_cast<const xmlChar *>("y"),
					reinterpret_cast<const xmlChar *>(toString(pi->y()).c_str()));

				xmlTextWriterEndElement(writer);	// point
			}

			xmlTextWriterEndElement(writer);	// stroke
		}

		xmlTextWriterEndElement(writer);	// character
	}

	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
}

}
