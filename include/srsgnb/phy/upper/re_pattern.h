/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/support/mask_types.h"
#include "srsgnb/ran/cyclic_prefix.h"

namespace srsgnb {

/// Describes a resource element pattern within a resource grid.
struct re_pattern {
  /// Resource block where the pattern begins in frequency domain, the range is (0...274).
  unsigned rb_begin{0};
  /// Resource block where the pattern ends in frequency domain (excluded), the range is (1...275).
  unsigned rb_end{1};
  /// Resource block index jump.
  unsigned rb_stride{1};
  /// Resource element mask per resource block. True entries indicate the resource elements affected by the pattern.
  re_prb_mask re_mask{};
  /// Symbol mask. True entries indicate the symbols affected by the pattern.
  symbol_slot_mask symbols{};

  /// Default constructor. It allows instantiating the structure without using other constructors.
  re_pattern() = default;

  /// \brief Constructs an RE pattern from parameters.
  ///
  /// \param[in] rb_begin_  Start RB block index.
  /// \param[in] rb_end_    End RB block index (excluded).
  /// \param[in] rb_stride_ RB index jump.
  /// \param[in] re_mask_   RE Mask.
  /// \param[in] symbols_   Symbol mask.
  re_pattern(unsigned                rb_begin_,
             unsigned                rb_end_,
             unsigned                rb_stride_,
             const re_prb_mask&      re_mask_,
             const symbol_slot_mask& symbols_) :
    rb_begin(rb_begin_), rb_end(rb_end_), rb_stride(rb_stride_), re_mask(re_mask_), symbols(symbols_)
  {
    // Do nothing.
  }

  /// \brief Copy constructor.
  ///
  /// \param[in] other Provides the reference to other resource element pattern to copy.
  re_pattern(const re_pattern& other) :
    rb_begin(other.rb_begin),
    rb_end(other.rb_end),
    rb_stride(other.rb_stride),
    re_mask(other.re_mask),
    symbols(other.symbols)
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
};

} // namespace srsgnb
