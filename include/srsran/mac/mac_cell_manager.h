
#pragma once

#include "cell_configuration.h"
#include "mac_ue_configurator.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Start configured cell.
struct mac_cell_start {
  du_cell_index_t cell_index;
  slot_point      slot;
};

/// Network slice configuration
struct mac_slice_configuration {
  // TODO: Fill remaining fields
};

/// Interface used to handle a MAC cell activation/deactivation.
class mac_cell_controller
{
public:
  virtual ~mac_cell_controller()   = default;
  virtual async_task<void> start() = 0;
  virtual async_task<void> stop()  = 0;
};

/// Class used to setup the MAC cells and slices.
class mac_cell_manager
{
public:
  virtual ~mac_cell_manager()                                                      = default;
  virtual void                 add_cell(const mac_cell_creation_request& cell_cfg) = 0;
  virtual void                 remove_cell(du_cell_index_t cell_index)             = 0;
  virtual mac_cell_controller& get_cell_controller(du_cell_index_t cell_index)     = 0;
};

} // namespace srsran
