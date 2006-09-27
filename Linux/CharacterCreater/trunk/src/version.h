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
 * $Id: version.h 18 2006-09-19 21:18:42Z palatis $
 */

#ifndef _VERSION_H
#define _VERSION_H

#include <vector>
#include <string>

#include <glibmm/i18n.h>

/**
 * do not inherit this class...
 */
class Version
{
private:
	class AuthorsVector;
public:
	Version():
		PROGRAM_NAME_("Character Creater"),
		PROGRAM_VERSION_(_("0.0.1")),
		PROGRAM_COPYRIGHT_("Victor Tseng"),
		PROGRAM_COMMENTS_(_("Character Creater is a tool to create characters for the RAOCHRS -- for development purpose.")),
		PROGRAM_LICENSE_(_("GPL")),
		PROGRAM_WEBSITE_(_("http://rt.openfoundry.org/Foundry/Project/Wiki/624/")),
		PROGRAM_WEBSITE_LABEL_(_("RAOCHRS Wiki"))
	{
		PROGRAM_AUTHORS_.push_back("Victor Tseng <palatis@gentoo.tw>");
	};

	const std::string & PROGRAM_NAME() const { return PROGRAM_NAME_; };
	const std::string & PROGRAM_VERSION() const { return PROGRAM_VERSION_; };
	const std::string & PROGRAM_COPYRIGHT() const { return PROGRAM_COPYRIGHT_; };
	const std::string & PROGRAM_COMMENTS() const { return PROGRAM_COMMENTS_; };
	const std::string & PROGRAM_LICENSE() const { return PROGRAM_LICENSE_; };
	const std::string & PROGRAM_WEBSITE() const { return PROGRAM_WEBSITE_; };
	const std::string & PROGRAM_WEBSITE_LABEL() const { return PROGRAM_WEBSITE_LABEL_; };
	const AuthorsVector PROGRAM_AUTHORS() const { return AuthorsVector(PROGRAM_AUTHORS_); };

private:
	class AuthorsVector {
	public:
		typedef std::vector< std::string >::const_iterator const_iterator;

		AuthorsVector(std::vector< std::string > const & av): av_(&av) {};
		int size() const { return av_->size(); };
		std::string const & operator[](int idx) const { return (*av_)[idx]; };

		const_iterator begin() { return av_->begin(); };
		const_iterator end() { return av_->end(); };
	private:
		std::vector< std::string > const * av_;	// actual vector
	};

	std::string PROGRAM_NAME_;
	std::string PROGRAM_VERSION_;
	std::string PROGRAM_COPYRIGHT_;
	std::string PROGRAM_COMMENTS_;
	std::string PROGRAM_LICENSE_;
	std::string PROGRAM_WEBSITE_;
	std::string PROGRAM_WEBSITE_LABEL_;
	std::vector<std::string> PROGRAM_AUTHORS_;
};

const Version & VersionInfo()
{
	static const Version ver;
	return ver;
}

#endif
