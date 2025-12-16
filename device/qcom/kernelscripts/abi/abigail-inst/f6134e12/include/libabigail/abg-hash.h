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

#ifndef __ABG_HASH_H__
#define __ABG_HASH_H__

#include <stdint.h>
#include <cstddef>
#include <string>

namespace abigail
{
/// Namespace for hashing.
namespace hashing
{
  /// Produce good hash value combining val1 and val2.
  /// This is copied from tree.c in GCC.
  std::size_t
  combine_hashes(std::size_t, std::size_t);

  uint32_t
  fnv_hash(const std::string& str);
}//end namespace hashing
}//end namespace abigail

#endif //__ABG_HASH_H__
