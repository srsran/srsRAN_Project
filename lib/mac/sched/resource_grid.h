
#ifndef SRSGNB_RESOURCE_GRID_H
#define SRSGNB_RESOURCE_GRID_H

#include "sched_prb.h"
#include "srsgnb/adt/circular_array.h"
#include "srsgnb/mac/sched_interface.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

using slot_index_t     = unsigned;
const size_t TTIMOD_SZ = 32;

/// Saves all the scheduled information relative to a {slot, BWP}
struct slot_bwp_resource_context {
  /// Stores the sum of all PRBs used by allocated DL grants
  bwp_rb_bitmap dl_prbs;

  /// Stores the sum of all PRBs used by allocated UL grants
  bwp_rb_bitmap ul_prbs;
};

/// Saves all the scheduled information relative to a {slot, cell}
struct slot_resource_context {
  slot_index_t slot_index;

  /// Saves DL grants allocated for the given slot and cell
  dl_sched_result dl_grants;

  /// Saves UL grants allocated for the given slot and cell
  ul_sched_result ul_grants;

  /// Saves information relative to cell BWPs
  std::vector<slot_bwp_resource_context> bwps;
};

/// Saves all the scheduled data relative to a cell
struct cell_resource_grid {
  /// Access slot resources
  slot_resource_context&       operator[](slot_point sl) { return slots[sl.to_uint()]; }
  const slot_resource_context& operator[](slot_point sl) const { return slots[sl.to_uint()]; }

private:
  circular_array<slot_resource_context, TTIMOD_SZ> slots;
};

} // namespace srsgnb

#endif // SRSGNB_RESOURCE_GRID_H
