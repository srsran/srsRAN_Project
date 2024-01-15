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

#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"
#include "srsran/support/units.h"
#include <cstdint>

namespace srsran {

/// \brief UE configuration to monitor PDCCH for INT_RNTI (interruption).
///
/// The identifiers are inspired by TS38.331 Section as 6.3.2 (see field \e DownlinkPreemption).
struct downlink_preemption {
  /// \brief Set selection for DL-preemption indication.
  /// \remark See TS 38.213, clause 11.2.
  enum class time_frequency_set { set0, set1 };

  /// \brief Indication of 14 bit INT values inside DCI payload.
  /// \remark See TS 38.213, clause 11.2.
  struct int_configuration_per_serving_cell {
    du_cell_index_t serving_cell_id;
    /// Starting position (in bits) of the 14 bit INT value applicable for this serving cell within the DCI payload.
    /// Must be multiples of 14 (bit). See TS 38.213, clause 11.2.
    /// Values {0..maxINT-DCI-PayloadSize-1}, where maxINT-DCI-PayloadSize = 126.
    unsigned position_in_dci;

    bool operator==(const int_configuration_per_serving_cell& rhs) const
    {
      return std::tie(serving_cell_id, position_in_dci) == std::tie(rhs.serving_cell_id, rhs.position_in_dci);
    }

    bool operator!=(const int_configuration_per_serving_cell& rhs) const { return !(rhs == *this); }
  };

  /// RNTI used for indication pre-emption in DL.
  rnti_t int_rnti;
  /// Determines how the UE interprets the DL preemption DCI payload.
  time_frequency_set time_freq_set;
  /// Total length of the DCI payload scrambled with INT-RNTI. Values {0..maxINT-DCI-PayloadSize}, where
  /// maxINT-DCI-PayloadSize = 126.
  units::bits dci_payload_size;
  /// Indicates (per serving cell) the position of the 14 bit INT values inside the DCI payload.
  static_vector<int_configuration_per_serving_cell, MAX_NOF_DU_CELLS> int_cfg;

  bool operator==(const downlink_preemption& rhs) const
  {
    return std::tie(int_rnti, time_freq_set, dci_payload_size, int_cfg) ==
           std::tie(rhs.int_rnti, rhs.time_freq_set, rhs.dci_payload_size, rhs.int_cfg);
  }

  bool operator!=(const downlink_preemption& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
