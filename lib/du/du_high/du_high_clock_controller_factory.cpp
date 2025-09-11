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

#include "srsran/adt/noop_functor.h"
#include "srsran/du/du_high/du_high_clock_controller.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/io/io_timer_source.h"
#include "srsran/support/timers.h"

using namespace srsran;
using namespace srs_du;

namespace {

// Helper to setup the cell initial last slot counter based on the master cell clock.
slot_point_extended get_init_slot_tx_ext(slot_point sl_tx, slot_point_extended master_sl_tx)
{
  static const int    max_slot_diff = sl_tx.nof_slots_per_hyper_system_frame() / 2;
  slot_point_extended curr_count{sl_tx, master_sl_tx.hyper_sfn()};
  int                 diff = curr_count - master_sl_tx;
  if (diff < -max_slot_diff) {
    // This new cell is a bit ahead of the master cell and there was SFN rollover.
    curr_count += sl_tx.nof_slots_per_hyper_system_frame();
  } else if (diff > max_slot_diff) {
    // This new cell is a bit behind the master cell and there was SFN rollover.
    curr_count -= sl_tx.nof_slots_per_hyper_system_frame();
  }
  return curr_count;
}

/// IO timer source that supports two modes: automatically ticking based on wall clock and manual ticking.
class io_timer_multi_source
{
public:
  /// Unique handle that allows to manually tick the timers.
  class unique_ticker
  {
  public:
    explicit unique_ticker(io_timer_multi_source& parent_) : parent(&parent_) {}
    unique_ticker(unique_ticker&& other) noexcept            = default;
    unique_ticker& operator=(unique_ticker&& other) noexcept = default;
    ~unique_ticker()
    {
      if (parent != nullptr) {
        parent->do_reset_ticker();
      }
    }

    void tick(unsigned nof_ticks) { parent->do_tick(nof_ticks); }

  private:
    std::unique_ptr<io_timer_multi_source, noop_operation> parent;
  };

  io_timer_multi_source(timer_manager&            timers_,
                        io_broker&                broker,
                        task_executor&            tick_exec_,
                        std::chrono::milliseconds tick_period,
                        srslog::basic_logger&     logger_,
                        bool                      auto_start = true) :
    timers(timers_),
    tick_exec(tick_exec_),
    logger(logger_),
    io_source(timers, broker, tick_exec, tick_period, auto_start)
  {
  }

  unique_ticker create_manual_ticker()
  {
    unsigned prev = nof_tickers.fetch_add(1, std::memory_order_acq_rel);
    if (prev == 0) {
      // First ticker created. Switch to manual mode.
      logger.debug("Switching to cell-driven timer ticking. Cause: First cell has been activated.");
      io_source.request_stop();
    }
    return unique_ticker(*this);
  }

private:
  void do_reset_ticker()
  {
    unsigned current = nof_tickers.fetch_sub(1, std::memory_order_acq_rel) - 1;
    if (current == 0) {
      // Last ticker to be deleted. Switch to automatic mode.
      logger.debug("Switching to wall clock timer ticking. Cause: All cells have been deactivated.");
      io_source.resume();
    }
  }

  void do_tick(unsigned nof_ticks)
  {
    nof_ticks += missed_ticks.exchange(0, std::memory_order_relaxed);
    if (nof_ticks == 0) {
      return;
    }

    if (not tick_exec.defer([this, nof_ticks]() {
          for (unsigned i = 0; i != nof_ticks; ++i) {
            timers.tick();
          }
        })) {
      // Failed to dispatch. Count as missed ticks.
      missed_ticks.fetch_add(nof_ticks, std::memory_order_relaxed);
    }
  }

  timer_manager&        timers;
  task_executor&        tick_exec;
  srslog::basic_logger& logger;

  std::atomic<unsigned> nof_tickers{0};
  std::atomic<unsigned> missed_ticks{0};

  // IO time source enabled when not in manual mode.
  io_timer_source io_source;
};

class du_high_time_source_impl final : public mac_clock_controller
{
public:
  class cell_ticker final : public mac_cell_clock_controller
  {
  public:
    cell_ticker(du_high_time_source_impl& parent_, du_cell_index_t cell_index_) :
      parent(&parent_), cell_index(cell_index_)
    {
    }

    ~cell_ticker() override { on_cell_deactivation(); }

    void on_cell_deactivation() override
    {
      if (parent != nullptr and this->cached_now.valid()) {
        parent->handle_cell_deactivation(cell_index);
        this->cached_now = {};
      }
    }

  private:
    slot_point_extended do_on_slot_indication(slot_point sl_tx) override
    {
      srsran_assert(parent != nullptr, "Slot indication for a deleted cell");
      return parent->handle_slot_indication(cell_index, sl_tx);
    }

    std::unique_ptr<du_high_time_source_impl, noop_operation> parent;
    du_cell_index_t                                           cell_index;
  };

  du_high_time_source_impl(timer_manager& timers_, io_broker& broker, task_executor& tick_exec_) :
    timers(timers_),
    tick_exec(tick_exec_),
    logger(srslog::fetch_basic_logger("DU", false)),
    io_source(timers, broker, tick_exec, std::chrono::milliseconds{1}, logger)
  {
  }

