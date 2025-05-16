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
#include "mac_dl_configurator.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/srsran_assert.h"
#include "srsran/support/timers.h"

using namespace srsran;

/// Number of subframes per hyper frame.
static constexpr uint32_t NOF_SF_PER_HFN = NOF_SFNS * NOF_SUBFRAMES_PER_FRAME;
static constexpr int      MAX_COUNT_DIFF = NOF_SF_PER_HFN / 2;

class du_time_controller::time_ticker_impl final : public du_cell_timer_source
{
public:
  du_time_controller&   parent;
  const du_cell_index_t cell_index;

  time_ticker_impl(du_time_controller& parent_, du_cell_index_t cell_index_) : parent(parent_), cell_index(cell_index_)
  {
  }

  void on_cell_activation(unsigned sfn, unsigned subframe) override
  {
    parent.handle_cell_activation(cell_index, sfn, subframe);
  }

  void on_cell_deactivation() override { parent.handle_cell_deactivation(cell_index); }

  void on_tick(unsigned sfn, unsigned subframe) override { parent.handle_slot_ind(cell_index, sfn, subframe); }

  subframe_counter now() const override { return parent.now(cell_index); }
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

void du_time_controller::handle_cell_activation(du_cell_index_t cell_index, unsigned sfn, unsigned subframe)
{
  srsran_assert(not cells[cell_index].active, "Invalid state");
  cells[cell_index].active = true;

  // Add cell to linked list of active cells.
  if (push_back_new_cell(cell_index)) {
    // The cell was the first to be added. Then, it is the current master cell.
    // Start with hfn_count == 0.
    master_count.store(sfn * NOF_SUBFRAMES_PER_FRAME + subframe - 1, std::memory_order_relaxed);
  } else {
    // Setup cell-local last subframe counter.
    uint32_t count_cpy  = master_count.load(std::memory_order_relaxed);
    uint32_t hfn_count  = count_cpy / NOF_SF_PER_HFN;
    uint32_t curr_count = hfn_count * NOF_SF_PER_HFN + sfn * NOF_SUBFRAMES_PER_FRAME + subframe;
    int      diff       = curr_count - count_cpy;
    if (diff < -MAX_COUNT_DIFF) {
      // SFN rollover.
      curr_count += NOF_SF_PER_HFN;
    }
    cells[cell_index].last_counter = curr_count - 1;
  }
}

void du_time_controller::handle_cell_deactivation(du_cell_index_t cell_index)
{
  srsran_assert(cells[cell_index].active, "Invalid state");
  cells[cell_index].active = false;

  bool all_cells_removed = rem_cell(cell_index);
  if (all_cells_removed) {
    logger.info("All cells have been deactivating. The timers will be stopped.");
    // TODO: Initiate system timer service.
  }
}

void du_time_controller::handle_slot_ind(du_cell_index_t cell_index, unsigned sfn, unsigned subframe)
{
  uint32_t last_hfn_count = cells[cell_index].last_counter / NOF_SF_PER_HFN;
  uint32_t cur_count      = last_hfn_count * NOF_SF_PER_HFN + sfn * NOF_SUBFRAMES_PER_FRAME + subframe;
  if (cur_count < cells[cell_index].last_counter) {
    // SFN rollover detected.
    cur_count += NOF_SF_PER_HFN;
  }
  cells[cell_index].last_counter = cur_count;

  // Check if the cell is responsible for ticking.
  if (this->head.load(std::memory_order_acquire) != cell_index) {
    // Another cell is in control. Return.
    return;
  }

  subframe_counter master_count_cpy = master_count.load(std::memory_order_relaxed);
  const uint32_t   global_hfn_count = master_count_cpy / NOF_SF_PER_HFN;
  subframe_counter curr_count       = global_hfn_count * NOF_SF_PER_HFN + sfn * NOF_SUBFRAMES_PER_FRAME + subframe;

  int nof_skipped = curr_count - master_count_cpy;
  if (nof_skipped < -static_cast<int>(NOF_SF_PER_HFN) or nof_skipped > MAX_COUNT_DIFF) {
    // The difference is too large. This could happen during activation/deactivation of the cells.
    // Fix the master_count.
    master_count_cpy = curr_count - 1;
    nof_skipped      = curr_count - master_count_cpy;
  } else if (nof_skipped < 0) {
    // SFN rollover detection.
    curr_count += NOF_SF_PER_HFN;
    nof_skipped = curr_count - master_count_cpy;
  }

  // Initiate operation to tick timers.
  if (tick_exec.defer([this, nof_skipped]() {
        for (int i = 0; i != nof_skipped; ++i) {
          timers.tick();
        }
      })) {
    // The task executor dispatch was successful. Update the master count.
    master_count.store(curr_count, std::memory_order_relaxed);
  }
}

subframe_counter du_time_controller::now(du_cell_index_t cell_index) const
{
  return cells[cell_index].last_counter;
}
