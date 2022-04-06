
#ifndef SRSGNB_MAC_MAC_RESOURCE_MANAGER_H
#define SRSGNB_MAC_MAC_RESOURCE_MANAGER_H

#include "cell_configuration.h"
#include "srsgnb/ran/du_types.h"

namespace srsgnb {

/// Start configured cell.
struct mac_cell_start {
  du_cell_index_t cell_index;
  slot_point      slot;
};

/// Network slice configuration
struct mac_slice_configuration {
  // TODO: Fill remaining fields
};

/// Class used to setup the MAC cells and slices.
class mac_resource_manager
{
public:
  virtual ~mac_resource_manager()                               = default;
  virtual void add_cell(const mac_cell_configuration& cell_cfg) = 0;
  virtual void remove_cell(du_cell_index_t cell_index)          = 0;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_MAC_RESOURCE_MANAGER_H
