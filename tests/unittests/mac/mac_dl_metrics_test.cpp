/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/mac/mac_dl/mac_dl_metric_handler.h"
#include "tests/test_doubles/mac/dummy_mac_metrics_notifier.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>

using namespace srsran;

class mac_dl_metric_handler_test : public ::testing::Test
{
protected:
  const std::chrono::milliseconds period{10};
  timer_manager                   timers{2};
  manual_task_worker              task_worker{16};
  dummy_mac_metrics_notifier      metric_notifier;
  mac_dl_metric_handler           metrics{period, metric_notifier, timers, task_worker};
};

TEST_F(mac_dl_metric_handler_test, cell_created_successfully)
{
  auto& cell_metrics1 = metrics.add_cell(to_du_cell_index(0), subcarrier_spacing::kHz15);
  ASSERT_TRUE(cell_metrics1.enabled());

  auto& cell_metrics2 = metrics.add_cell(to_du_cell_index(1), subcarrier_spacing::kHz15);
  ASSERT_TRUE(cell_metrics2.enabled());
  ASSERT_NE(&cell_metrics2, &cell_metrics1);
}

TEST_F(mac_dl_metric_handler_test, for_single_cell_on_period_elapsed_then_report_is_generated)
{
  auto& cell_metrics = metrics.add_cell(to_du_cell_index(0), subcarrier_spacing::kHz15);

  // Number of slots equal to period has elapsed.
  for (unsigned i = 0; i != period.count(); ++i) {
    task_worker.run_pending_tasks();
    auto meas = cell_metrics.start_slot();
  }

  ASSERT_FALSE(metric_notifier.last_report.has_value());
  task_worker.run_pending_tasks();
  ASSERT_TRUE(metric_notifier.last_report.has_value());
}

TEST_F(mac_dl_metric_handler_test, for_multi_cell_then_report_generated_when_all_cells_generated_report)
{
  auto& cell_metrics1 = metrics.add_cell(to_du_cell_index(0), subcarrier_spacing::kHz15);
  auto& cell_metrics2 = metrics.add_cell(to_du_cell_index(1), subcarrier_spacing::kHz15);

  // Number of slots equal to period has elapsed.
  for (unsigned i = 0; i != period.count() - 1; ++i) {
    task_worker.run_pending_tasks();
    auto meas1 = cell_metrics1.start_slot();
    auto meas2 = cell_metrics2.start_slot();
  }

  // All but one cell have produced the report.
  {
    auto meas = cell_metrics2.start_slot();
  }
  task_worker.run_pending_tasks();
  ASSERT_FALSE(metric_notifier.last_report.has_value());

  // Last cell makes report.
  {
    auto meas = cell_metrics1.start_slot();
  }
  task_worker.run_pending_tasks();
  ASSERT_TRUE(metric_notifier.last_report.has_value());
}
