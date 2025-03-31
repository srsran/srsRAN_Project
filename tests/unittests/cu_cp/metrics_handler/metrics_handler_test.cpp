/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/cu_cp/metrics_handler/metrics_handler_impl.h"
#include "lib/cu_cp/ngap_repository.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class dummy_ue_metrics_handler : public ue_metrics_handler,
                                 public du_repository_metrics_handler,
                                 public ngap_repository_metrics_handler
{
public:
  metrics_report next_metrics;

  std::vector<metrics_report::ue_info> handle_ue_metrics_report_request() const override { return next_metrics.ues; }

  std::vector<metrics_report::du_info> handle_du_metrics_report_request() const override { return next_metrics.dus; }

  std::vector<metrics_report::ngap_info> handle_ngap_metrics_report_request() const override
  {
    return next_metrics.ngaps;
  }
};

class dummy_metrics_notifier : public metrics_report_notifier
{
public:
  std::optional<metrics_report> last_metrics_report;

  void notify_metrics_report_request(const metrics_report& report) override { last_metrics_report = report; }
};

TEST(metrics_handler_test, get_periodic_metrics_report_while_session_is_active)
{
  manual_task_worker       worker{16};
  timer_manager            timers{2};
  dummy_ue_metrics_handler metrics_hdlr;
  metrics_handler_impl     metrics{worker, timers, metrics_hdlr, metrics_hdlr, metrics_hdlr};

  std::chrono::milliseconds period{5};
  dummy_metrics_notifier    metrics_notifier;
  auto session = metrics.create_periodic_report_session(periodic_metric_report_request{period, &metrics_notifier});

  // First report.
  metrics_hdlr.next_metrics.ues.emplace_back(
      metrics_report::ue_info{to_rnti(1), int_to_gnb_du_id(0), pci_t{2}, rrc_state::connected});
  metrics_hdlr.next_metrics.dus.emplace_back(metrics_report::du_info{
      int_to_gnb_du_id(0),
      {metrics_report::cell_info{
          nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(0x22).value()}, pci_t{2}}},
      {2, 4}});

  for (unsigned i = 0; i != period.count(); ++i) {
    ASSERT_FALSE(metrics_notifier.last_metrics_report.has_value());
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_TRUE(metrics_notifier.last_metrics_report.has_value());
  ASSERT_EQ(metrics_notifier.last_metrics_report->ues.size(), 1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ues[0].rnti, metrics_hdlr.next_metrics.ues[0].rnti);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ues[0].pci, metrics_hdlr.next_metrics.ues[0].pci);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ues[0].rrc_connection_state,
            metrics_hdlr.next_metrics.ues[0].rrc_connection_state);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus.size(), 1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus[0].rrc_metrics.mean_nof_rrc_connections, 2);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus[0].rrc_metrics.max_nof_rrc_connections, 4);

  // Second report.
  metrics_notifier.last_metrics_report.reset();
  metrics_hdlr.next_metrics.ues.emplace_back(
      metrics_report::ue_info{to_rnti(2), int_to_gnb_du_id(0), pci_t{3}, rrc_state::connected});
  for (unsigned i = 0; i != period.count(); ++i) {
    ASSERT_FALSE(metrics_notifier.last_metrics_report.has_value());
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_TRUE(metrics_notifier.last_metrics_report.has_value());
  ASSERT_EQ(metrics_notifier.last_metrics_report->ues.size(), 2);

  // Destroy session.
  metrics_notifier.last_metrics_report.reset();
  session.reset();
  for (unsigned i = 0; i != period.count(); ++i) {
    ASSERT_FALSE(metrics_notifier.last_metrics_report.has_value());
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_FALSE(metrics_notifier.last_metrics_report.has_value());
}
