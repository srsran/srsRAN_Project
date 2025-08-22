/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/du/du_high/du_high_clock_controller.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
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
    bool              ret = shutdown_worker.push_task([this, &stopped]() {
      timer_ctrl.reset();
      stopped = true;
    });
    report_fatal_error_if_not(ret, "Failed to push task to worker");
    while (not stopped) {
      timer_worker.run_pending_tasks();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  timer_manager              timers;
  manual_task_worker         timer_worker{128};
  task_worker                shutdown_worker{"WORKER", 128};
  std::unique_ptr<io_broker> broker{
      create_io_broker(io_broker_type::epoll, io_broker_config{os_thread_realtime_priority::no_realtime()})};
  std::unique_ptr<mac_clock_controller> timer_ctrl = create_du_high_clock_controller(timers, *broker, timer_worker);
};

TEST_F(du_high_time_source_test, time_ticks_when_no_cells)
{
  auto                  timer = timers.create_unique_timer(timer_worker);
  std::atomic<unsigned> count{0};
  timer.set(std::chrono::milliseconds(1), [&](timer_id_t /* unused */) { count++; });
  auto prev = timers.now();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  ASSERT_EQ(count, 0);

  timer.run();
  static constexpr unsigned test_timeout = 100000;
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

TEST_F(du_high_time_source_test, add_cell)
{
  auto cell_source = timer_ctrl->add_cell(to_du_cell_index(0));
  ASSERT_NE(cell_source, nullptr);

  ASSERT_FALSE(cell_source->now().valid());

  slot_point cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  cell_source->on_slot_indication(cur_sl_tx);

  ASSERT_TRUE(cell_source->now().valid());
  ASSERT_EQ(cell_source->now(), cur_sl_tx);
}

TEST_F(du_high_time_source_test, tick_periodicity_for_scs15khz_is_1msec)
{
  auto cell_source = timer_ctrl->add_cell(to_du_cell_index(0));

  slot_point     cur_sl_tx = {subcarrier_spacing::kHz15, 0};
  const unsigned nof_slots = 40;
  for (unsigned i = 0; i != nof_slots; ++i) {
    cell_source->on_slot_indication(cur_sl_tx);
    timer_worker.run_pending_tasks();
    ASSERT_EQ(cell_source->now(), cur_sl_tx);
    ++cur_sl_tx;
  }

  ASSERT_EQ(timers.now(), nof_slots);
}

TEST_F(du_high_time_source_test, tick_periodicity_for_scs30khz_is_1msec)
{
  auto cell_source = timer_ctrl->add_cell(to_du_cell_index(0));

  slot_point     cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  const unsigned nof_slots = 40;
  for (unsigned i = 0; i != nof_slots; ++i) {
    cell_source->on_slot_indication(cur_sl_tx);
    timer_worker.run_pending_tasks();
    ASSERT_EQ(cell_source->now(), cur_sl_tx);
    ++cur_sl_tx;
  }

  ASSERT_EQ(timers.now(), nof_slots / 2 - 1);
}

TEST_F(du_high_time_source_test, only_one_tick_per_msec_for_multiple_cells)
{
  auto cell_source1 = timer_ctrl->add_cell(to_du_cell_index(0));
  auto cell_source2 = timer_ctrl->add_cell(to_du_cell_index(1));

  slot_point     cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  const unsigned nof_slots = 40;
  for (unsigned i = 0; i != nof_slots; ++i) {
    cell_source1->on_slot_indication(cur_sl_tx);
    cell_source2->on_slot_indication(cur_sl_tx);
    timer_worker.run_pending_tasks();
    ASSERT_EQ(cell_source1->now(), cur_sl_tx);
    ASSERT_EQ(cell_source2->now(), cur_sl_tx);
    ++cur_sl_tx;
  }

  ASSERT_EQ(timers.now(), nof_slots / 2 - 1);
}
