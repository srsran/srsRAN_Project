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
};

} // namespace srsran