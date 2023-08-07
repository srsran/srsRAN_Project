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
    /// \brief Maps the concatenation of the parameters to Part 2 size.
    static_vector<uint16_t, max_size_table> map;
  };

  /// CSI Part 2 entries.
  static_vector<entry, max_nof_entries> entries;
};

} // namespace srsran