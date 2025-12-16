// -*- Mode: C++ -*-
//
// Copyright (C) 2020 Google, Inc.
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
//
// Author: Matthias Maennich

/// @file
///
/// This contains the declarations for the symtab reader.

#ifndef __ABG_SYMTAB_READER_H__
#define __ABG_SYMTAB_READER_H__

#include <gelf.h>

#include <iterator>
#include <vector>

#include "abg-cxx-compat.h"
#include "abg-ir.h"

namespace abigail
{
namespace symtab_reader
{

class symtab_filter_builder;

/// The symtab filter is the object passed to the symtab object in order to
/// iterate over the symbols in the symtab while applying filters.
///
/// The general idea is that it consists of a set of optionally enforced flags,
/// such as 'functions' or 'variables'. If not set, those are not filtered for,
/// neither inclusive nor exclusive. If set they are all ANDed together.
class symtab_filter
{
public:
  // The symtab_filter_builder helps us to build filters efficiently, hence
  // let's be nice and grant access to our internals.
  friend class symtab_filter_builder;

  // Default constructor disabling all features.
  symtab_filter() {}

  /// Determine whether a symbol is matching the filter criteria of this filter
  /// object. In terms of a filter functionality, you would _not_ filter out
  /// this symbol if it passes this (i.e. returns true).
  ///
  /// @param symbol The Elf symbol under test.
  ///
  /// @return whether the symbol matches all relevant / required criteria
  bool
  matches(const elf_symbol_sptr& symbol) const;

private:
  // The symbol is a function (FUNC)
  abg_compat::optional<bool> functions_;

  // The symbol is a variables (OBJECT)
  abg_compat::optional<bool> variables_;

  // The symbol is publicly accessible (global/weak with default/protected
  // visibility)
  abg_compat::optional<bool> public_symbols_;

  // The symbols is not defined (declared)
  abg_compat::optional<bool> undefined_symbols_;

  // The symbol is listed in the ksymtab (for Linux Kernel binaries).
  abg_compat::optional<bool> kernel_symbols_;
};

/// Helper class to provide an attractive interface to build symtab_filters.
///
/// When constructed, the helper instantiates a default symtab_filter and
/// allows modifications to it via builder pattern / fluent interface.
///
/// When assigned to a symtab_filter instance, it converts by returning the
/// locally build symtab_filter instance.
///
/// Example usage:
///
///   const symtab_filter filter =
///                symtab_filter_builder().functions().kernel_symbols();
///
/// In that case we would filter for the conjunction of function symbols that
/// also appear in the ksymtab (i.e. kernel symbols).
class symtab_filter_builder
{
public:
  /// Enable inclusive / exclusive filtering for functions.
  symtab_filter_builder&
  functions(bool value = true)
  { filter_.functions_ = value; return *this; }

  /// Enable inclusive / exclusive filtering for variables.
  symtab_filter_builder&
  variables(bool value = true)
  { filter_.variables_ = value; return *this; }

  /// Enable inclusive / exclusive filtering for public symbols.
  symtab_filter_builder&
  public_symbols(bool value = true)
  { filter_.public_symbols_ = value; return *this; }

  /// Enable inclusive / exclusive filtering for undefined symbols.
  symtab_filter_builder&
  undefined_symbols(bool value = true)
  { filter_.undefined_symbols_ = value; return *this; }

  /// Enable inclusive / exclusive filtering for kernel symbols.
  symtab_filter_builder&
  kernel_symbols(bool value = true)
  { filter_.kernel_symbols_ = value; return *this; }

  /// Convert seamlessly to a symtab_filter instance.
  ///
  /// We could possibly validate the filter constellations here. For now, we
  /// just return the local filter instance.
  operator symtab_filter() { return filter_; }

private:
  /// Local symtab_filter instance that we build and eventually pass on.
  symtab_filter filter_;
};

/// Base iterator for our custom iterator based on whatever the const_iterator
/// is for a vector of symbols.
/// As of writing this, std::vector<elf_symbol_sptr>::const_iterator.
typedef elf_symbols::const_iterator base_iterator;

/// An iterator to walk a vector of elf_symbols filtered by symtab_filter.
///
/// The implementation inherits all properties from the vector's
/// const_iterator, but intercepts where necessary to allow effective
/// filtering. This makes it a STL compatible iterator for general purpose
/// usage.
class symtab_iterator : public base_iterator
{
public:
  typedef base_iterator::value_type	 value_type;
  typedef base_iterator::reference	 reference;
  typedef base_iterator::pointer	 pointer;
  typedef base_iterator::difference_type difference_type;
  typedef std::forward_iterator_tag	 iterator_category;

