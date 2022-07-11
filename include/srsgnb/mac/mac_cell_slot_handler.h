
#pragma once

#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Interface used to handle slot indications specific to a cell.
class mac_cell_slot_handler
{
public:
  virtual ~mac_cell_slot_handler()                      = default;
  virtual void handle_slot_indication(slot_point sl_tx) = 0;
};

} // namespace srsgnb
