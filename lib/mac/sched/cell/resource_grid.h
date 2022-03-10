
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

/// Circular pool of resource grids
struct cell_resource_grid_pool {
  static const size_t RESOURCE_GRID_SIZE = 40;

  const cell_configuration& cfg;

  explicit cell_resource_grid_pool(const cell_configuration& cfg_);

  void slot_indication(slot_point sl_tx);

  du_cell_index_t cell_index() const { return cfg.cell_index; }
  slot_point      slot_tx() const { return last_sl_ind; }

  cell_resource_grid& operator[](unsigned sl_delay)
  {
    srsran_sanity_check(sl_delay < RESOURCE_GRID_SIZE / 2, "The cell resource pool is too small for provided delay");
    return slots_[(last_sl_ind + sl_delay).to_uint() % RESOURCE_GRID_SIZE];
  }
  const cell_resource_grid& operator[](unsigned sl_delay) const
  {
    srsran_sanity_check(sl_delay < RESOURCE_GRID_SIZE / 2, "The cell resource pool is too small for provided delay");
    return slots_[(last_sl_ind + sl_delay).to_uint() % RESOURCE_GRID_SIZE];
  }

private:
  slot_point                      last_sl_ind;
  std::vector<cell_resource_grid> slots_;
};

class slot_resource_allocator
{
public:
  slot_resource_allocator(cell_resource_grid& res_grid_, slot_point sl_pt_, du_bwp_id_t bwp_id_) :
    slot(sl_pt_), bwp_id(bwp_id_), sl_res(res_grid_)
  {}

  const slot_point  slot;
  const du_bwp_id_t bwp_id;

  bool is_dl_active() const { return true; }
  bool is_ul_active() const { return true; }

  const prb_bitmap& used_dl_prbs() const { return sl_res.dl_prbs.prbs(); }
  const prb_bitmap& used_ul_prbs() const { return sl_res.ul_prbs.prbs(); }

  dl_sched_result& dl_res() { return sl_res.dl_grants; }
  ul_sched_result& ul_res() { return sl_res.ul_grants; }

  ul_sched_info& alloc_pusch(const prb_grant& prbs)
  {
    sl_res.ul_prbs |= prbs;
    sl_res.ul_grants.puschs.emplace_back();
    return sl_res.ul_grants.puschs.back();
  }

private:
  cell_resource_grid& sl_res;
};

class cell_resource_allocator
{
public:
  cell_resource_allocator(cell_resource_grid_pool& res_grid_pool_, du_bwp_id_t bwp_id_, unsigned slot_offset = 0) :
    sl_offset(slot_offset), bwp_id(bwp_id_), res_grid_pool(res_grid_pool_)
  {}

  /// Access resource allocator for a given {slot, cell}
  slot_resource_allocator operator[](unsigned sl_delay)
  {
    return slot_resource_allocator{
        res_grid_pool[sl_offset + sl_delay], res_grid_pool.slot_tx() + sl_offset + sl_delay, bwp_id};
  }

private:
  const unsigned           sl_offset;
  const du_bwp_id_t        bwp_id;
  cell_resource_grid_pool& res_grid_pool;
};

} // namespace srsgnb

#endif // SRSGNB_RESOURCE_GRID_H
