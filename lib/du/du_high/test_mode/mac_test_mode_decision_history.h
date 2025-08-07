/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/lockfree_triple_buffer.h"
#include "srsran/adt/static_vector.h"
#include "srsran/mac/cell_configuration.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/scheduler/resource_grid_util.h"
#include "srsran/scheduler/result/pucch_info.h"
#include "srsran/scheduler/result/pusch_info.h"
#include <vector>

namespace srsran {

/// Stores the decision history of the MAC cell when in test mode. This is an SPSC class.
class mac_test_mode_cell_decision_history
{
public:
  struct slot_decision_history {
    slot_point                                            slot;
    static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>    pucchs;
    static_vector<ul_sched_info, MAX_PUSCH_PDUS_PER_SLOT> puschs;
  };

  mac_test_mode_cell_decision_history(const mac_cell_creation_request& cell_cfg) :
    sched_decision_history(get_ring_size(cell_cfg))
  {
  }

  const slot_decision_history* read(slot_point sl)
  {
    auto& entry = sched_decision_history[get_ring_idx(sl)].read();
    if (entry.slot == sl) {
      return &entry;
    }
    return nullptr;
  }

  slot_decision_history& write(slot_point sl)
  {
    auto& entry = sched_decision_history[get_ring_idx(sl)].write();
    entry.slot  = sl;

    // Resets the history for this ring element.
    entry.pucchs.clear();
    entry.puschs.clear();

    return entry;
  }

  void commit(slot_point sl) { sched_decision_history[get_ring_idx(sl)].commit(); }

  /// Called on error indication from the writer side to clear the history entry for the given slot.
  void clear(slot_point sl)
  {
    auto& entry = write(sl);
    entry.slot  = {};
    commit(sl);
  }

private:
  static size_t get_ring_size(const mac_cell_creation_request& cell_cfg)
  {
    // Estimation of the time it takes the UL lower-layers to process and forward CRC/UCI indications.
    static constexpr unsigned MAX_UL_PHY_DELAY = 40;
    // Note: The history ring size has to be a multiple of the TDD frame size in slots.
    // Number of slots managed by this container.
    return get_allocator_ring_size_gt_min(get_max_slot_ul_alloc_delay(cell_cfg.sched_req.ntn_cs_koffset) +
                                          MAX_UL_PHY_DELAY);
  }

  size_t get_ring_idx(slot_point sl) const { return sl.to_uint() % sched_decision_history.size(); }

  /// Ring buffer of slot decision history.
  std::vector<lockfree_triple_buffer<slot_decision_history>> sched_decision_history;
};

} // namespace srsran
