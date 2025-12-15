// -*- mode: C++ -*-
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

#ifndef __ABG_LIBZIP_UTILS_H__
#define __ABG_LIBZIP_UTILS_H__

#include <zip.h>
#include "abg-cxx-compat.h"

namespace abigail
{

namespace zip_utils
{

using abg_compat::shared_ptr;
using std::string;

/// @brief Functor passed to shared_ptr constructor during
/// instantiation with zip*
///
/// Its aim is to delete zip* managed by shared_ptr.
struct archive_deleter
{
  void
  operator()(zip* archive)
  {
    /// ??? Maybe check the return code of close and throw if the
    /// close fails?  But then callers must be prepared to handle
    /// this.
    zip_close(archive);
  }
};//end archive_deleter

/// @brief Functor passed to shared_ptr<zip_file>'s constructor.
///
/// Its aim is to close (actually delete) the zip_file* managed by the
/// shared_ptr.
struct zip_file_deleter
{
  void
  operator()(zip_file*f)
  {
    zip_fclose(f);
  }
};

typedef shared_ptr<zip> zip_sptr;
zip_sptr open_archive(const string& path, int flags, int *errorp);

typedef shared_ptr<zip_file> zip_file_sptr;
zip_file_sptr open_file_in_archive(zip_sptr archive,
				   int file_index);

}// end namespace zip
}// end namespace abigail
#endif //__ABG_LIBZIP_UTILS_H__