  std::unique_ptr<mac_cell_clock_controller> add_cell(du_cell_index_t cell_index) override
  {
    srsran_assert(not cells[cell_index].active(), "Cell ticker already created");
    return std::make_unique<cell_ticker>(*this, cell_index);
  }

  timer_manager& get_timer_manager() override { return timers; }

private:
  struct cell_context {
    slot_point_extended                                 last_counter;
    std::optional<io_timer_multi_source::unique_ticker> ticker;

    bool active() const { return ticker.has_value(); }
  };

  slot_point_extended handle_slot_indication(du_cell_index_t cell_index, slot_point sl_tx)
  {
    static constexpr int MAX_SKIPPED = 128;

    // Update cell slot counter.
    slot_point_extended& cell_sl_counter = cells[cell_index].last_counter;
    if (SRSRAN_UNLIKELY(not cells[cell_index].active())) {
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

    // Update the master state.
    // For each new slot, only one cell will be able to win the "race" and update the master clock.
    uint64_t cur_master_state = master_state.load(std::memory_order_relaxed);
    while (cell_sl_counter > unpack_slot_ext(cur_master_state)) {
      uint64_t next_master_state = pack_state(unpack_nof_active_cells(cur_master_state), cell_sl_counter);
      if (master_state.compare_exchange_weak(cur_master_state, next_master_state)) {
        // Successfully updated the master state.
        break;
      }
    }

    // Compute the number of skipped subframes.
    int nof_skipped =
        (cell_sl_counter - unpack_slot_ext(cur_master_state)) / static_cast<int>(sl_tx.nof_slots_per_subframe());

    // If nof_skipped > 0, this is the cell responsible for ticking.
    if (nof_skipped <= 0) {
      return cell_sl_counter;
    }

    if (nof_skipped >= MAX_SKIPPED) {
      // Number of skipped slots is too high. This is likely an error.
      logger.warning("cell={}: Unexpected jump in slot indications of {}", fmt::underlying(cell_index), nof_skipped);
      return cell_sl_counter;
    }

    // Tick the timer.
    cells[cell_index].ticker->tick(nof_skipped);

    return cell_sl_counter;
  }

  /// Called on the first slot indication received for a cell.
  void handle_cell_activation(du_cell_index_t cell_index, slot_point sl_tx)
  {
    if (cells[cell_index].active()) {
      return;
    }
    cells[cell_index].ticker.emplace(io_source.create_manual_ticker());

    // Setup the cell initial slot counter.
    uint64_t cur_master_state = master_state.load(std::memory_order_relaxed);
    while (unpack_nof_active_cells(cur_master_state) == 0) {
      // First cell to be activated. Start with HFN count == 0, and set the master clock to one slot before the current
      // slot.
      cells[cell_index].last_counter = slot_point_extended{sl_tx, 0};
      uint64_t next_master_state     = pack_state(1, cells[cell_index].last_counter - 1);
      if (master_state.compare_exchange_weak(cur_master_state, next_master_state)) {
        // Successfully set the master state.
        return;
      }
      // Either another cell concurrently changed the state or the CAS operation failed spuriously.
    }

    // We reach this point only when nof active cells is > 0.
    cells[cell_index].last_counter = get_init_slot_tx_ext(sl_tx, unpack_slot_ext(cur_master_state));
  }

  /// Called when the cell gets deactivated.
  void handle_cell_deactivation(du_cell_index_t cell_index)
  {
    if (not cells[cell_index].active()) {
      // Cell ticker was created but never used.
      return;
    }
    cells[cell_index].ticker.reset();

    // Decrement number of active cells.
    master_state.fetch_sub(static_cast<uint64_t>(1U) << 32U, std::memory_order_acq_rel);
  }

  static uint32_t            unpack_nof_active_cells(uint64_t state) { return state >> 32U; }
  static slot_point_extended unpack_slot_ext(uint64_t state)
  {
    uint32_t numerology = (state >> 29U) & 0b111U;
    // 29 bits for the count
    uint32_t count = state & 0x1fffffffU;
    return slot_point_extended{to_subcarrier_spacing(numerology), count};
  }
  static uint64_t pack_slot_ext(slot_point_extended sl)
  {
    uint64_t numerology = sl.numerology();
    uint64_t count      = sl.count();
    return (numerology << 29U) | count;
  }
  static uint64_t pack_state(uint32_t nof_active_cells, slot_point_extended sl)
  {
    return pack_slot_ext(sl) | (static_cast<uint64_t>(nof_active_cells) << 32U);
  }

  timer_manager&        timers;
  task_executor&        tick_exec;
  srslog::basic_logger& logger;

  std::array<cell_context, MAX_NOF_DU_CELLS> cells;

  /// Representation of the state of the DU-high time source. The upper 32 bits represent the number of active cells,
  /// while the lower 32 bits represent the latest slot point. The latest time point is represented using 3 bits for the
  /// the numerology and 29 bits for the slot count.
  std::atomic<uint64_t> master_state{0};

  io_timer_multi_source io_source;
};

} // namespace

std::unique_ptr<mac_clock_controller>
srs_du::create_du_high_clock_controller(timer_manager& timers, io_broker& broker, task_executor& tick_exec)
{
  return std::make_unique<du_high_time_source_impl>(timers, broker, tick_exec);
}
