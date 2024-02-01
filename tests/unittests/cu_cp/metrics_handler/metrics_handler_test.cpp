/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/cu_cp/metrics_handler/metrics_handler_impl.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class dummy_ue_metrics_handler : public ue_metrics_handler
{
public:
  ue_metrics_report next_metrics;

  ue_metrics_report handle_ue_metrics_report_request() override { return next_metrics; }
};

class dummy_metrics_notifier : public metrics_report_notifier
{
public:
  optional<metrics_report> last_metrics_report;

  void notify_metrics_report_request(const metrics_report& report) override { last_metrics_report = report; }
};

TEST(metrics_handler_test, get_periodic_metrics_report_while_session_is_active)
{
  manual_task_worker       worker{16};
  timer_manager            timers{2};
  dummy_ue_metrics_handler ue_metrics;
  metrics_handler_impl     metrics{worker, timers, ue_metrics};

  std::chrono::milliseconds period{5};
  dummy_metrics_notifier    notif;
  auto session = metrics.create_periodic_report_session(periodic_metric_report_request{period, &notif});

  // First report.
  ue_metrics.next_metrics.ues.emplace_back(ue_metrics_report::ue_context{to_rnti(1), pci_t{2}});
  for (unsigned i = 0; i != period.count(); ++i) {
    ASSERT_FALSE(notif.last_metrics_report.has_value());
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_TRUE(notif.last_metrics_report.has_value());
  ASSERT_EQ(notif.last_metrics_report->ue_metrics.ues.size(), 1);
  ASSERT_EQ(notif.last_metrics_report->ue_metrics.ues[0].rnti, ue_metrics.next_metrics.ues[0].rnti);
  ASSERT_EQ(notif.last_metrics_report->ue_metrics.ues[0].pci, ue_metrics.next_metrics.ues[0].pci);

  // Second report.
  notif.last_metrics_report.reset();
  ue_metrics.next_metrics.ues.emplace_back(ue_metrics_report::ue_context{to_rnti(2), pci_t{3}});
  for (unsigned i = 0; i != period.count(); ++i) {
    ASSERT_FALSE(notif.last_metrics_report.has_value());
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_TRUE(notif.last_metrics_report.has_value());
  ASSERT_EQ(notif.last_metrics_report->ue_metrics.ues.size(), 2);

  // Destroy session.
  notif.last_metrics_report.reset();
  session.reset();
  for (unsigned i = 0; i != period.count(); ++i) {
    ASSERT_FALSE(notif.last_metrics_report.has_value());
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_FALSE(notif.last_metrics_report.has_value());
}