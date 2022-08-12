
#pragma once

#include "srsgnb/ran/phy_time_unit.h"

namespace srsgnb {

struct mac_rach_indication {
  static constexpr unsigned MAX_PREAMBLES = 64;
  static constexpr unsigned MAX_OCCASIONS = 4;

  struct rach_preamble {
    unsigned      index;
    phy_time_unit time_advance;
    float         power_dB;
    float         snr_dB;
  };
  struct rach_occasion {
    unsigned                                    start_symbol;
    unsigned                                    slot_index;
    unsigned                                    frequency_index;
    float                                       rssi_dB;
    static_vector<rach_preamble, MAX_PREAMBLES> preambles;
  };

  slot_point                                  slot_rx;
  static_vector<rach_occasion, MAX_OCCASIONS> occasions;
};

class mac_cell_rach_handler
{
public:
  virtual ~mac_cell_rach_handler()                                         = default;
  virtual void handle_rach_indication(const mac_rach_indication& rach_ind) = 0;
};

} // namespace srsgnb
