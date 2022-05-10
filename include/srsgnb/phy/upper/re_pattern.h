/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_RE_PATTERN_H
#define SRSGNB_PHY_UPPER_RE_PATTERN_H

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/cyclic_prefix.h"

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
  std::array<bool, NRE> re_mask{};
  /// Symbol mask. True entries indicate the symbols affected by the pattern.
  std::array<bool, MAX_NSYMB_PER_SLOT> symbols{};

  /// Default constructor. It allows instantiating the structure without using other constructors.
  re_pattern() = default;

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
  void get_inclusion_mask(span<bool> mask, unsigned symbol) const;

  /// \brief Exclude the described resource element pattern in a resource grid symbol mask.
  ///
  /// This method sets to false the elements that are described in the pattern for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask Provides a mask representing an entire symbol in a resource grid.
  /// \param[in] symbol Indicates the symbol index for the mask to be excluded.
  /// \note This method expects that the number of elements of \c mask is equal to or greater than \c rb_end
  void get_exclusion_mask(span<bool> mask, unsigned symbol) const;
};

/// Describes a resource element pattern list.
class re_pattern_list
{
private:
  /// Defines the maximum number or RE pattern a list can carry.
  static constexpr unsigned MAX_RE_PATTERN = 4;

  /// Resource element pattern storage.
  static_vector<re_pattern, MAX_RE_PATTERN> list = {};

public:
  /// Default constructor.
  re_pattern_list() = default;

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
  bool operator==(re_pattern_list& other) const;

  /// \brief Checks if the pattern list is the pattern list is different from another pattern list.
  ///
  /// \param[in] other The other pattern list.
  /// \return True if the patterns described by the two lists are different.
  bool operator!=(re_pattern_list& other) const { return !(*this == other); }

  /// \brief Includes the described resource element pattern list in a resource grid symbol mask.
  ///
  /// This method sets to true the elements that are described in the pattern list for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask   Provides a mask representing an entire symbol in a resource grid.
  /// \param[in]     symbol Indicates the symbol index for the mask to be included.
  /// \note This method expects that the number of elements of \c mask is equal to or greater than \c rb_end.
  void get_inclusion_mask(span<bool> mask, unsigned symbol) const;

  /// \brief Counts the number of elements included in the described pattern.
  ///
  /// This method counts the included elements in the described pattern for a symbol range and a selection of resource
  /// blocks.
  ///
  /// \param[in] start_symbol Indicates the start symbol index within the slot.
  /// \param[in] nof_symbols  Indicates the number of symbols within the slot.
  /// \param[in] prb_mask     Selection of physical resource blocks to count.
  /// \return The number of included elements.
  unsigned get_inclusion_count(unsigned start_symbol, unsigned nof_symbols, span<const bool> prb_mask) const;

  /// \brief Excludes the described resource element pattern list in a resource grid symbol mask.
  ///
  /// This method sets to false the elements that are described in the pattern list for a given symbol index. The mask
  /// represents resource elements allocation for a given symbol in a resource grid.
  ///
  /// \param[in,out] mask   Provides a mask representing an entire symbol in a resource grid.
  /// \param[in]     symbol Indicates the symbol index for the mask to be excluded.
  /// \note This method expects that the number of elements of \c mask is equal to or greater than \c rb_end.
  void get_exclusion_mask(span<bool> mask, unsigned symbol) const;
};

} // namespace srsgnb
#endif // SRSGNB_PHY_UPPER_RE_PATTERN_H
