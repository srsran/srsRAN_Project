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

#include "du_time_controller.h"
#include "../mac_dl/mac_dl_configurator.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/srsran_assert.h"
#include "srsran/support/timers.h"

using namespace srsran;

class du_time_controller::time_ticker_impl final : public du_cell_timer_source
{
public:
  du_time_controller&   parent;
  const du_cell_index_t cell_index;

  time_ticker_impl(du_time_controller& parent_, du_cell_index_t cell_index_) : parent(parent_), cell_index(cell_index_)
  {
  }

  ~time_ticker_impl() override
  {
    if (parent.cells[cell_index].active) {
      parent.handle_cell_deactivation(cell_index);
    }
  }

  void on_cell_deactivation() override { parent.handle_cell_deactivation(cell_index); }

  slot_point_extended on_slot_indication_impl(slot_point sl_tx) override
  {
    return parent.handle_slot_ind(cell_index, sl_tx);
  }
};

du_time_controller::du_time_controller(timer_manager&        timers_,
                                       task_executor&        tick_exec_,
                                       srslog::basic_logger& logger_) :
  timers(timers_), tick_exec(tick_exec_), logger(logger_)
{
}

std::unique_ptr<du_cell_timer_source> du_time_controller::add_cell(du_cell_index_t cell_index)
{
  return std::make_unique<time_ticker_impl>(*this, cell_index);
}

// Helper to setup the cell initial last slot counter based on the master cell clock.
static slot_point_extended get_init_slot_tx_ext(slot_point sl_tx, uint32_t master_cell_clock)
{
  static const int    max_slot_diff = sl_tx.nof_slots_per_hyper_system_frame() / 2;
  slot_point_extended master_sl{sl_tx.scs(), master_cell_clock};
  slot_point_extended curr_count{sl_tx, master_sl.hyper_sfn()};
  int                 diff = curr_count - master_sl;
  if (diff < -max_slot_diff) {
    // This new cell is a bit ahead of the master cell and there was SFN rollover.
    curr_count += sl_tx.nof_slots_per_hyper_system_frame();
  } else if (diff > max_slot_diff) {
    // This new cell is a bit behind the master cell and there was SFN rollover.
    curr_count -= sl_tx.nof_slots_per_hyper_system_frame();
  }
  return curr_count;
}

void du_time_controller::handle_cell_activation(du_cell_index_t cell_index, slot_point sl_tx)
{
  if (cells[cell_index].active) {
    return;
  }
  cells[cell_index].active = true;

  // Increment number of active cells.
  nof_active_cells.fetch_add(1, std::memory_order_acq_rel);

  // Setup the cell initial slot counter.
  int32_t cur_master_clock = master_count.load(std::memory_order_relaxed);
  if (cur_master_clock < 0) {
    // First cell to be activated. Start with HFN count == 0, and set the master clock to one slot before the current
    // slot.
    cells[cell_index].last_counter = slot_point_extended{sl_tx, 0};
    uint32_t master_count_val      = (cells[cell_index].last_counter - 1).count();
    if (not master_count.compare_exchange_strong(cur_master_clock, master_count_val)) {
      // Another cell won the race and initialized the master clock instead.
      srsran_sanity_check(cur_master_clock >= 0, "invalid master clock");
      cells[cell_index].last_counter = get_init_slot_tx_ext(sl_tx, cur_master_clock);
    }
  } else {
    cells[cell_index].last_counter = get_init_slot_tx_ext(sl_tx, cur_master_clock);
  }
}

void du_time_controller::handle_cell_deactivation(du_cell_index_t cell_index)
{
  if (not cells[cell_index].active) {
    // Cell is already deactivated. Nothing to do.
    return;
  }
  cells[cell_index].active = false;

  int32_t master_cell_clock = master_count.load(std::memory_order_relaxed);

  // Decrement the number of active cells.
  uint32_t nof_active = nof_active_cells.fetch_sub(1, std::memory_order_release) - 1;

  // Check if this is the last cell to be deactivated.
  // If so, we will try to reset the master clock, as there may be a long period of time ahead when we will not be
  // receiving new slot indications. However, we have to watch out for concurrent cell activations.
  while (nof_active == 0 and master_cell_clock >= 0 and
         not master_count.compare_exchange_strong(
             master_cell_clock, -1, std::memory_order_release, std::memory_order_relaxed)) {
    nof_active = nof_active_cells.load(std::memory_order_acquire);
  }

  if (nof_active == 0) {
    logger.info("All cells have been deactivating. The timers will be stopped.");
    // TODO: Initiate system timer service.
  }
}

slot_point_extended du_time_controller::handle_slot_ind(du_cell_index_t cell_index, slot_point sl_tx)
{
  constexpr static int MAX_SKIPPED = 128;

  // Set logger context.
  logger.set_context(sl_tx.sfn(), sl_tx.slot_index());

  // Update cell slot counter.
  slot_point_extended& cell_sl_counter = cells[cell_index].last_counter;
  if (SRSRAN_UNLIKELY(not cells[cell_index].active)) {
    // Create cell if it is not yet active.
    handle_cell_activation(cell_index, sl_tx);
  } else {
    slot_point_extended sl_tx_ext{sl_tx, cell_sl_counter.hyper_sfn()};
    if (sl_tx_ext < cell_sl_counter) {
      // SFN rollover detected. Increment HFN.
      sl_tx_ext += sl_tx.nof_slots_per_hyper_system_frame();
    }
    cell_sl_counter = sl_tx_ext;
  }

  if (cell_sl_counter.subframe_slot_index() != 0) {
    // Not a subframe boundary. Return.
    return cell_sl_counter;
  }

  // Update the master cell clock.
  // For each new slot, only one cell will be able to win the "race" and update the master clock.
  int32_t             master_cpy = master_count.load(std::memory_order_relaxed);
  slot_point_extended master_sl;
  do {
    master_sl = {sl_tx.scs(), static_cast<uint32_t>(master_cpy)};
  } while (cell_sl_counter > master_sl and not master_count.compare_exchange_weak(master_cpy, cell_sl_counter.count()));
  int nof_skipped = cell_sl_counter - master_sl;

  // If nof_skipped > 0, this is the cell responsible for ticking.
  if (nof_skipped <= 0) {
    return cell_sl_counter;
  }

  if (nof_skipped >= MAX_SKIPPED) {
    // Number of skipped slots is too high. This is likely an error.
    logger.warning("cell={}: Unexpected jump in slot indications of {}", fmt::underlying(cell_index), nof_skipped);
    return cell_sl_counter;
  }

  // Add the slots that could not be pushed to the executor before.
  nof_skipped += missed_slots.exchange(0, std::memory_order_relaxed);

  // Initiate operation to tick timers.
  if (not tick_exec.defer([this, nof_skipped]() {
        for (int i = 0; i != nof_skipped; ++i) {
          timers.tick();
        }
      })) {
    // Defer operation failed. The queue is full.
    // We accumulate the number of skipped slots, so that we can tick them later.
    missed_slots.store(nof_skipped, std::memory_order_relaxed);
  }

  return cell_sl_counter;
}
