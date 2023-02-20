/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_rnti_table.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/scheduler_configurator.h"

namespace srsran {

class rnti_allocator
{
public:
  explicit rnti_allocator(du_rnti_table& rnti_table_) : rnti_table(rnti_table_) {}

  /// Handles detected PRACHs by allocating a temporary C-RNTI and signalling the scheduler to allocate an RAR.
  rnti_t allocate()
  {
    if (rnti_table.nof_ues() >= MAX_NOF_DU_UES) {
      // If the number of UEs is already maximum, ignore RACH.
      return rnti_t::INVALID_RNTI;
    }
    // Increments rnti counter until it finds an available temp C-RNTI.
    rnti_t temp_crnti;
    do {
      uint16_t prev_counter = rnti_counter.fetch_add(1, std::memory_order_relaxed) % CRNTI_RANGE;
      temp_crnti            = to_rnti(prev_counter + rnti_t::MIN_CRNTI);
    } while (rnti_table.has_rnti(temp_crnti));
    return temp_crnti;
  }

private:
  static constexpr int    CRNTI_RANGE  = rnti_t::MAX_CRNTI + 1 - rnti_t::MIN_CRNTI;
  static constexpr rnti_t INITIAL_RNTI = to_rnti(0x4601);

  du_rnti_table& rnti_table;

  std::atomic<std::underlying_type_t<rnti_t>> rnti_counter{INITIAL_RNTI - MIN_CRNTI};
};

/// \brief Handles the allocation of RNTIs and forwarding of RACH indications to scheduler.
/// This class is thread-safe.
class cell_rach_handler final : public mac_cell_rach_handler
{
public:
  explicit cell_rach_handler(du_cell_index_t cell_index_, scheduler_configurator& sched_, rnti_allocator& rnti_alloc_) :
    logger(srslog::fetch_basic_logger("MAC")), cell_index(cell_index_), rnti_alloc(rnti_alloc_), sched(sched_)
  {
  }

  /// Handles detected PRACHs by allocating a temporary C-RNTI and signalling the scheduler to allocate an RAR.
  void handle_rach_indication(const mac_rach_indication& rach_ind) override
  {
    rach_indication_message sched_rach{};
    sched_rach.cell_index = cell_index;
    sched_rach.slot_rx    = rach_ind.slot_rx;
    for (const auto& occasion : rach_ind.occasions) {
      auto& sched_occasion           = sched_rach.occasions.emplace_back();
      sched_occasion.start_symbol    = occasion.start_symbol;
      sched_occasion.frequency_index = occasion.frequency_index;
      for (const auto& preamble : occasion.preambles) {
        rnti_t alloc_tc_rnti = rnti_alloc.allocate();
        if (alloc_tc_rnti == rnti_t::INVALID_RNTI) {
          logger.warning(
              "Ignoring PRACH, cell={} preamble id={}. Cause: Failed to allocate TC-RNTI.", cell_index, preamble.index);
          continue;
        }
        auto& sched_preamble        = sched_occasion.preambles.emplace_back();
        sched_preamble.preamble_id  = preamble.index;
        sched_preamble.tc_rnti      = alloc_tc_rnti;
        sched_preamble.time_advance = preamble.time_advance;
      }
      if (sched_occasion.preambles.empty()) {
        // No preamble was added. Remove occasion.
        sched_rach.occasions.pop_back();
      }
    }

    notify_sched(sched_rach);
  }

private:
  static constexpr int    CRNTI_RANGE  = rnti_t::MAX_CRNTI + 1 - rnti_t::MIN_CRNTI;
  static constexpr rnti_t INITIAL_RNTI = to_rnti(0x4601);

  /// Notify Scheduler of handled RACH indication.
  void notify_sched(const rach_indication_message& rach_ind) { sched.handle_rach_indication(rach_ind); }

  srslog::basic_logger&   logger;
  const du_cell_index_t   cell_index;
  rnti_allocator&         rnti_alloc;
  scheduler_configurator& sched;

  std::atomic<std::underlying_type_t<rnti_t>> rnti_counter{INITIAL_RNTI - MIN_CRNTI};
};

class rach_handler : public rach_handler_configurator
{
public:
  rach_handler(scheduler_configurator& sched_, du_rnti_table& rnti_table_) : sched(sched_), rnti_alloc(rnti_table_) {}

  void add_cell(du_cell_index_t cell_index) override
  {
    srsgnb_assert(not cells.contains(cell_index), "Cell {} already exists", cell_index);
    cells.emplace(cell_index, cell_index, sched, rnti_alloc);
  }

  void remove_cell(du_cell_index_t cell_index) override
  {
    srsgnb_assert(cells.contains(cell_index), "Cell {} already exists", cell_index);
    cells.erase(cell_index);
  }

  mac_cell_rach_handler& get_cell(du_cell_index_t cell_index)
  {
    srsgnb_assert(cells.contains(cell_index), "Cell index does not exist.");
    return cells[cell_index];
  }

private:
  scheduler_configurator&                            sched;
  rnti_allocator                                     rnti_alloc;
  slotted_array<cell_rach_handler, MAX_NOF_DU_CELLS> cells;
};

} // namespace srsran
