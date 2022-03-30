
#ifndef SRSGNB_MAC_RESOURCE_MANAGER_H
#define SRSGNB_MAC_RESOURCE_MANAGER_H

#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Cell Configuration
struct mac_cell_configuration {
  du_cell_index_t cell_index;
  // TODO
};

/// Start configured cell.
struct mac_cell_start {
  du_cell_index_t cell_index;
  slot_point      slot;
};

/// Network Slice Configuration
struct mac_slice_configuration {
  /// TODO
};

/// Class used to setup the MAC cells and slices.
class mac_resource_manager
{
public:
  virtual ~mac_resource_manager()                               = default;
  virtual void add_cell(const mac_cell_configuration& cell_cfg) = 0;
  virtual void remove_cell(du_cell_index_t cell_index)          = 0;
  virtual void start_cell(du_cell_index_t cell_index)           = 0;
  virtual void stop_cell(du_cell_index_t cell_index)            = 0;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_RESOURCE_MANAGER_H
