// -*- Mode: C++ -*-
//
// Copyright (C) 2013-2020 Red Hat, Inc.
//
// This file is part of the GNU Application Binary Interface Generic
// Analysis and Instrumentation Library (libabigail).  This library is
// free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 3, or (at your option) any
// later version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this program; see the file COPYING-LGPLV3.  If
// not, see <http://www.gnu.org/licenses/>.

/// @file

#ifndef __ABG_CONFIG_H__
#define __ABG_CONFIG_H__

#include <string>

namespace abigail
{

/// This type abstracts the configuration information of the library.
class config
{
  std::string	m_format_minor;
  std::string	m_format_major;
  unsigned	m_xml_element_indent;
  std::string	m_tu_instr_suffix;
  std::string	m_tu_instr_archive_suffix;

public:
  config();

  const std::string&
  get_format_minor_version_number() const;

  void
  set_format_minor_version_number(const std::string& v);

  const std::string&
  get_format_major_version_number() const;

  void
  set_format_major_version_number(const std::string& v);

  unsigned
  get_xml_element_indent() const;

  void
  set_xml_element_indent(unsigned);

  const std::string&
  get_tu_instr_suffix() const;

  void
  set_tu_instr_suffix(const std::string&);

  const std::string&
  get_tu_instr_archive_suffix() const;

  void
  set_tu_instr_archive_suffix(const std::string&);
};

extern "C"
{
  /// Return the relevant version numbers of the library.
  ///
  /// \param maj the major version number of the library.
  ///
  /// \param min the minor version number of the library.
  ///
  /// \param rev the revision version number of the library.
  ///
  /// \param suf the version suffix of the library.
  void
  abigail_get_library_version(std::string& maj,
			      std::string& min,
			      std::string& rev,
			      std::string& suf);
}

}//end namespace abigail

#endif //__ABG_CONFIG_H__
