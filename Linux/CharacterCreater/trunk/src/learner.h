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

#ifndef _LEANER_H
#define _LEANER_H

#include "common.h"
#include "character.h"
#include "database.h"

class basic_learner
{
public:
	typedef std::map< int, Character >	CharacterMemories;

	basic_learner(Database & db): db_(db) { prepare_database(); };

	void learn(Character const &);

private:
	void prepare_database();
	void remember(Character const &);
	void reflect(Character const &, int const);
	CharacterMemories recall(std::string const &);

	Database & db_;
};

typedef basic_learner Learner;

#ifndef USE_EXPORT
//# include "learner.hpp"
#endif

#endif
