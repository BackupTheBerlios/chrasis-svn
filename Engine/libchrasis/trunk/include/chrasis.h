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
 * $Id$
 */

/**
 * \author $Author$
 *
 * \version $Revision$
 *
 * \date $Date$
 */

#ifndef _CHRASIS_H
#define _CHRASIS_H

#ifndef CHRASIS_API
# define CHRASIS_API
#endif

#ifndef CHRASIS_INTERNAL
# define CHRASIS_INTERNAL __attribute__ ((visibility ("hidden")))
#endif

#include <complex>
#include <cmath>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <iterator>
#include <iostream>
#include <sstream>
#include <fstream>

#include <sys/stat.h>
#include <unistd.h>

#include <chrasis/character.h>
#include <chrasis/global.h>
#include <chrasis/settings.h>
#include <chrasis/database.h>

namespace chrasis
{

class ItemPossibility
{
private:
	struct Item;
public:
	typedef double				possibility_t;
	typedef	std::vector< Item >		container;
	typedef container::iterator		iterator;
	typedef container::const_iterator	const_iterator;

	enum SORTING_POLICY {
		SORTING_POSSIBILITY,
		SORTING_ID,
		SORTING_NAME
	};

	void add_item(possibility_t p, int i, std::string n = std::string());

	void sort(SORTING_POLICY const sp);
	
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	size_t empty() const;
	size_t size() const;

	ItemPossibility & operator += (ItemPossibility const & rhs);

private:
	struct Item
	{
		Item(possibility_t p, int i, std::string n);

		possibility_t	possibility;
		int		id;
		std::string	name;
	};

	struct PossibilityComparer
	{
		bool operator() (Item const & lhs, Item const & rhs);
	};
	struct IdComparer
	{
		bool operator() (Item const & lhs, Item const & rhs);
	};
	struct NameComparer
	{
		bool operator() (Item const & lhs, Item const & rhs);
	};

	container items_;
};

/**
 * Normalize a single stroke based on distance threshold.
 *
 * @brief normalize a stroke
 *
 * @param stroke the original stroke
 * @param distance_threshold the distance threshold of the elimination of points
 * @return normalized stroke
 *
 */
CHRASIS_API
Stroke
normalize(Stroke const & stroke, int const distance_threshold);

/**
 * Walk through each stroke in a character and calls
 * normalize(stroke) with proper distance threshold on them.
 *
 * @brief normalize a character
 *
 * @param character the original character
 */
CHRASIS_API
Character
normalize(Character const & character);

/**
 * Try recognize the character with pre-defined databases.
 *   - system database
 *     - in /usr/share/chrasis/chr_data.db
 *   - user database
 *     - in ~/.chrasis/chr_data.db
 *
 * @param character the character to be recognized
 * @return a list of character with alikeness
 *
 * @see character_possibility_t
 *
 */
CHRASIS_API
ItemPossibility
recognize(Character const & character);

/**
 * Try recognize the character with given databases.
 *
 * @param character the character to be recognized
 * @param odb the database with character/stroke informations
 * @return a list of character with alikeness
 *
 * @see character_possibility_t
 * @see Database::OPENDB
 */
CHRASIS_API
ItemPossibility
recognize(Character const & character, SQLite::Command & cmd);

/**
 * Learn the specified character into user database.
 * The given character would be either:
 *   - remembered if
 *     - the character doesn't exist in the database, or
 *     - character in db looks not alike
 *   - reflected if
 *     - character exists in db and is somewhat alike this one
 *
 * @param character the character to be learned
 * @return true if learned with no problem, false otherwise.
 *
 * @see recognize
 */
CHRASIS_API
bool
learn(Character const & character);

/**
 * Learn the specified character into the specified database.
 * The given character would be either:
 *   - remembered if
 *     - the character doesn't exist in the database, or
 *     - character in db looks not alike
 *   - reflected if
 *     - character exists in db and is somewhat alike this one
 *
 * @param character the character to be learned
 * @param odb the database used to store the character information.
 * @return true if learned with no problem, false otherwise.
 *
 * @see recognize
 * @see Database::OPENDB
 */
CHRASIS_API
bool
learn(Character const & character, SQLite::Command & cmd);

} // namespace chrasis

#endif
