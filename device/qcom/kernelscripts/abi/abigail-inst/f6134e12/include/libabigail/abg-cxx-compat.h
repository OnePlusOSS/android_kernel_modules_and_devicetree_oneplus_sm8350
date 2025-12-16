// -*- Mode: C++ -*-
//
// Copyright (C) 2019-2020 Google, Inc.
//
// This file is part of the GNU Application Binary Interface Generic
// Analysis and Instrumentation Library (libabigail).  This library is
// free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
// later version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this program; see the file COPYING-LGPLV2.  If
// not, see <http://www.gnu.org/licenses/>.

/// @file

#ifndef __ABG_CXX_COMPAT_H
#define __ABG_CXX_COMPAT_H

// C++11 support (mostly via tr1 if compiled with earlier standard)

#if __cplusplus >= 201103L

#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#else

#include <tr1/functional>
#include <tr1/memory>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#endif

// C++17 support (via custom implementations if compiled with earlier standard)

#if __cplusplus >= 201703L

#include <optional>

#else

#include <stdexcept> // for throwing std::runtime_error("bad_optional_access")

#endif

namespace abg_compat {

#if __cplusplus >= 201103L

// <functional>
using std::bind;
using std::function;
using std::hash;

namespace placeholders
{
using namespace std::placeholders;
}

// <memory>
using std::shared_ptr;
using std::weak_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;

// <unordered_map>
using std::unordered_map;

// <unordered_set>
using std::unordered_set;

#else

// <functional>
using std::tr1::bind;
using std::tr1::function;
using std::tr1::hash;

namespace placeholders
{
using namespace std::tr1::placeholders;
}

// <memory>
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::dynamic_pointer_cast;
using std::tr1::static_pointer_cast;

// <unordered_map>
using std::tr1::unordered_map;

// <unordered_set>
using std::tr1::unordered_set;

#endif

#if __cplusplus >= 201703L

using std::optional;

#else

// <optional>

/// Simplified implementation of std::optional just enough to be used as a
/// replacement for our purposes and when compiling with pre C++17.
///
/// The implementation intentionally does not support a whole lot of features
/// to minimize the maintainence effort with this.
template <typename T> class optional
{
  bool has_value_;
  T    value_;

public:
  optional() : has_value_(false), value_() {}
  optional(const T& value) : has_value_(true), value_(value) {}

  bool
  has_value() const
  {
    return has_value_;
  }

  const T&
  value() const
  {
    if (!has_value_)
      throw std::runtime_error("bad_optional_access");
    return value_;
  }

  const T
  value_or(const T& default_value) const
  {
    if (!has_value_)
      return default_value;
    return value_;
  }

  const T&
  operator*() const
  { return value_; }

  T&
  operator*()
  { return value_; }

  const T*
  operator->() const
  { return &value_; }

  T*
  operator->()
  { return &value_; }

  optional&
  operator=(const T& value)
  {
    has_value_ = true;
    value_ = value;
    return *this;
  }

  explicit operator bool() const { return has_value_; }
};

#endif
}

#endif  // __ABG_CXX_COMPAT_H
