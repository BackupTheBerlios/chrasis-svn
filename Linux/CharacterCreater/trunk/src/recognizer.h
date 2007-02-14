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

#ifndef _RECOGNIZER_H
#define _RECOGNIZER_H

#include "common.h"

#include "character.h"
#include "database.h"

class Recognizer
{
public:
	typedef double possibility_t;
	typedef std::map<
		possibility_t,
		std::pair< int, std::string > > character_possibility_t;

	static Recognizer & Instance();

	Stroke normalize(const Stroke &, const Point::value_t) const;
	Character normalize(const Character &) const;
	character_possibility_t recognize(const Character &, Database &) const;

private:
	Recognizer() {};
	Recognizer(const Recognizer &) {};
	Recognizer & operator=(const Recognizer &) { return *this; };
	~Recognizer() {};
};

#endif
