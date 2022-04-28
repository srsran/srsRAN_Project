
#ifndef SRSGNB_RESOURCE_GRID_H
#define SRSGNB_RESOURCE_GRID_H

#include "../sched_prb.h"
#include "cell_configuration.h"
#include "srsgnb/adt/circular_array.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/sched_interface.h"

namespace srsgnb {

using slot_index_t = unsigned;

/// Stores all the scheduled information relative to a {slot, cell}
struct cell_resource_grid {
  const cell_configuration& cfg;

  slot_point slot;

  /// Stores the sum of all PRBs used for DL grants
  prb_bitmap dl_prbs;

  /// Stores the sum of all PRBs used for UL grants
  prb_bitmap ul_prbs;

  /// Saves DL grants allocated for the given slot and cell
  dl_sched_result dl_grants;

  /// Saves UL grants allocated for the given slot and cell
  ul_sched_result ul_grants;

  explicit cell_resource_grid(const cell_configuration& cfg_);

  /// copies and moves are disabled to ensure pointer/reference validity
  cell_resource_grid(const cell_resource_grid&) = delete;
  cell_resource_grid(cell_resource_grid&&)      = delete;
  cell_resource_grid& operator=(const cell_resource_grid&) = delete;
  cell_resource_grid& operator=(cell_resource_grid&&) = delete;

  void slot_indication(slot_point sl);
  void reset();

  bool is_dl_active() const { return true; }
  bool is_ul_active() const { return true; }
};

/// Circular Ring of cell resource grid objects. This class manages the automatic resetting of cell resource
/// grid objects, once they become too old to be used by the PHY.
struct cell_resource_grid_pool {
  /// Number of cell resource grid objects stored in the pool
  static const size_t RESOURCE_GRID_SIZE = 40;

  /// Lowest difference between a slot and the last slot indication that avoids overflowing the ring pool.
  static const int MINIMUM_SLOT_DIFF = -10;

  /// Highest difference between a slot and the last slot indication that avoids overflowing the ring pool.
  static const int MAXIMUM_SLOT_DIFF = RESOURCE_GRID_SIZE / 2;

  /// Cell configuration
  const cell_configuration& cfg;

  explicit cell_resource_grid_pool(const cell_configuration& cfg_);

  /// Indicate the processing of a new slot in the scheduler.
  void slot_indication(slot_point sl_tx);

  /// Cell index of the resource grid.
  du_cell_index_t cell_index() const { return cfg.cell_index; }

  /// Last slot indicated to the scheduler.
  slot_point slot_tx() const { return last_slot_ind; }

  /// Access cell resource grid respective to given slot
  cell_resource_grid& operator[](slot_point sl_tx)
  {
    assert_valid_sl(sl_tx);
    cell_resource_grid& r = slots_[sl_tx.to_uint() % RESOURCE_GRID_SIZE];
    if (r.slot != sl_tx) {
      r.slot_indication(sl_tx);
    }
    return r;
  }
  const cell_resource_grid& operator[](slot_point sl_tx) const
  {
    assert_valid_sl(sl_tx);
    const cell_resource_grid& r = slots_[sl_tx.to_uint() % RESOURCE_GRID_SIZE];
    srsran_assert(r.slot == sl_tx, "Bad access to uninitialized cell_resource_grid");
    return r;
  }

private:
  /// Ensure we are not overflowing the circular pool
  void assert_valid_sl(slot_point sl) const
  {
    srsran_sanity_check(((sl - last_slot_ind) < MAXIMUM_SLOT_DIFF) and (sl - last_slot_ind) >= MINIMUM_SLOT_DIFF,
                        "The cell resource pool is too small for accessing a slot with delay: {}",
                        sl - last_slot_ind);
  }

  /// The latest slot value indicated by the PHY to the MAC/scheduler.
  slot_point last_slot_ind;

  /// Circular pool of cell resource grids, where each entry represents a separate slot.
  static_vector<cell_resource_grid, RESOURCE_GRID_SIZE> slots_;
};

/// Provides an interface to access slot_resource_allocator objects for different slots of the same cell.
class cell_resource_allocator
{
public:
  cell_resource_allocator(cell_resource_grid_pool& res_grid_pool_, unsigned slot_offset = 0) :
    sl_offset(slot_offset), res_grid_pool(res_grid_pool_)
  {}

  /// Access resource allocator for a given {slot, cell}. Slot corresponds to the latest slot indication provided
  /// to the resource grid + a slot delay provided as argument. Given that slot_delay is unsigned, this class
  /// can only access the present and future slots.
  cell_resource_grid& operator[](unsigned slot_delay)
  {
    return res_grid_pool[res_grid_pool.slot_tx() + sl_offset + slot_delay];
  }

private:
  /// Slot offset allows having multiple allocators pointing to the same grid, but pointing at a different default slot
  /// Note: This feature may become useful if we plan to allocate a batch of slots in one single scheduler call.
  const unsigned sl_offset;

  /// Reference to resource grid pool
  cell_resource_grid_pool& res_grid_pool;
};

} // namespace srsgnb

#endif // SRSGNB_RESOURCE_GRID_H
