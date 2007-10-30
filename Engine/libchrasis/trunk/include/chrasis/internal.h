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

#ifndef _CHRASIS_INTERNAL_H
#define _CHRASIS_INTERNAL_H

#define CHRASIS_INTERNAL

#include "character.h"

namespace chrasis
{

// database related

#define DEFAULT_DB_FILE		"chr_data.db"
#define DEFAULT_EMPTYDB_FILE	"empty_db.db"
#define DEFAULT_SCHEMA_FILE	"db_schema.sql"

// used by recognize() and learn()
class CharacterImpl
{
public:
	class StrokeImpl
	{
	public:
		typedef Character::Stroke::Point	point_t;
		typedef	point_t::value_t		value_t;
		typedef std::vector< StrokeImpl >	container;
		typedef container::iterator		iterator;
		typedef container::const_iterator	const_iterator;

		StrokeImpl(int const & ssid,
			point_t const & lt, point_t const & rb):
			ss_id_(ssid), lt_(lt), rb_(rb)
		{ }

		StrokeImpl(int const & ssid,
			value_t const & ltx, value_t const & lty,
			value_t const & rbx, value_t const & rby):
			ss_id_(ssid), lt_(ltx, lty), rb_(rbx, rby)
		{ }

		int const & get_stroke_shape_id() const { return ss_id_; }
		point_t const & get_lefttop() const { return lt_; }
		point_t const & get_rightbottom() const { return rb_; }

	private:
		int				ss_id_;
		Character::Stroke::Point	lt_;
		Character::Stroke::Point	rb_;
	};
	
	typedef StrokeImpl::point_t		point_t;
	typedef point_t::value_t		value_t;
	typedef std::vector< CharacterImpl >	container;
	typedef container::iterator		iterator;
	typedef container::const_iterator	const_iterator;

	CharacterImpl(std::string const & n = std::string()):
		name_(n)
	{ }

	void add_stroke(int const & ssid,
		point_t const & lt, point_t const & rb)
	{
		stroke_impls_.push_back( StrokeImpl(ssid, lt, rb) );
	}
	void add_stroke(int const & ssid,
		value_t const & ltx, value_t const & lty,
		value_t const & rbx, value_t const & rby)
	{
		stroke_impls_.push_back( StrokeImpl(ssid, ltx, lty, rbx, rby) );
	}
	
	int stroke_count() const { return stroke_impls_.size(); }
	
	StrokeImpl::iterator strokes_begin() { return stroke_impls_.begin(); }
	StrokeImpl::iterator strokes_end() { return stroke_impls_.end(); }
	StrokeImpl::const_iterator strokes_begin() const { return stroke_impls_.begin(); }
	StrokeImpl::const_iterator strokes_end() const { return stroke_impls_.end(); }

	std::string get_name() const { return name_; }
	void set_name(std::string const & n) { name_ = n; }
private:
	StrokeImpl::container stroke_impls_;
	std::string name_;
};

CHRASIS_INTERNAL
Stroke
_shift(Stroke const & nstr);

// used by recognize()

// these numbers are basically magics...
static const double	ANGLE_THRESHOLD		= 30.0 / 180.0 * M_PI;	//< 30 deg
static const double	DIST_THRESHOLD_RATIO	= 1.0 / 15.0;		//< 1/15 of diagonal line
static const int	RESOLUTION		= 10000;		//< sampling resolution

CHRASIS_INTERNAL
id_container_t
_get_ids_by_prototype(Stroke const & pstr, Database::OPENDB & odb);

CHRASIS_INTERNAL
Stroke
_get_stroke_by_id(int id, Database::OPENDB & odb);

CHRASIS_INTERNAL
stroke_possibility_t
_recognize(Stroke const & nstr, Database::OPENDB & odb);

CHRASIS_INTERNAL
id_container_t
_get_ids_by_prototype(Character const & pchr, std::vector< stroke_possibility_t > sptv, Database::OPENDB & odb);

CHRASIS_INTERNAL
CharacterImpl
_get_character_by_id(int const & id, Database::OPENDB & odb);

CHRASIS_INTERNAL
character_possibility_t
_recognize(Character const & nchr, Database::OPENDB & odb);

// used by learn()

// these numbers a basically magics...
static const double	LEARNING_THRESHOLD	= 0.15 * 0.15;		//< 15% of sampling resolution

CHRASIS_INTERNAL
int
_learn(Stroke const & nstr, Database::OPENDB & odb);

CHRASIS_INTERNAL
int
_remember(Stroke const & nstr, Database::OPENDB & odb);

CHRASIS_INTERNAL
int
_reflect(Stroke const & nstr, int id, Database::OPENDB & odb);

CHRASIS_INTERNAL
int
_learn(Character const & nchr, Database::OPENDB & odb);

CHRASIS_INTERNAL
int
_remember(CharacterImpl const & chr_impl, id_container_t const & shape_ids, Database::OPENDB & odb);

CHRASIS_INTERNAL
int
_reflect(CharacterImpl const & chr_impl, int const id, Database::OPENDB & odb);

} // namespace chrasis

#endif
