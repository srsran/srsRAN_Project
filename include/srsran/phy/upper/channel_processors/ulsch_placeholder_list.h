/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/modulation_scheme.h"
#include "srsran/ran/pusch/pusch_constants.h"

namespace srsran {

/// \brief Abstracts a list of UL-SCH repetition placeholders \f$y\f$.
///
/// The process of descrambling reverses the effect of the repetition placeholders \f$y\f$ when one bit of information
/// is encoded in one of the Uplink Control Information (UCI) fields. The encoding is described in TS38.212
/// Section 5.3.3.1 and the scrambling in TS38.211 Section 6.3.1.1.
class ulsch_placeholder_list
{
  /// Indexing data type. Unsigned 16-bit integer is sufficient for the maximum resource grid size considering a maximum
  /// of \ref pusch_constants::MAX_NRE_PER_RB resource elements for \ref MAX_RB resource blocks.
  using index_type = uint16_t;

public:
  /// The maximum number of RE containing repetition placeholders is equal to the maximum number of RE used for a PUSCH
  /// transmission.
  static constexpr unsigned MAX_NOF_PLACEHOLDERS = pusch_constants::MAX_NRE_PER_RB * MAX_RB;

  /// Default constructor: builds an empty placeholder list.
  ulsch_placeholder_list() = default;

  /// Constructor: Initializes the list of RE indexes with the provided values.
  ulsch_placeholder_list(const std::initializer_list<unsigned>& list)
  {
    std::copy_n(list.begin(), list.size(), std::back_inserter(re_indexes));
  }

  /// Appends a resource element index that contains a repetition placeholder.
  void push_back(unsigned re_index)
  {
    // Append an RE index to the list.
    re_indexes.emplace_back(static_cast<index_type>(re_index));
  }

  /// \brief Applies the function \c func() to every repetition placeholder.
  ///
  /// The bit index is passed as an argument to the function.
  ///
  /// \tparam Func     Lambda function to process \f$x\f$ and \f$y\f$ placeholders.
  /// \param func[in]  Lambda function to apply for each placeholder.
  /// \remark The function \c Func shall have the signature <tt>void ()(unsigned, unsigned)</tt>, where:
  /// - The first argument is the bit position of a \f$y\f$ placeholder; and
  /// - The second argument is the number of \f$x\f$ placeholders that follow.
  template <typename Func>
  void for_each(modulation_scheme modulation, unsigned nof_layers, Func&& func) const
  {
    static_assert(std::is_convertible<Func, std::function<void(unsigned, unsigned)>>::value,
                  "The function signature must be \"void () (unsigned, unsigned)\"");
    unsigned bits_per_symbol = get_bits_per_symbol(modulation);

    // According to TS38.212 Tables 5.3.3.1-1, and 5.3.3.2-1, x placeholders are placed after the first two bits for
    // modulation orders larger than 2.
    unsigned nof_trailing_x_placeholders = (bits_per_symbol > 2) ? (bits_per_symbol - 2) : 0;

    // For each RE index in the list...
    for (uint16_t i_re : re_indexes) {
      // For each layer...
      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        // Calculate the soft bit index that contains a placeholder within the RE as described in TS38.212
        // Table 5.3.3.1-1, where the repetition placeholder `y` is placed always on the second bit.
        unsigned i_bit = bits_per_symbol * (nof_layers * static_cast<unsigned>(i_re) + i_layer) + 1;

        // Call the lambda function.
        func(i_bit, nof_trailing_x_placeholders);
      }
    }
  }

  /// \brief Compares two placeholder lists.
  /// \return Returns true if the two lists are equal to one another.
  bool operator==(const ulsch_placeholder_list& other) const noexcept { return re_indexes == other.re_indexes; }

  /// \brief Compares two placeholder lists.
  /// \return Returns true if the two lists are different.
  bool operator!=(const ulsch_placeholder_list& other) const noexcept { return re_indexes != other.re_indexes; }

private:
  /// List of RE indexes.
  static_vector<index_type, MAX_NOF_PLACEHOLDERS> re_indexes;
};

} // namespace srsran
