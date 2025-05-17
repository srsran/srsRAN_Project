/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/mac/mac_ctrl/du_time_controller.h"
#include "lib/mac/mac_ctrl/mac_metrics_aggregator.h"
#include "lib/mac/mac_dl/mac_dl_metric_handler.h"
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
  dummy_sched_metric_handler(scheduler_cell_metrics_notifier& notif_, unsigned period_slots_) :
    notif(notif_), period_slots(period_slots_)
  {
  }

  void slot_indication(slot_point sl_tx)
  {
    if (not next_report_slot.valid()) {
      unsigned sl_mod  = sl_tx.to_uint() % period_slots;
      next_report_slot = sl_tx + period_slots - sl_mod;
    }
    report_slot_count++;

    if (sl_tx == next_report_slot) {
      auto builder       = notif.get_builder();
      builder->slot      = sl_tx - report_slot_count;
      builder->nof_slots = report_slot_count;
      builder.reset();
      next_report_slot += period_slots;
      next_hfn += (period_slots / (NOF_SFNS * NOF_SUBFRAMES_PER_FRAME));
      if (next_report_slot.to_uint() < sl_tx.to_uint()) {
        next_hfn += 1;
      }
      report_slot_count = 0;
    }
  }

  scheduler_cell_metrics_notifier& notif;
  unsigned                         period_slots;
  slot_point                       next_report_slot;
  unsigned                         next_hfn{0};
  unsigned                         report_slot_count{0};
};

class mac_metric_handler_test : public ::testing::Test
{
protected:
  const std::chrono::milliseconds period{10};
  const subcarrier_spacing        scs = subcarrier_spacing::kHz15;
  const unsigned                  period_slots{static_cast<unsigned>(period.count() * get_nof_slots_per_subframe(scs))};
  unsigned aggr_timeout_slots = mac_metrics_aggregator::aggregation_timeout.count() * get_nof_slots_per_subframe(scs);
  timer_manager                       timers{2};
  manual_task_worker                  task_worker{16};
  dummy_mac_metrics_notifier          metric_notifier;
  dummy_scheduler_ue_metrics_notifier sched_notifier;
  mac_metrics_aggregator              metrics{period,
                                 metric_notifier,
                                 &sched_notifier,
                                 task_worker,
                                 timers,
                                 srslog::fetch_basic_logger("MAC", true)};

  slotted_id_table<du_cell_index_t, mac_dl_cell_metric_handler, MAX_CELLS_PER_DU> cells;
  slotted_id_table<du_cell_index_t, dummy_sched_metric_handler, MAX_CELLS_PER_DU> sched_cells;

  slot_point next_point{0, 1};

  mac_dl_cell_metric_handler& add_cell(du_cell_index_t cell_index)
  {
    pci_t pci         = static_cast<unsigned>(cell_index);
    auto  metrics_cfg = metrics.add_cell(to_du_cell_index(cell_index), scs);
    cells.emplace(cell_index,
                  pci,
                  scs,
                  mac_cell_config_dependencies{nullptr, metrics_cfg.report_period, metrics_cfg.mac_notifier});
    sched_cells.emplace(cell_index, *metrics_cfg.sched_notifier, period_slots);
    return cells[cell_index];
  }

  void run_slot()
  {
    for (auto& cell : sched_cells) {
      cell.slot_indication(next_point);
    }
    for (auto& cell : cells) {
      auto meas = cell.start_slot(next_point, metric_clock::now());
    }
    timers.tick();
    task_worker.run_pending_tasks();
    ++next_point;
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
  auto& cellgen = add_cell(to_du_cell_index(0));
  cellgen.on_cell_activation();

  // Number of slots equal to period+timeout has elapsed.
  unsigned wait_slots = period_slots + aggr_timeout_slots - 1;
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

TEST_F(mac_metric_handler_test, when_multi_cell_then_mac_report_generated_when_all_cells_generated_report)
{
  auto& cellgen1 = add_cell(to_du_cell_index(0));
  auto& cellgen2 = add_cell(to_du_cell_index(1));
  cellgen1.on_cell_activation();
  cellgen2.on_cell_activation();

  // No report is ready until report period + timeout is reached.
  unsigned wait_slots = period_slots + aggr_timeout_slots - 1;
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
  auto& cellgen1 = add_cell(to_du_cell_index(0));
  cellgen1.on_cell_activation();

  // Number of slots lower than period + timeout has elapsed.
  const unsigned count_until_cell2 = test_rgen::uniform_int<unsigned>(0, period_slots - 2);
  for (unsigned i = 0; i != count_until_cell2; ++i) {
    ASSERT_FALSE(metric_notifier.last_report.has_value());
    run_slot();
  }

  // Cell 2 is created and we run the remaining slots of the period+timeout window.
  auto& cellgen2 = add_cell(to_du_cell_index(1));
  cellgen2.on_cell_activation();
  unsigned wait_slots = period_slots + aggr_timeout_slots - 1;
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
  auto& cellgen1 = add_cell(to_du_cell_index(0));
  auto& cellgen2 = add_cell(to_du_cell_index(1));
  cellgen1.on_cell_activation();
  cellgen2.on_cell_activation();

  const unsigned count_until_cell_rem = test_rgen::uniform_int<unsigned>(1, period_slots - 1);
  for (unsigned i = 0; i != count_until_cell_rem; ++i) {
    ASSERT_FALSE(metric_notifier.last_report.has_value());
    run_slot();
  }

  // Cell1 is deactivated.
  cellgen1.on_cell_deactivation();
  unsigned wait_slots = period_slots + aggr_timeout_slots - 1;
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
