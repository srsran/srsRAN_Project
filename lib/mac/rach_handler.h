/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
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

namespace srsgnb {

class rnti_allocator
{
public:
  explicit rnti_allocator(du_rnti_table& rnti_table_) : rnti_table(rnti_table_) {}

  /// Handles detected PRACHs by allocating a temporary C-RNTI and signalling the scheduler to allocate an RAR.
  rnti_t allocate()
  {
    if (rnti_table.nof_ues() >= MAX_NOF_DU_UES) {
      // If the number of UEs is already maximum, ignore RACH.
      return INITIAL_RNTI;
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
    rnti_t alloc_temp_crnti = rnti_alloc.allocate();
    if (alloc_temp_crnti != INITIAL_RNTI) {
      notify_sched(rach_ind, alloc_temp_crnti);
    }
  }

private:
  static constexpr int    CRNTI_RANGE  = rnti_t::MAX_CRNTI + 1 - rnti_t::MIN_CRNTI;
  static constexpr rnti_t INITIAL_RNTI = to_rnti(0x4601);

  /// Notify Scheduler of handled RACH indication.
  void notify_sched(const mac_rach_indication& rach_ind, rnti_t rnti)
  {
    rach_indication_message sched_rach{};
    sched_rach.cell_index      = cell_index;
    sched_rach.timing_info     = 0; // TODO
    sched_rach.slot_rx         = rach_ind.slot_rx;
    sched_rach.symbol_index    = rach_ind.symbol_index;
    sched_rach.frequency_index = rach_ind.frequency_index;
    sched_rach.preamble_id     = rach_ind.preamble_id;
    sched_rach.timing_advance  = rach_ind.timing_advance;
    sched_rach.crnti           = rnti;
    sched.handle_rach_indication(sched_rach);
  }

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

  mac_cell_rach_handler& get_cell(du_cell_index_t cell_index) { return cells[cell_index]; }

private:
  scheduler_configurator&                         sched;
  rnti_allocator                                  rnti_alloc;
  slot_array<cell_rach_handler, MAX_NOF_DU_CELLS> cells;
};

} // namespace srsgnb
