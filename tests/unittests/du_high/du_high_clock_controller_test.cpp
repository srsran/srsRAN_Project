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

#include "srsran/du/du_high/du_high_clock_controller.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/test_utils.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

class du_high_time_source_test : public ::testing::Test
{
protected:
  du_high_time_source_test() {}
  ~du_high_time_source_test() override
  {
    // The timer_ctrl.reset() is blocking, and since timer_worker is manual, we have to dispatch the reset to a separate
    // thread.
    std::atomic<bool> stopped{false};
    shutdown_worker.push_task_blocking([this, &stopped]() {
      timer_ctrl.reset();
      stopped = true;
    });
    while (not stopped) {
      timer_worker.run_pending_tasks();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  bool is_clock_running_automatically()
  {
    auto prev = timers.now();
    // Very high timeout to avoid hanging the test in case of failure.
    static constexpr unsigned test_timeout = 1000000;
    for (unsigned i = 0; i != test_timeout; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      timer_worker.run_pending_tasks();
      if (timers.now() > prev) {
        return true;
      }
    }
    return false;
  }

  bool is_clock_running_manually()
  {
    auto prev = timers.now();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    timer_worker.run_pending_tasks();
    return prev == timers.now();
  }

  timer_manager              timers;
  manual_task_worker         timer_worker{128};
  task_worker                shutdown_worker{"WORKER", 128};
  std::unique_ptr<io_broker> broker{
      create_io_broker(io_broker_type::epoll, io_broker_config{os_thread_realtime_priority::no_realtime()})};
  std::unique_ptr<mac_clock_controller> timer_ctrl = create_du_high_clock_controller(timers, *broker, timer_worker);
};

TEST_F(du_high_time_source_test, time_ticks_when_no_cells_were_added)
{
  auto                  timer = timers.create_unique_timer(timer_worker);
  std::atomic<unsigned> count{0};
  timer.set(std::chrono::milliseconds(1), [&](timer_id_t /* unused */) { count++; });
  auto prev = timers.now();

  // Timer is not running yet.
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  timer_worker.run_pending_tasks();
  ASSERT_EQ(count, 0);

  // Timer is now being triggered.
  timer.run();
  static constexpr unsigned test_timeout = 1000000;
  for (unsigned i = 0; i != test_timeout; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    timer_worker.run_pending_tasks();
    if (count > 0) {
      break;
    }
  }
  ASSERT_GT(count, 0);
  ASSERT_GT(timers.now(), prev);
}

TEST_F(du_high_time_source_test, when_cell_is_activated_then_ticking_becomes_manual)
{
  // Create a cell ticker.
  auto cell_source = timer_ctrl->add_cell(to_du_cell_index(0));
  ASSERT_NE(cell_source, nullptr);
  ASSERT_FALSE(cell_source->now().valid());

  // Timer is still advancing, until cell is activated (!= created).
  ASSERT_TRUE(is_clock_running_automatically());

  // Activate cell.
  slot_point cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  cell_source->on_slot_indication(cur_sl_tx);
  timer_worker.run_pending_tasks();

  // Ticking is now manual, so unless we call slot_indication, the clock doesn't advance.
  ASSERT_TRUE(is_clock_running_manually());
}

TEST_F(du_high_time_source_test, cell_slot_now_is_updated_on_slot_indication)
{
  auto cell_source = timer_ctrl->add_cell(to_du_cell_index(0));
  ASSERT_NE(cell_source, nullptr);

  ASSERT_FALSE(cell_source->now().valid());

  slot_point cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  cell_source->on_slot_indication(cur_sl_tx);

  ASSERT_TRUE(cell_source->now().valid());
  ASSERT_EQ(cell_source->now(), cur_sl_tx);
}

TEST_F(du_high_time_source_test, when_cell_is_deactivated_then_ticking_becomes_automatic)
{
  std::vector<std::unique_ptr<mac_cell_clock_controller>> cells(test_rgen::uniform_int<unsigned>(1, 5));
  slot_point                                              cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  for (auto& cell : cells) {
    cell = timer_ctrl->add_cell(to_du_cell_index(std::distance(cells.data(), &cell)));
    cell->on_slot_indication(cur_sl_tx);
    timer_worker.run_pending_tasks();
  }

  ASSERT_TRUE(is_clock_running_manually());

  // Deactivate cells.
  cells.clear();

  // Timer is now automatic again.
  ASSERT_TRUE(is_clock_running_automatically());
}

TEST_F(du_high_time_source_test, tick_periodicity_for_scs15khz_is_1msec)
{
  auto cell_source = timer_ctrl->add_cell(to_du_cell_index(0));

  // Cells get activated with slot_indication.
  slot_point cur_sl_tx = {subcarrier_spacing::kHz15, 0};
  cell_source->on_slot_indication(cur_sl_tx++);
  timer_worker.run_pending_tasks();

  auto           start_now = timers.now();
  const unsigned nof_slots = 40;
  for (unsigned i = 0; i != nof_slots; ++i) {
    cell_source->on_slot_indication(cur_sl_tx);
    timer_worker.run_pending_tasks();
    ASSERT_EQ(cell_source->now(), cur_sl_tx);
    ++cur_sl_tx;
  }

  ASSERT_EQ(timers.now() - start_now, nof_slots);
}

TEST_F(du_high_time_source_test, tick_periodicity_for_scs30khz_is_1msec)
{
  auto cell_source = timer_ctrl->add_cell(to_du_cell_index(0));

  // Cells get activated with slot_indication.
  slot_point cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  cell_source->on_slot_indication(cur_sl_tx++);
  // Note: We advance twice to start next check in an even slot index.
  cell_source->on_slot_indication(cur_sl_tx++);
  timer_worker.run_pending_tasks();

  const unsigned nof_slots = 40;
  auto           start_now = timers.now();
  for (unsigned i = 0; i != nof_slots; ++i) {
    cell_source->on_slot_indication(cur_sl_tx);
    timer_worker.run_pending_tasks();
    ASSERT_EQ(cell_source->now(), cur_sl_tx);
    ++cur_sl_tx;
  }

  ASSERT_EQ(timers.now() - start_now, nof_slots / 2);
}

TEST_F(du_high_time_source_test, only_one_tick_per_msec_for_multiple_cells)
{
  auto cell_source1 = timer_ctrl->add_cell(to_du_cell_index(0));
  auto cell_source2 = timer_ctrl->add_cell(to_du_cell_index(1));

  // Cells get activated with slot_indication.
  slot_point cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  cell_source1->on_slot_indication(cur_sl_tx);
  cell_source2->on_slot_indication(cur_sl_tx++);
  // Note: We advance twice to start next check in an even slot index.
  cell_source1->on_slot_indication(cur_sl_tx);
  cell_source2->on_slot_indication(cur_sl_tx++);
  timer_worker.run_pending_tasks();

  auto           start_now = timers.now();
  const unsigned nof_slots = 40;
  for (unsigned i = 0; i != nof_slots; ++i) {
    cell_source1->on_slot_indication(cur_sl_tx);
    cell_source2->on_slot_indication(cur_sl_tx);
    timer_worker.run_pending_tasks();
    ASSERT_EQ(cell_source1->now(), cur_sl_tx);
    ASSERT_EQ(cell_source2->now(), cur_sl_tx);
    ++cur_sl_tx;
  }

  ASSERT_EQ(timers.now() - start_now, nof_slots / 2);
}
