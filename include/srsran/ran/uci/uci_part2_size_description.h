/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"

namespace srsran {

/// Collects the parameters that describe the UCI Part 1 correspondence to Part 2 sizes.
struct uci_part2_size_description {
  /// Maximum number of parameters.
  static constexpr unsigned max_nof_parameters = 2;
  /// Maximum number of Part 2 entries.
  static constexpr unsigned max_nof_entries = 2;
  /// Maximum number of aggregated bits per entry.
  static constexpr unsigned max_nof_entry_bits = 4;
  /// Maximum number of Part 2 sizes per entry.
  static constexpr unsigned max_size_table = 1U << max_nof_entry_bits;

  uci_part2_size_description() = default;

  /// \brief Creates a fixed-size UCI Part 2.
  ///
  /// A UCI Part 1 correspondence to UCI Part 2 size that has one possible size independently of the UCI Part 1
  /// contents.
  uci_part2_size_description(unsigned fixed_size)
  {
    // Clear entries if no UCI Part 2 payload.
    if (fixed_size == 0) {
      entries.clear();
      return;
    }

    // Create new size entry.
    entry& e = entries.emplace_back();

    // Create zero bit width parameter.
    parameter& p = e.parameters.emplace_back();
    p.width      = 0;
    p.offset     = 0;

    // Add size to table.
    e.map.emplace_back(fixed_size);
  }

  /// Collects parameter attributes.
  struct parameter {
    /// Bit offset of the parameter from the beginning of the Part 1.
    uint16_t offset;
    /// CSI Part 1 parameter bit width.
    uint8_t width;
  };

  /// Collects the parameters to determine a single CSI Part 2 report.
  struct entry {
    /// \brief Part 1 parameters that influence the size of this part 2.
    /// \remark The total accumulated width of the parameters must not exceed \ref max_nof_entry_bits bits.
    static_vector<parameter, max_nof_parameters> parameters;
    /// \brief Maps the concatenation of the parameters to Part 2 size in bits.
    static_vector<uint16_t, max_size_table> map;
  };

  /// CSI Part 2 entries.
  static_vector<entry, max_nof_entries> entries;

  /// Checks if the UCI part 2 size description is consistent with the CSI Part 1 reports sizes.
  bool is_valid(unsigned uci_part1_nof_bits) const
  {
    // For each entry...
    for (const auto& entry_ : entries) {
      unsigned param_size = 0;

      // For each parameter...
      for (const auto& param : entry_.parameters) {
        // Accumulate the parameter width.
        param_size += param.width;

        // Make sure the parameter offset and width does not exceed the size of CSI Part 1.
        if ((param.width + param.offset) >= uci_part1_nof_bits) {
          return false;
        }
      }

      // Make sure the entry table size corresponds to the number of bits.
      if (1U << param_size != entry_.map.size()) {
        return false;
      }
    }

    // The description must be valid if reached here.
    return true;
  }
};

} // namespace srsran
