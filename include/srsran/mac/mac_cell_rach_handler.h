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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/prach/prach_constants.h"
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
  virtual ~mac_cell_rach_handler()                                         = default;
  virtual void handle_rach_indication(const mac_rach_indication& rach_ind) = 0;
};

} // namespace srsran
