/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/interval.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/mask_types.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {

/// Describes a resource element pattern within a resource grid.
struct re_pattern {
  /// Physical resource block mask.
  bounded_bitset<MAX_RB> prb_mask;
  /// Resource element mask per resource block. True entries indicate the resource elements affected by the pattern.
  re_prb_mask re_mask{};
  /// Symbol mask. True entries indicate the symbols affected by the pattern.
  symbol_slot_mask symbols{};

  /// Default constructor. It allows instantiating the structure without using other constructors.
  re_pattern() = default;

  /// \brief Constructs an RE pattern from parameters.
  /// \param[in] rb_begin  Start RB block index {0, ..., 274}.
  /// \param[in] rb_end    End RB block index {0, ..., 275} (excluded).
  /// \param[in] rb_stride RB index jump {1, ..., 274}.
  /// \param[in] re_mask_  RE Mask.
  /// \param[in] symbols_  Symbol mask.
  /// \remark An assertion is triggered if \c rb_begin, \c rb_end or \c rb_stride value is not within the indicated
  /// interval.
  /// \remark An assertion is triggered if \c rb_end is less than \c rb_begin.
  re_pattern(unsigned                rb_begin,
             unsigned                rb_end,
             unsigned                rb_stride,
             const re_prb_mask&      re_mask_,
             const symbol_slot_mask& symbols_) :
    prb_mask(rb_end), re_mask(re_mask_), symbols(symbols_)
  {
    static constexpr interval<unsigned, true> rb_begin_interval  = {0, MAX_RB - 1};
    static constexpr interval<unsigned, true> rb_stride_interval = {1, MAX_RB - 1};
    static constexpr interval<unsigned, true> rb_end_interval    = {1, MAX_RB};

    srsran_assert(rb_begin_interval.contains(rb_begin),
                  "The RB begin (i.e., {}) is out of range {}.",
                  rb_begin,
                  rb_begin_interval);
    srsran_assert(
        rb_end_interval.contains(rb_end), "The RB end (i.e., {}) is out of range {}.", rb_end, rb_end_interval);
    srsran_assert(rb_stride_interval.contains(rb_stride),
                  "The RB stride (i.e., {}) is out of range {}.",
                  rb_stride,
                  rb_stride_interval);
    srsran_assert(
        rb_begin < rb_end, "The RB begin (i.e., {}) must be smaller than RB end (i.e., {})", rb_begin, rb_end);

    if (rb_stride == 1) {
      prb_mask.fill(rb_begin, rb_end);
    } else {
      for (unsigned i_prb = rb_begin; i_prb < rb_end; i_prb += rb_stride) {
        prb_mask.set(i_prb);
      }
    }
  }

  /// \brief Copy constructor.
  ///
  /// \param[in] other Provides the reference to other resource element pattern to copy.
  re_pattern(const re_pattern& other) : prb_mask(other.prb_mask), re_mask(other.re_mask), symbols(other.symbols)
  {
    // Do nothing.
  }

  /// \brief Include the described resource element pattern in a resource grid symbol mask.
  ///
  /// This method sets to true the elements that are described in the pattern for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask Provides a mask representing an entire symbol in a resource grid.
  /// \param[in] symbol Indicates the symbol index for the mask to be included.
  /// \note This method expects that the number of elements of \c mask is equal to or greater than \c rb_end
  void get_inclusion_mask(bounded_bitset<MAX_RB * NRE>& mask, unsigned symbol) const;

  /// \brief Exclude the described resource element pattern in a resource grid symbol mask.
  ///
  /// This method sets to false the elements that are described in the pattern for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask Provides a mask representing an entire symbol in a resource grid.
  /// \param[in] symbol Indicates the symbol index for the mask to be excluded.
  /// \note This method expects that the number of elements of \c mask is equal to or greater than \c rb_end
  void get_exclusion_mask(bounded_bitset<MAX_RB * NRE>& mask, unsigned symbol) const;
};

/// \brief Checks if two RE patterns are equal.
///
/// Two RE patterns are equal if their attributes are equal.
inline bool operator==(const re_pattern& pattern1, const re_pattern& pattern2)
{
  if (pattern1.symbols != pattern2.symbols) {
    return false;
  }

  if (pattern1.prb_mask != pattern2.prb_mask) {
    return false;
  }

  if (pattern1.re_mask != pattern2.re_mask) {
    return false;
  }

  return true;
}

/// Describes a resource element pattern list.
class re_pattern_list
{
private:
  /// Defines the maximum number or RE pattern a list can carry.
  static constexpr unsigned MAX_RE_PATTERN = 4;

  /// Resource element pattern storage.
  static_vector<re_pattern, MAX_RE_PATTERN> list;

public:
  /// Default constructor.
  re_pattern_list() = default;

  /// Implicit construction from a single RE pattern.
  re_pattern_list(const re_pattern& pattern) : list() { list.emplace_back(pattern); }

  /// \brief Create a pattern list from an initializer list of patterns.
  ///
  /// This is useful for static constant pattern list creation. For example:
  /// \code
  ///   re_pattern_list list = {
  ///      {0, 52, 1, {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}},
  ///      {0, 52, 1, {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}}};
  /// \endcode
  ///
  /// \param[in] patterns Initializer list with patterns.
  re_pattern_list(std::initializer_list<const re_pattern> patterns)
  {
    for (const re_pattern& pattern : patterns) {
      merge(pattern);
    }
  }

  /// \brief Copy constructor.
  ///
  /// \param[in] other Provides other instance.
  re_pattern_list(const re_pattern_list& other) : list(other.list)
  {
    // Do nothing.
  }

  /// Clear the current pattern list.
  void clear() { list.clear(); }

  /// Get the number of internal resource element pattern.
  unsigned get_nof_entries() const { return list.size(); }

  /// \brief Merges a given resource element pattern list into the list.
  void merge(const re_pattern_list& other)
  {
    for (const auto& pattern : other.get_re_patterns()) {
      merge(pattern);
    }
  }

  /// \brief Merges a given resource element pattern into the list.
  ///
  /// This method tries to combine a new pattern with the existing ones in the list according to if
  /// - the resource block allocation parameters are identical, and
  /// - the symbol mask or the resource element mask is identical.
  /// Otherwise, the pattern is appended in the list.
  ///
  /// \param[in] pattern Provides the reference to a resource element pattern.
  void merge(const re_pattern& pattern);

  /// \brief Checks if the pattern list is equal to another pattern list.
  ///
  /// \remark Two pattern lists are considered equal if their generated inclusion masks are equal for the maximum number
  /// of RBs and symbols.
  /// \param[in] other The other pattern list.
  /// \return True if the patterns described by the two lists are equal.
  bool operator==(const re_pattern_list& other) const;

  /// \brief Checks if the pattern list is the pattern list is different from another pattern list.
  ///
  /// \param[in] other The other pattern list.
  /// \return True if the patterns described by the two lists are different.
  bool operator!=(const re_pattern_list& other) const { return !(*this == other); }

  /// \brief Includes the described resource element pattern list in a resource grid symbol mask.
  ///
  /// This method sets to true the elements that are described in the pattern list for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask   Provides a mask representing an entire symbol in a resource grid.
  /// \param[in]     symbol Indicates the symbol index for the mask to be included.
  /// \note This method expects that the number of elements of \c mask is equal to or greater than \c rb_end.
  void get_inclusion_mask(bounded_bitset<MAX_RB * NRE>& mask, unsigned symbol) const;

  /// \brief Counts the number of elements included in the described pattern.
  ///
  /// This method counts the included elements in the described pattern for a symbol range and a selection of PRB.
  ///
  /// \param[in] start_symbol Indicates the start symbol index within the slot.
  /// \param[in] nof_symbols  Indicates the number of symbols within the slot.
  /// \param[in] prb_mask     Selection of PRB to count.
  /// \return The number of included elements.
  unsigned
  get_inclusion_count(unsigned start_symbol, unsigned nof_symbols, const bounded_bitset<MAX_RB>& prb_mask) const;

  /// \brief Excludes the described resource element pattern list in a resource grid symbol mask.
  ///
  /// This method sets to false the elements that are described in the pattern list for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask   Provides a mask representing an entire symbol in a resource grid.
  /// \param[in]     symbol Indicates the symbol index for the mask to be excluded.
  /// \note This method expects that the number of elements of \c mask is equal to or greater than \c rb_end.
  void get_exclusion_mask(bounded_bitset<MAX_RB * NRE>& mask, unsigned symbol) const;

  /// Get the internal resource element patterns.
  span<const re_pattern> get_re_patterns() const { return list; }
};

} // namespace srsran
