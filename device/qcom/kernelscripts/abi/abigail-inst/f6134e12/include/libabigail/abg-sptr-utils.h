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
///
/// Utilities to ease the wrapping of C types into std::tr1::shared_ptr

#ifndef __ABG_SPTR_UTILS_H__
#define __ABG_SPTR_UTILS_H__

#include <regex.h>

#include "abg-cxx-compat.h"

namespace abigail
{

/// Namespace for the utilities to wrap C types into std::tr1::shared_ptr.
namespace sptr_utils
{

using abg_compat::shared_ptr;

/// This is to be specialized for the diverse C types that needs
/// wrapping in shared_ptr.
///
/// @tparam T the type of the C type to wrap in a shared_ptr.
///
/// @param p a pointer to wrap in a shared_ptr.
///
/// @return then newly created shared_ptr<T>
template<class T>
shared_ptr<T>
build_sptr(T* p);

/// This is to be specialized for the diverse C types that needs
/// wrapping in shared_ptr.
///
/// This variant creates a pointer to T and wraps it into a
/// shared_ptr<T>.
///
/// @tparam T the type of the C type to wrap in a shared_ptr.
///
/// @return then newly created shared_ptr<T>
template<class T>
shared_ptr<T>
build_sptr();

/// A deleter for shared pointers that ... doesn't delete the object
/// managed by the shared pointer.
struct noop_deleter
{
  template<typename T>
  void
  operator()(const T*)
  {}
};

}// end namespace sptr_utils
}// end namespace abigail

#endif //__ABG_SPTR_UTILS_H__
