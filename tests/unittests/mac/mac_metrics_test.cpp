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

#include "lib/mac/mac_ctrl/mac_metrics_aggregator.h"
#include "lib/mac/mac_dl/mac_dl_metric_handler.h"
#include "mac_test_helpers.h"
#include "tests/test_doubles/mac/dummy_mac_metrics_notifier.h"
#include "tests/test_doubles/mac/dummy_scheduler_ue_metric_notifier.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/test_utils.h"
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

namespace {

struct dummy_sched_metric_handler {
  dummy_sched_metric_handler(scheduler_cell_metrics_notifier& notif_) : notif(notif_) { builder = notif.get_builder(); }

  void slot_indication(slot_point sl_tx)
  {
    report_slot_count++;

    if (notif.is_sched_report_required(sl_tx)) {
      builder->slot      = sl_tx - report_slot_count;
      builder->nof_slots = report_slot_count;
      builder.reset();
      builder           = notif.get_builder();
      report_slot_count = 0;
    }
  }

  scheduler_cell_metrics_notifier& notif;
  unsigned                         report_slot_count{0};

  zero_copy_notifier<scheduler_cell_metrics>::builder builder;
};

class mac_metric_handler_test : public ::testing::Test
{
protected:
  struct cell_context {
    bool                                       active = true;
    dummy_sched_metric_handler                 sched;
    mac_dl_cell_metric_handler                 mac;
    std::unique_ptr<mac_cell_clock_controller> timer_source;

    cell_context(scheduler_cell_metrics_notifier&           sched_notif,
                 mac_cell_metric_notifier&                  mac_notif,
                 std::unique_ptr<mac_cell_clock_controller> timer_source_,
                 pci_t                                      pci,
                 subcarrier_spacing                         scs) :
      sched(sched_notif), mac(pci, scs, &mac_notif), timer_source(std::move(timer_source_))
    {
    }
  };

  const std::chrono::milliseconds period{10};
  const subcarrier_spacing        scs = subcarrier_spacing::kHz15;
  const unsigned                  period_slots{static_cast<unsigned>(period.count() * get_nof_slots_per_subframe(scs))};
  unsigned aggr_timeout_slots = mac_metrics_aggregator::aggregation_timeout.count() * get_nof_slots_per_subframe(scs);
  srslog::basic_logger&                    logger = srslog::fetch_basic_logger("MAC", true);
  timer_manager                            timers{2};
  manual_task_worker                       task_worker{16};
  test_helpers::dummy_mac_clock_controller timer_ctrl{timers};
  dummy_mac_metrics_notifier               metric_notifier;
  mac_metrics_aggregator                   metrics{mac_control_config::metrics_config{period, metric_notifier},
                                 task_worker,
                                 timers,
                                 logger};

  slotted_id_table<du_cell_index_t, cell_context, MAX_CELLS_PER_DU> cells;

  slot_point next_point{0, 1};

  mac_dl_cell_metric_handler& add_cell(du_cell_index_t cell_index)
  {
    pci_t pci         = static_cast<unsigned>(cell_index);
    auto  time_source = timer_ctrl.add_cell(cell_index);
    auto  metrics_cfg = metrics.add_cell(to_du_cell_index(cell_index), scs, *time_source);
    cells.emplace(cell_index, *metrics_cfg.sched_notifier, *metrics_cfg.mac_notifier, std::move(time_source), pci, scs);
    return cells[cell_index].mac;
  }

  void run_slot()
  {
    for (auto& cell : cells) {
      cell.timer_source->on_slot_indication(next_point);
      if (cell.active) {
        cell.sched.slot_indication(next_point);
        cell.mac.start_slot(next_point, metric_clock::now());
      }
    }
    task_worker.run_pending_tasks();
    ++next_point;
  }

  void deactivate_cell(du_cell_index_t cell_index)
  {
    cells[cell_index].active = false;
    cells[cell_index].mac.on_cell_deactivation();
  }
};

} // namespace

TEST_F(mac_metric_handler_test, cell_created_successfully)
{
  add_cell(to_du_cell_index(0));
  add_cell(to_du_cell_index(1));
}

TEST_F(mac_metric_handler_test, for_single_cell_on_period_elapsed_then_report_is_generated)
{
  add_cell(to_du_cell_index(0));

  // Number of slots equal to period+timeout has elapsed.
  unsigned wait_slots = period_slots + aggr_timeout_slots;
  for (unsigned i = 0; i != wait_slots; ++i) {
    ASSERT_FALSE(metric_notifier.last_report.has_value());
    run_slot();
  }
  ASSERT_TRUE(metric_notifier.last_report.has_value());
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells.size(), 1);
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells[0].nof_slots, period.count());
  auto expected_slot_dur = std::chrono::nanoseconds{std::chrono::milliseconds{1}} / next_point.nof_slots_per_subframe();
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells[0].slot_duration, expected_slot_dur);
}

