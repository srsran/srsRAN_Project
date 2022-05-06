
#ifndef SRSGNB_RESOURCE_GRID_H
#define SRSGNB_RESOURCE_GRID_H

#include "../sched_prb.h"
#include "cell_configuration.h"
#include "srsgnb/adt/circular_array.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/mac_scheduler.h"

namespace srsgnb {

/// Stores all the scheduled information relative to a {slot, cell}.
struct cell_slot_resource_grid {
  const cell_configuration& cfg;

  /// Current slot that "this" slot resource grid refers to.
  slot_point slot;

  /// Stores the sum of all PRBs used for DL grants.
  prb_bitmap dl_prbs;

  /// Stores the sum of all PRBs used for UL grants.
  prb_bitmap ul_prbs;

  /// Saves grants allocated for the given slot and cell.
  sched_result result;

  explicit cell_slot_resource_grid(const cell_configuration& cfg_);

  /// copies and moves are disabled to ensure pointer/reference validity.
  cell_slot_resource_grid(const cell_slot_resource_grid&) = delete;
  cell_slot_resource_grid(cell_slot_resource_grid&&)      = delete;
  cell_slot_resource_grid& operator=(const cell_slot_resource_grid&) = delete;
  cell_slot_resource_grid& operator=(cell_slot_resource_grid&&) = delete;

  /// Sets new slot.
  void slot_indication(slot_point sl);

  /// Clears all allocations.
  void reset();
};

/// Circular Ring of cell_slot_resource_grid objects. This class manages the automatic resetting of
/// cell_slot_resource_grid objects, once they become old.
struct cell_resource_grid {
  /// Number of cell resource grid objects stored in the pool
  static const size_t RESOURCE_GRID_SIZE = 40;

  /// Highest difference between a slot and the last slot indication that avoids overflowing the ring pool.
  static const int MAXIMUM_SLOT_DIFF = RESOURCE_GRID_SIZE / 2;

  /// Cell configuration
  const cell_configuration& cfg;

  explicit cell_resource_grid(const cell_configuration& cfg_);

  /// Indicate the processing of a new slot in the scheduler.
  void slot_indication(slot_point sl_tx);

  /// Cell index of the resource grid.
  du_cell_index_t cell_index() const { return cfg.cell_index; }

  /// Last slot indicated to the scheduler.
  slot_point slot_tx() const { return last_slot_ind; }

  /// Access resource allocator for a given {slot, cell}. The accessed slot corresponds to the latest slot indication
  /// provided to the cell_resource_grid_manager + slot_delay provided as argument.
  /// Given that slot_delay is unsigned, this class can only access the present and future slots.
  /// \param slot_delay delay in #slots added to the last slot indication value.
  /// \return the cell resource grid corresponding to the accessed slot.
  const cell_slot_resource_grid& operator[](unsigned slot_delay) const
  {
    assert_valid_sl(slot_delay);
    slot_point                     sl_tx = last_slot_ind + slot_delay;
    const cell_slot_resource_grid& r     = slots[sl_tx.to_uint() % RESOURCE_GRID_SIZE];
    srsran_assert(r.slot == sl_tx, "Bad access to uninitialized cell_resource_grid");
    return r;
  }
  cell_slot_resource_grid& operator[](unsigned slot_delay)
  {
    assert_valid_sl(slot_delay);
    slot_point               sl_tx = last_slot_ind + slot_delay;
    cell_slot_resource_grid& r     = slots[sl_tx.to_uint() % RESOURCE_GRID_SIZE];
    srsran_assert(r.slot == sl_tx, "Bad access to uninitialized cell_resource_grid");
    return r;
  }

private:
  /// Ensure we are not overflowing the ring.
  void assert_valid_sl(unsigned slot_delay) const
  {
    srsran_sanity_check(slot_delay < MAXIMUM_SLOT_DIFF,
                        "The cell resource pool is too small for accessing a slot with delay: {}",
                        slot_delay);
  }

  /// The latest slot value indicated by the PHY to the MAC/scheduler.
  slot_point last_slot_ind;

  /// Circular pool of cell resource grids, where each entry represents a separate slot.
  static_vector<cell_slot_resource_grid, RESOURCE_GRID_SIZE> slots;
};

} // namespace srsgnb

#endif // SRSGNB_RESOURCE_GRID_H
