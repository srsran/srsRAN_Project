
#ifndef SRSGNB_RESOURCE_GRID_H
#define SRSGNB_RESOURCE_GRID_H

#include "../sched_prb.h"
#include "cell_configuration.h"
#include "srsgnb/adt/circular_array.h"
#include "srsgnb/mac/sched_interface.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

using slot_index_t = unsigned;

/// Saves all the scheduled information relative to a {slot, cell}
struct cell_resource_grid {
  const cell_configuration& cfg;
  slot_index_t              slot_index;

  /// Stores the sum of all PRBs used for DL grants
  bwp_rb_bitmap dl_prbs;

  /// Stores the sum of all PRBs used for UL grants
  bwp_rb_bitmap ul_prbs;

  /// Saves DL grants allocated for the given slot and cell
  dl_sched_result dl_grants;

  /// Saves UL grants allocated for the given slot and cell
  ul_sched_result ul_grants;

  cell_resource_grid(const cell_configuration& cfg_, slot_index_t sl_idx);

  void reset();
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
    return slots_[sl_tx.to_uint() % RESOURCE_GRID_SIZE];
  }
  const cell_resource_grid& operator[](slot_point sl_tx) const
  {
    assert_valid_sl(sl_tx);
    return slots_[sl_tx.to_uint() % RESOURCE_GRID_SIZE];
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
  std::vector<cell_resource_grid> slots_;
};

/// Helper class that provides a generic interface to register scheduler grants in a specific {slot, cell}
/// The exposed methods are unsafe in the sense that they don't verify for grant collisions or invalid parameters. Such
/// checks will be performed by other allocator classes that wrap this one, and will have more specific use-cases
/// (e.g. UE vs SIB vs RAR allocators).
class slot_resource_allocator
{
public:
  slot_resource_allocator(cell_resource_grid& res_grid_, slot_point sl_pt_) : slot(sl_pt_), sl_res(res_grid_) {}

  const slot_point slot;

  bool is_dl_active() const { return true; }
  bool is_ul_active() const { return true; }

  const prb_bitmap& used_dl_prbs() const { return sl_res.dl_prbs.prbs(); }
  const prb_bitmap& used_ul_prbs() const { return sl_res.ul_prbs.prbs(); }

  dl_sched_result& dl_res() { return sl_res.dl_grants; }
  ul_sched_result& ul_res() { return sl_res.ul_grants; }

  /// Create a PUSCH grant and update the slot total UL PRB bitmap
  ul_sched_info& alloc_pusch(const prb_grant& prbs)
  {
    sl_res.ul_prbs |= prbs;
    sl_res.ul_grants.puschs.emplace_back();
    return sl_res.ul_grants.puschs.back();
  }

  rar_information& alloc_rar(const prb_grant& prbs)
  {
    sl_res.dl_prbs |= prbs;
    sl_res.dl_grants.rar_grants.emplace_back();
    return sl_res.dl_grants.rar_grants.back();
  }

private:
  cell_resource_grid& sl_res;
};

/// Provides an interface to access slot_resource_allocator objects for different slots of the same cell.
class cell_resource_allocator
{
public:
  cell_resource_allocator(cell_resource_grid_pool& res_grid_pool_, unsigned slot_offset = 0) :
    sl_offset(slot_offset), res_grid_pool(res_grid_pool_)
  {}

  /// Access resource allocator for a given {slot, cell}. Slot correspons to the latest slot indication provided
  /// to the resource grid + a slot delay provided as argument. Given that slot_delay is unsigned, this class
  /// can only access the present and future slots.
  slot_resource_allocator operator[](unsigned slot_delay)
  {
    return slot_resource_allocator{res_grid_pool[res_grid_pool.slot_tx() + sl_offset + slot_delay],
                                   res_grid_pool.slot_tx() + sl_offset + slot_delay};
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
