/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/bounded_integer.h"
#include "srsran/ran/srs/srs_configuration.h"
#include <optional>
#include <variant>

namespace srsran {

/// \brief Options for the number of Initial Cyclic Shifts that can be set for PUCCH Format 1.
///
/// Defines the number of different Initial Cyclic Shifts that can be used for PUCCH Format 1, as per \c PUCCH-format1,
/// in \c PUCCH-Config, TS 38.331. We assume the CS are evenly distributed, which means we can only have a divisor of 12
/// possible cyclic shifts.
enum class nof_cyclic_shifts { no_cyclic_shift = 1, two = 2, three = 3, four = 4, six = 6, twelve = 12 };

struct srs_builder_params {
  /// If present, defines the SRS period for SRS periodic resources, in slots.
  /// When not present, the SRS resources are set as aperiodic.
  std::optional<srs_periodicity> srs_period = std::nullopt;
  /// Maximum number of symbols per UL slot dedicated for SRS resources.
  /// \remark In case of Sounding Reference Signals (SRS) being used, the number of symbols should be reduced so that
  /// the PUCCH resources do not overlap in symbols with the SRS resources.
  /// \remark The SRS resources are always placed at the end of the slot.
  /// \remark As per TS 38.211, Section 6.4.1.4.1, SRS resource can only be placed in the last 6 symbols of a slot.
  bounded_integer<unsigned, 1, 6> max_nof_symbols = 2U;
  /// \c Transmission comb number, as per TS 38.211, Section 6.4.1.4.2, or TS 38.331, "SRS-Resource".
  tx_comb_size tx_comb = tx_comb_size::n4;
  /// Defines the number of symbols per SRS resource.
  srs_nof_symbols nof_symbols = srs_nof_symbols::n1;
  /// Defines the CS reuse factor for the SRS resources.
  /// \remark With 2 or 4 antenna ports, different cyclic shifts are used by the different antennas. This parameter
  /// defines how many UEs can be multiplexed in the same symbols and RBs by exploiting different cyclic shifts.
  /// Values: {no_cyclic_shift, two, three, four, six} for 2 UL antenna ports.
  /// Values: {no_cyclic_shift, three} for 4 UL antenna ports.
  nof_cyclic_shifts cyclic_shift_reuse_factor = nof_cyclic_shifts::no_cyclic_shift;
  /// Defines the reuse of the SRS sequence ID for different UEs within the same cell.
  /// \remark The goal of the SRS sequence ID would be to reduce the inter-cell interference. However, if the cell is
  /// not in a dense multi-cell environment, we can reuse different sequence ID for different cell UEs.
  /// Values: {1, 2, 3, 5, 6, 10, 15, 30}.
  unsigned sequence_id_reuse_factor = 1;
  /// \c p0, TS 38.331. Value in dBm. Only even values allowed within {-202,...,24}.
  int p0 = -84;
};

} // namespace srsran
