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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/prach/prach_constants.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

struct mac_rach_indication {
  struct rach_preamble {
    unsigned      index;
    phy_time_unit time_advance;
    float         power_dB;
    float         snr_dB;
  };

  struct rach_occasion {
    unsigned                                                       start_symbol;
    unsigned                                                       slot_index;
    unsigned                                                       frequency_index;
    float                                                          rssi_dB;
    static_vector<rach_preamble, MAX_PREAMBLES_PER_PRACH_OCCASION> preambles;
  };

  slot_point                                                 slot_rx;
  static_vector<rach_occasion, MAX_PRACH_OCCASIONS_PER_SLOT> occasions;
};

/// Interface used to handle RACH indications specific to a cell.
class mac_cell_rach_handler
{
public:
  virtual ~mac_cell_rach_handler() = default;

  /// Handles incoming RACH indication from the L1.
  ///
  /// \param rach_ind Received RACH indication.
  virtual void handle_rach_indication(const mac_rach_indication& rach_ind) = 0;
};

} // namespace srsran
