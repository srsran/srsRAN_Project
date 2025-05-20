/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_time_controller.h"
#include "../mac_dl/mac_dl_configurator.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/srsran_assert.h"
#include "srsran/support/timers.h"

using namespace srsran;

/// Number of subframes per hyper frame.
static constexpr uint32_t NOF_SF_PER_HFN = NOF_SFNS * NOF_SUBFRAMES_PER_FRAME;

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

  slot_point_extended on_slot_indication(slot_point sl_tx) override
  {
    this->cached_now = parent.handle_slot_ind(cell_index, sl_tx);
    return this->cached_now;
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

bool du_time_controller::push_back_new_cell(du_cell_index_t cell_index)
{
  cells[cell_index].next = -1;

  while (true) {
    int curr = head.load(std::memory_order_acquire);

    if (curr == -1) {
      // Empty list, try to insert at head
      if (head.compare_exchange_strong(curr, cell_index, std::memory_order_release, std::memory_order_acquire)) {
        return true;
      }
    } else {
      // Walk to the tail
      while (true) {
        int next = cells[curr].next.load(std::memory_order_acquire);
        if (next == -1) {
          // Try to attach at the tail
          if (cells[curr].next.compare_exchange_weak(
                  next, cell_index, std::memory_order_release, std::memory_order_acquire)) {
            return false;
          } else {
            // Restart traversal in case of contention.
            break;
          }
        }
        curr = next;
      }
    }
  }
}

bool du_time_controller::rem_cell(du_cell_index_t cell_index)
{
  int prev = -1;
  int curr = head.load(std::memory_order_acquire);
  while (curr != -1) {
    int next = cells[curr].next.load(std::memory_order_acquire);

    if (curr == cell_index) {
      // Found target
      if (prev == -1) {
        // It's the head
        if (head.compare_exchange_strong(curr, next, std::memory_order_release, std::memory_order_acquire)) {
          return next == -1;
        }
      } else {
        if (cells[prev].next.compare_exchange_strong(
                curr, next, std::memory_order_release, std::memory_order_acquire)) {
          return false;
        }
      }
    }

    prev = curr;
    curr = next;
  }

  report_fatal_error("Unable to remove cell {} from linked list", fmt::underlying(cell_index));
}

void du_time_controller::handle_cell_activation(du_cell_index_t cell_index, slot_point sl_tx)
{
  srsran_assert(not cells[cell_index].active, "Invalid state");
  cells[cell_index].active = true;

  // Add cell to linked list of active cells.
  if (push_back_new_cell(cell_index)) {
    // The cell was the first to be added. Then, it is the current master cell.
    // Start with HFN count == 0.
    cells[cell_index].last_counter = slot_point_extended{sl_tx, 0};
    master_count.store(cells[cell_index].last_counter.count(), std::memory_order_relaxed);
  } else {
    // Setup cell-local last slot counter.
    slot_point_extended master_cpy{sl_tx.scs(), master_count.load(std::memory_order_relaxed)};
    slot_point_extended curr_count{sl_tx, master_cpy.hfn()};
    int                 diff          = curr_count - master_cpy;
    const int           max_slot_diff = sl_tx.nof_slots_per_hyper_frame() / 2;
    if (diff < -max_slot_diff) {
      // This new cell is a bit ahead of the master cell and there was SFN rollover.
      curr_count += sl_tx.nof_slots_per_hyper_frame();
    } else if (diff > max_slot_diff) {
      // This new cell is a bit behind the master cell and there was SFN rollover.
      curr_count -= sl_tx.nof_slots_per_hyper_frame();
    }
    cells[cell_index].last_counter = curr_count;
  }
}

void du_time_controller::handle_cell_deactivation(du_cell_index_t cell_index)
{
  if (not cells[cell_index].active) {
    // Cell is already deactivated. Nothing to do.
    return;
  }
  cells[cell_index].active = false;

  bool all_cells_removed = rem_cell(cell_index);
  if (all_cells_removed) {
    logger.info("All cells have been deactivating. The timers will be stopped.");
    // TODO: Initiate system timer service.
  }
}

slot_point_extended du_time_controller::handle_slot_ind(du_cell_index_t cell_index, slot_point sl_tx)
{
  constexpr static int MAX_SKIPPED = 128;

  // Set logger context.
  logger.set_context(sl_tx.sfn(), sl_tx.slot_index());

  if (not cells[cell_index].active) {
    // Create cell if it is not yet active.
    handle_cell_activation(cell_index, sl_tx);
  }

  slot_point_extended sl_tx_ext{sl_tx, cells[cell_index].last_counter.hfn()};
  if (sl_tx_ext < cells[cell_index].last_counter) {
    // SFN rollover detected. Increment HFN.
    sl_tx_ext += sl_tx.nof_slots_per_hyper_frame();
  }

  // Update cell slot counter.
  int nof_skipped                = sl_tx_ext - cells[cell_index].last_counter;
  cells[cell_index].last_counter = sl_tx_ext;

  if (sl_tx_ext.subframe_slot_index() != 0) {
    // Not a subframe boundary. Return.
    return cells[cell_index].last_counter;
  }

  // Check if the cell is responsible for ticking.
  if (this->head.load(std::memory_order_acquire) != cell_index) {
    // Another cell is in control. Return.
    return cells[cell_index].last_counter;
  }

  if (nof_skipped < 0 or nof_skipped >= MAX_SKIPPED) {
    // Number of skipped slots is too high. This is likely an error.
    logger.warning("cell={}: Unexpected jump in slot indications of {}", fmt::underlying(cell_index), nof_skipped);
    return cells[cell_index].last_counter;
  }

  // Initiate operation to tick timers.
  if (tick_exec.defer([this, nof_skipped]() {
        for (int i = 0; i != nof_skipped; ++i) {
          timers.tick();
        }
      })) {
    // The task executor dispatch was successful. Update the master count.
    master_count.store(sl_tx_ext.count(), std::memory_order_relaxed);
  }

  return cells[cell_index].last_counter;
}
