/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

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

  struct reader_deleter {
    reader_deleter() = default;
    reader_deleter(std::atomic<uint32_t>& lock_) : lock(&lock_) {}
    void operator()(slot_decision_history* ptr) const
    {
      if (ptr != nullptr) {
        auto prev = lock->fetch_sub(2, std::memory_order_release);
        srsran_sanity_check((prev & 1) == 0, "Invalid lock state in reader deleter");
        srsran_sanity_check(prev > 0, "Invalid lock state in reader deleter");
      }
    }

    std::atomic<uint32_t>* lock = nullptr;
  };

  struct writer_deleter {
    writer_deleter() = default;
    writer_deleter(std::atomic<uint32_t>& lock_) : lock(&lock_) {}
    void operator()(slot_decision_history* ptr) const
    {
      if (ptr != nullptr) {
        auto prev = lock->exchange(0, std::memory_order_release);
        srsran_sanity_check(prev == 1, "Invalid lock state in writer deleter");
      }
    }

    std::atomic<uint32_t>* lock = nullptr;
  };

  using writer = std::unique_ptr<slot_decision_history, writer_deleter>;
  using reader = std::unique_ptr<slot_decision_history, reader_deleter>;

  mac_test_mode_cell_decision_history(const mac_cell_creation_request& cell_cfg) :
    sched_decision_history(get_ring_size(cell_cfg))
  {
  }

  reader read(slot_point sl)
  {
    auto& entry = sched_decision_history[get_ring_idx(sl)];
    auto  r     = entry.lock_read();
    if (r != nullptr and r->slot != sl) {
      r.reset();
    }
    return r;
  }

  writer write(slot_point sl)
  {
    auto& entry = sched_decision_history[get_ring_idx(sl)];
    auto  w     = entry.lock_write();
    if (w != nullptr) {
      w->slot = sl;
      w->pucchs.clear();
      w->puschs.clear();
    }
    return w;
  }

  void clear(slot_point sl)
  {
    auto w = write(sl);
    if (w != nullptr) {
      w->slot = {};
      w->puschs.clear();
      w->pucchs.clear();
    }
  }

private:
  struct locked_slot_context {
    std::atomic<uint32_t> lock{0};
    slot_decision_history decision;

    writer lock_write()
    {
      // Lock if there is no concurrent read or write.
      uint32_t expected = 0;
      if (lock.compare_exchange_strong(expected, 1, std::memory_order_acq_rel, std::memory_order_relaxed)) {
        return writer{&decision, writer_deleter(lock)};
      }
      return writer{};
    }

    reader lock_read()
    {
      uint32_t expected = lock.load(std::memory_order_relaxed);
      while ((expected & 1) == 0) {
        // While there is no concurrent write.
        if (lock.compare_exchange_weak(expected, expected + 2, std::memory_order_acq_rel, std::memory_order_relaxed)) {
          return reader{&decision, reader_deleter(lock)};
        }
      }
      return reader{};
    }
  };

  static size_t get_ring_size(const mac_cell_creation_request& cell_cfg)
  {
    // Estimation of the time it takes the UL lower-layers to process and forward CRC/UCI indications.
    static constexpr unsigned MAX_UL_PHY_DELAY = 80;
    // Note: The history ring size has to be a multiple of the TDD frame size in slots.
    // Number of slots managed by this container.
    return get_allocator_ring_size_gt_min(get_max_slot_ul_alloc_delay(cell_cfg.sched_req.ntn_cs_koffset) +
                                          MAX_UL_PHY_DELAY);
  }

  size_t get_ring_idx(slot_point sl) const { return sl.to_uint() % sched_decision_history.size(); }

  /// Ring buffer of slot decision history.
  std::vector<locked_slot_context> sched_decision_history;
};

} // namespace srsran
