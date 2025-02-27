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

#include "lib/mac/mac_dl/mac_dl_metric_handler.h"
#include "tests/test_doubles/mac/dummy_mac_metrics_notifier.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>

using namespace srsran;

static void print_report(const mac_dl_metric_report& rep)
{
  fmt::print("New report:\n", rep.cells.size());
  for (unsigned i = 0; i != rep.cells.size(); ++i) {
    auto& cell = rep.cells[i];
    fmt::print("- cell={}: slots={} wall_latency={{avg={}, min={}, max={}}}nsec, user_latency={{avg={}, min={}, "
               "max={}}}, sys_latency={{avg={}, min={}, max={}}}, vol_ctx_switches={} invol_ctx_switches={}\n",
               i,
               cell.nof_slots,
               cell.wall_clock_latency.average.count(),
               cell.wall_clock_latency.min.count(),
               cell.wall_clock_latency.max.count(),
               cell.user_time.average.count(),
               cell.user_time.min.count(),
               cell.user_time.max.count(),
               cell.sys_time.average.count(),
               cell.sys_time.min.count(),
               cell.sys_time.max.count(),
               cell.count_voluntary_context_switches,
               cell.count_involuntary_context_switches);
  }
}

class mac_dl_metric_handler_test : public ::testing::Test
{
protected:
  const std::chrono::milliseconds period{10};
  timer_manager                   timers{2};
  manual_task_worker              task_worker{16};
  dummy_mac_metrics_notifier      metric_notifier;
  mac_dl_metric_handler           metrics{period, metric_notifier, timers, task_worker};

  slot_point next_point{0, 1};
};

TEST_F(mac_dl_metric_handler_test, cell_created_successfully)
{
  auto& cell_metrics1 = metrics.add_cell(to_du_cell_index(0), 1, subcarrier_spacing::kHz15);
  ASSERT_TRUE(cell_metrics1.enabled());

  auto& cell_metrics2 = metrics.add_cell(to_du_cell_index(1), 2, subcarrier_spacing::kHz15);
  ASSERT_TRUE(cell_metrics2.enabled());
  ASSERT_NE(&cell_metrics2, &cell_metrics1);
}

TEST_F(mac_dl_metric_handler_test, for_single_cell_on_period_elapsed_then_report_is_generated)
{
  auto& cell_metrics = metrics.add_cell(to_du_cell_index(0), 1, subcarrier_spacing::kHz15);

  // Number of slots equal to period has elapsed.
  for (unsigned i = 0; i != period.count(); ++i) {
    task_worker.run_pending_tasks();
    auto meas = cell_metrics.start_slot(next_point, metric_clock::now());
    ++next_point;
  }

  ASSERT_FALSE(metric_notifier.last_report.has_value());
  task_worker.run_pending_tasks();
  ASSERT_TRUE(metric_notifier.last_report.has_value());
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells.size(), 1);
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells[0].nof_slots, period.count());
  auto expected_slot_dur = std::chrono::nanoseconds{std::chrono::milliseconds{1}} / next_point.nof_slots_per_subframe();
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells[0].slot_duration, expected_slot_dur);
}

TEST_F(mac_dl_metric_handler_test, when_multi_cell_then_mac_report_generated_when_all_cells_generated_report)
{
  auto& cell_metrics1 = metrics.add_cell(to_du_cell_index(0), 1, subcarrier_spacing::kHz15);
  auto& cell_metrics2 = metrics.add_cell(to_du_cell_index(1), 2, subcarrier_spacing::kHz15);

  // Number of slots equal to period-1 has elapsed.
  for (unsigned i = 0; i != period.count() - 1; ++i) {
    task_worker.run_pending_tasks();
    auto meas1 = cell_metrics1.start_slot(next_point, metric_clock::now());
    auto meas2 = cell_metrics2.start_slot(next_point, metric_clock::now());
    ++next_point;
  }

  // All but one cell have produced the report.
  {
    auto meas = cell_metrics2.start_slot(next_point, metric_clock::now());
  }
  task_worker.run_pending_tasks();
  ASSERT_FALSE(metric_notifier.last_report.has_value());

  // Last cell makes report.
  {
    auto meas = cell_metrics1.start_slot(next_point, metric_clock::now());
  }
  task_worker.run_pending_tasks();
  ASSERT_TRUE(metric_notifier.last_report.has_value());
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells.size(), 2);
  auto& cells = metric_notifier.last_report.value().dl.cells;
  ASSERT_EQ(cells[0].nof_slots, period.count());
  ASSERT_GE(cells[0].wall_clock_latency.max, cells[0].wall_clock_latency.average);
  ASSERT_GE(cells[0].wall_clock_latency.average, cells[0].wall_clock_latency.min);
  ASSERT_EQ(cells[1].nof_slots, period.count());
  ASSERT_GE(cells[1].wall_clock_latency.max, cells[1].wall_clock_latency.average);
  ASSERT_GE(cells[1].wall_clock_latency.average, cells[1].wall_clock_latency.min);

  print_report(metric_notifier.last_report.value().dl);
}

TEST_F(mac_dl_metric_handler_test, when_multi_cell_creation_staggered_then_reports_are_aligned_in_slot)
{
  auto& cell_metrics1 = metrics.add_cell(to_du_cell_index(0), 1, subcarrier_spacing::kHz15);

  // Number of slots lower than period has elapsed.
  const unsigned count_until_cell2 = period.count() - 5;
  for (unsigned i = 0; i != count_until_cell2; ++i) {
    task_worker.run_pending_tasks();
    auto meas1 = cell_metrics1.start_slot(next_point, metric_clock::now());
    ++next_point;
  }
  task_worker.run_pending_tasks();
  ASSERT_FALSE(metric_notifier.last_report.has_value());

  // Cell 2 is created and we run the remaining slots.
  auto& cell_metrics2 = metrics.add_cell(to_du_cell_index(1), 2, subcarrier_spacing::kHz15);
  for (unsigned i = 0; i != period.count() - count_until_cell2; ++i) {
    task_worker.run_pending_tasks();
    auto meas1 = cell_metrics1.start_slot(next_point, metric_clock::now());
    auto meas2 = cell_metrics2.start_slot(next_point, metric_clock::now());
    ++next_point;
  }

  ASSERT_FALSE(metric_notifier.last_report.has_value());
  task_worker.run_pending_tasks();
  ASSERT_TRUE(metric_notifier.last_report.has_value());
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells.size(), 2);
  auto& cells = metric_notifier.last_report.value().dl.cells;
  ASSERT_EQ(cells[0].nof_slots, period.count());
  ASSERT_EQ(cells[1].nof_slots, period.count() - count_until_cell2);

  print_report(metric_notifier.last_report.value().dl);
}
