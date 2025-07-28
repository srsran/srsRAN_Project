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

#include "lib/mac/mac_ctrl/du_time_controller.h"
#include "lib/mac/mac_dl/mac_dl_configurator.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>

using namespace srsran;

class du_timer_controller_test : public ::testing::Test
{
protected:
  du_timer_controller_test() {}

  timer_manager      timers;
  manual_task_worker task_worker{128};
  du_time_controller timer_ctrl{timers, task_worker, srslog::fetch_basic_logger("TEST")};
};

TEST_F(du_timer_controller_test, add_cell)
{
  auto cell_source = timer_ctrl.add_cell(to_du_cell_index(0));
  ASSERT_NE(cell_source, nullptr);

  ASSERT_FALSE(cell_source->now().valid());

  slot_point cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  cell_source->on_slot_indication(cur_sl_tx);

  ASSERT_TRUE(cell_source->now().valid());
  ASSERT_EQ(cell_source->now(), cur_sl_tx);
}

TEST_F(du_timer_controller_test, tick_periodicity_for_scs15khz_is_1msec)
{
  auto cell_source = timer_ctrl.add_cell(to_du_cell_index(0));

  slot_point     cur_sl_tx = {subcarrier_spacing::kHz15, 0};
  const unsigned nof_slots = 40;
  for (unsigned i = 0; i != nof_slots; ++i) {
    cell_source->on_slot_indication(cur_sl_tx);
    task_worker.run_pending_tasks();
    ASSERT_EQ(cell_source->now(), cur_sl_tx);
    ++cur_sl_tx;
  }

  ASSERT_EQ(timers.now(), nof_slots);
}

TEST_F(du_timer_controller_test, tick_periodicity_for_scs30khz_is_1msec)
{
  auto cell_source = timer_ctrl.add_cell(to_du_cell_index(0));

  slot_point     cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  const unsigned nof_slots = 40;
  for (unsigned i = 0; i != nof_slots; ++i) {
    cell_source->on_slot_indication(cur_sl_tx);
    task_worker.run_pending_tasks();
    ASSERT_EQ(cell_source->now(), cur_sl_tx);
    ++cur_sl_tx;
  }

  ASSERT_EQ(timers.now(), nof_slots / 2 - 1);
}

TEST_F(du_timer_controller_test, only_one_tick_per_msec_for_multiple_cells)
{
  auto cell_source1 = timer_ctrl.add_cell(to_du_cell_index(0));
  auto cell_source2 = timer_ctrl.add_cell(to_du_cell_index(1));

  slot_point     cur_sl_tx = {subcarrier_spacing::kHz30, 0};
  const unsigned nof_slots = 40;
  for (unsigned i = 0; i != nof_slots; ++i) {
    cell_source1->on_slot_indication(cur_sl_tx);
    cell_source2->on_slot_indication(cur_sl_tx);
    task_worker.run_pending_tasks();
    ASSERT_EQ(cell_source1->now(), cur_sl_tx);
    ASSERT_EQ(cell_source2->now(), cur_sl_tx);
    ++cur_sl_tx;
  }

  ASSERT_EQ(timers.now(), nof_slots / 2 - 1);
}