  /// Construct the iterator based on a pair of underlying iterators and a
  /// symtab_filter object. Immediately fast forward to the next element that
  /// matches the criteria (if any).
  symtab_iterator(base_iterator	       begin,
		  base_iterator	       end,
		  const symtab_filter& filter = symtab_filter())
    : base_iterator(begin), end_(end), filter_(filter)
  { skip_to_next(); }

  /// Pre-increment operator to advance to the next matching element.
  symtab_iterator&
  operator++()
  {
    base_iterator::operator++();
    skip_to_next();
    return *this;
  }

  /// Post-increment operator to advance to the next matching element.
  symtab_iterator
  operator++(int)
  {
    symtab_iterator result(*this);
    ++(*this);
    return result;
  }

private:
  /// The end of the underlying iterator.
  const base_iterator end_;

  /// The symtab_filter used to determine when to advance.
  const symtab_filter& filter_;

  /// Skip to the next element that matches the filter criteria (if any). Hold
  /// off when reaching the end of the underlying iterator.
  void
  skip_to_next()
  {
    while (*this != end_ && !filter_.matches(**this))
      ++(*this);
  }
};

/// Convenience declaration of a shared_ptr<symtab>
class symtab;
typedef abg_compat::shared_ptr<symtab> symtab_sptr;

/// symtab is the actual data container of the symtab_reader implementation.
///
/// The symtab is instantiated either via an Elf handle (from binary) or from a
/// set of existing symbol maps (usually when instantiated from XML). It will
/// then discover the symtab, possibly the ksymtab (for Linux Kernel binaries)
/// and setup the data containers and lookup maps for later perusal.
///
/// The symtab is supposed to be used in a const context as all information is
/// already computed at construction time. Symbols are stored sorted to allow
/// deterministic reading of the entries.
///
/// An example use of the symtab class is
///
/// const symtab_sptr   tab    = symtab::load(elf_handle, env);
/// const symtab_filter filter = tab->make_filter()
///                              .public_symbols()
///                              .functions();
///
/// for (symtab::const_iterator I = tab.begin(filter), E = tab.end();
///                             I != E; ++I)
///   {
///       std::cout << (*I)->get_name() << "\n";
///   }
///
/// C++11 and later allows a more brief syntax for the same:
///
///   for (const auto& symbol : filtered_symtab(*tab, filter))
///     {
///       std::cout << symbol->get_name() << "\n";
///     }
///
/// This uses the filtered_symtab proxy object to capture the filter.
class symtab
{
public:
  typedef abg_compat::function<bool(const elf_symbol_sptr&)> symbol_predicate;

  /// Indicate whether any (kernel) symbols have been seen at construction.
  ///
  /// @return true if there are symbols detected earlier.
  bool
  has_symbols() const
  { return is_kernel_binary_ ? has_ksymtab_entries_ : !symbols_.empty(); }

  /// Obtain a suitable default filter for iterating this symtab object.
  ///
  /// The symtab_filter_build obtained is populated with some sensible default
  /// settings, such as public_symbols(true) and kernel_symbols(true) if the
  /// binary has been identified as Linux Kernel binary.
  ///
  /// @return a symtab_filter_builder with sensible populated defaults
  symtab_filter_builder
  make_filter() const;

  /// The (only) iterator type we offer is a const_iterator implemented by the
  /// symtab_iterator.
  typedef symtab_iterator const_iterator;

  /// Obtain an iterator to the beginning of the symtab according to the filter
  /// criteria. Whenever this iterator advances, it skips elements that do not
  /// match the filter criteria.
  ///
  /// @param filter the symtab_filter to match symbols against
  ///
  /// @return a filtering const_iterator of the underlying type
  const_iterator
  begin(const symtab_filter& filter) const
  { return symtab_iterator(symbols_.begin(), symbols_.end(), filter); }

  /// Obtain an iterator to the end of the symtab.
  ///
  /// @return an end iterator
  const_iterator
  end() const
  { return symtab_iterator(symbols_.end(), symbols_.end()); }

  /// Get a vector of symbols that are associated with a certain name
  ///
  /// @param name the name the symbols need to match
  ///
  /// @return a vector of symbols, empty if no matching symbols have been found
  const elf_symbols&
  lookup_symbol(const std::string& name) const;

  /// Lookup a symbol by its address
  ///
  /// @param symbol_addr the starting address of the symbol
  ///
  /// @return a symbol if found, else an empty sptr
  const elf_symbol_sptr&
  lookup_symbol(GElf_Addr symbol_addr) const;