TEST_F(mac_metric_handler_test, when_single_cell_and_deactivated_before_period_then_report_is_generated)
{
  add_cell(to_du_cell_index(0));

  // Number of slots equal to period+timeout has elapsed.
  unsigned wait_slots = period_slots - 1;
  for (unsigned i = 0; i != wait_slots; ++i) {
    ASSERT_FALSE(metric_notifier.last_report.has_value());
    run_slot();
  }
  ASSERT_FALSE(metric_notifier.last_report.has_value());

  deactivate_cell(to_du_cell_index(0));
  run_slot();
  ASSERT_TRUE(metric_notifier.last_report.has_value());
}

TEST_F(mac_metric_handler_test, when_multi_cell_then_mac_report_generated_when_all_cells_generated_report)
{
  add_cell(to_du_cell_index(0));
  add_cell(to_du_cell_index(1));

  // No report is ready until report period + timeout is reached.
  unsigned wait_slots = period_slots + aggr_timeout_slots;
  for (unsigned i = 0; i != wait_slots; ++i) {
    ASSERT_FALSE(metric_notifier.last_report.has_value());
    run_slot();
  }

  ASSERT_TRUE(metric_notifier.last_report.has_value());
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells.size(), 2);
  auto& rep_cells = metric_notifier.last_report.value().dl.cells;
  ASSERT_EQ(rep_cells[0].nof_slots, period.count());
  ASSERT_GE(rep_cells[0].wall_clock_latency.max, rep_cells[0].wall_clock_latency.average);
  ASSERT_GE(rep_cells[0].wall_clock_latency.average, rep_cells[0].wall_clock_latency.min);
  ASSERT_EQ(rep_cells[1].nof_slots, period.count());
  ASSERT_GE(rep_cells[1].wall_clock_latency.max, rep_cells[1].wall_clock_latency.average);
  ASSERT_GE(rep_cells[1].wall_clock_latency.average, rep_cells[1].wall_clock_latency.min);

  print_report(metric_notifier.last_report.value().dl);
}

TEST_F(mac_metric_handler_test, when_multi_cell_creation_staggered_then_reports_are_aligned_in_slot)
{
  add_cell(to_du_cell_index(0));

  // Number of slots lower than period + timeout has elapsed.
  const unsigned count_until_cell2 = test_rgen::uniform_int<unsigned>(0, period_slots - 2);
  for (unsigned i = 0; i != count_until_cell2; ++i) {
    ASSERT_FALSE(metric_notifier.last_report.has_value());
    run_slot();
  }

  // Cell 2 is created and we run the remaining slots of the period+timeout window.
  add_cell(to_du_cell_index(1));
  unsigned wait_slots = period_slots + aggr_timeout_slots;
  for (unsigned i = 0, e = wait_slots - count_until_cell2; i != e; ++i) {
    ASSERT_FALSE(metric_notifier.last_report.has_value());
    run_slot();
  }

  ASSERT_TRUE(metric_notifier.last_report.has_value());
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells.size(), 2);
  auto& rep_cells = metric_notifier.last_report.value().dl.cells;
  ASSERT_EQ(rep_cells[0].nof_slots, period_slots);
  ASSERT_EQ(rep_cells[1].nof_slots, period_slots - count_until_cell2);

  print_report(metric_notifier.last_report.value().dl);
}

TEST_F(mac_metric_handler_test, when_one_cell_gets_removed_then_last_report_still_contains_its_report)
{
  add_cell(to_du_cell_index(0));
  add_cell(to_du_cell_index(1));

  const unsigned count_until_cell_rem = test_rgen::uniform_int<unsigned>(1, period_slots);
  for (unsigned i = 0; i != count_until_cell_rem; ++i) {
    ASSERT_FALSE(metric_notifier.last_report.has_value());
    run_slot();
  }

  // Cell1 is deactivated.
  deactivate_cell(to_du_cell_index(0));
  unsigned wait_slots = period_slots + aggr_timeout_slots;
  for (unsigned i = 0, e = wait_slots - count_until_cell_rem; i != e; ++i) {
    ASSERT_FALSE(metric_notifier.last_report.has_value());
    run_slot();
  }

  // Report contains two cells, the deactivated one having a lower number of slots.
  ASSERT_TRUE(metric_notifier.last_report.has_value());
  ASSERT_EQ(metric_notifier.last_report.value().dl.cells.size(), 2);
  auto& rep_cells = metric_notifier.last_report.value().dl.cells;
  ASSERT_EQ(rep_cells[0].nof_slots, count_until_cell_rem);
  ASSERT_EQ(rep_cells[1].nof_slots, period_slots);
}
