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

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/ran/prach/prach_constants.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

struct mac_rach_indication {
  struct rach_preamble {
    unsigned      index;
    phy_time_unit time_advance;
    float         power_dB;
    float         snr_dB;
  };

  struct rach_occasion {
    unsigned                                                         start_symbol;
    unsigned                                                         slot_index;
    unsigned                                                         frequency_index;
    float                                                            rssi_dB;
    static_vector<rach_preamble, prach_constants::MAX_NUM_PREAMBLES> preambles;
  };

  slot_point                                                         slot_rx;
  static_vector<rach_occasion, prach_constants::MAX_PRACH_OCCASIONS> occasions;
};

/// Interface used to handle RACH indications specific to a cell.
class mac_cell_rach_handler
{
public:
  virtual ~mac_cell_rach_handler()                                         = default;
  virtual void handle_rach_indication(const mac_rach_indication& rach_ind) = 0;
};

} // namespace srsgnb