  /// Construct a symtab object and instantiate from an ELF handle. Also pass
  /// in an ir::environment handle to interact with the context we are living
  /// in. If specified, the symbol_predicate will be respected when creating
  /// the full vector of symbols.
  static symtab_sptr
  load(Elf*		elf_handle,
       ir::environment* env,
       symbol_predicate is_suppressed = NULL);

  /// Construct a symtab object from existing name->symbol lookup maps.
  /// They were possibly read from a different representation (XML maybe).
  static symtab_sptr
  load(string_elf_symbols_map_sptr function_symbol_map,
       string_elf_symbols_map_sptr variables_symbol_map);

  /// Notify the symtab about the name of the main symbol at a given address.
  ///
  /// From just alone the symtab we can't guess the main symbol of a bunch of
  /// aliased symbols that all point to the same address. During processing of
  /// additional information (such as DWARF), this information becomes apparent
  /// and we can adjust the addr->symbol lookup map as well as the alias
  /// reference of the symbol objects.
  ///
  /// @param addr the addr that we are updating the main symbol for
  /// @param name the name of the main symbol
  void
  update_main_symbol(GElf_Addr addr, const std::string& name);

private:
  /// Default constructor. Private to enforce creation by factory methods.
  symtab();

  /// The vector of symbols we discovered.
  elf_symbols symbols_;

  /// Whether this is a Linux Kernel binary
  bool is_kernel_binary_;

  /// Whether this kernel_binary has ksymtab entries
  ///
  /// A kernel module might not have a ksymtab if it does not export any
  /// symbols. In order to quickly decide whether the symbol table is empty, we
  /// remember whether we ever saw ksymtab entries.
  bool has_ksymtab_entries_;

  /// Lookup map name->symbol(s)
  typedef abg_compat::unordered_map<std::string, std::vector<elf_symbol_sptr> >
		       name_symbol_map_type;
  name_symbol_map_type name_symbol_map_;

  /// Lookup map name->symbol
  typedef abg_compat::unordered_map<GElf_Addr, elf_symbol_sptr>
		       addr_symbol_map_type;
  addr_symbol_map_type addr_symbol_map_;

  /// Lookup map function entry address -> symbol
  addr_symbol_map_type entry_addr_symbol_map_;

  /// Load the symtab representation from an Elf binary presented to us by an
  /// Elf* handle.
  ///
  /// This method iterates over the entries of .symtab and collects all
  /// interesting symbols (functions and variables).
  ///
  /// In case of a Linux Kernel binary, it also collects information about the
  /// symbols exported via EXPORT_SYMBOL in the Kernel that would then end up
  /// having a corresponding __ksymtab entry.
  ///
  /// Symbols that are suppressed will be omitted from the symbols_ vector, but
  /// still be discoverable through the name->symbol and addr->symbol lookup
  /// maps.
  bool
  load_(Elf* elf_handle, ir::environment* env, symbol_predicate is_suppressed);

  /// Load the symtab representation from a function/variable lookup map pair.
  ///
  /// This method assumes the lookup maps are correct and sets up the data
  /// vector as well as the name->symbol lookup map. The addr->symbol lookup
  /// map cannot be set up in this case.
  bool
  load_(string_elf_symbols_map_sptr function_symbol_map,
       string_elf_symbols_map_sptr variables_symbol_map);

  void
  update_function_entry_address_symbol_map(Elf*	     elf_handle,
					   GElf_Sym* native_symbol,
					   const elf_symbol_sptr& symbol_sptr);
};

/// Helper class to allow range-for loops on symtabs for C++11 and later code.
/// It serves as a proxy for the symtab iterator and provides a begin() method
/// without arguments, as required for range-for loops (and possibly other
/// iterator based transformations).
///
/// Example usage:
///
///   for (const auto& symbol : filtered_symtab(tab, filter))
///     {
///       std::cout << symbol->get_name() << "\n";
///     }
///
class filtered_symtab
{
  const symtab&	      tab_;
  const symtab_filter filter_;

public:
  /// Construct the proxy object keeping references to the underlying symtab
  /// and the filter object.
  filtered_symtab(const symtab& tab, const symtab_filter& filter)
    : tab_(tab), filter_(filter) { }

  /// Pass through symtab.begin(), but also pass on the filter.
  symtab::const_iterator
  begin() const
  { return tab_.begin(filter_); }

  /// Pass through symtab.end().
  symtab::const_iterator
  end() const
  { return tab_.end(); }
};

} // end namespace symtab_reader
} // end namespace abigail

#endif // __ABG_SYMTAB_READER_H__
