/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_metrics_collector_impl.h"
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/scheduler/scheduler_metrics.h"

using namespace srsran;
using namespace srs_du;

namespace {

class mac_metrics_null_notifier final : public mac_metrics_notifier
{
public:
  void on_new_metrics_report(const mac_metric_report& metrics) override
  {
    // do nothing.
  }
};

class scheduler_ue_metrics_null_notifier final : public scheduler_metrics_notifier
{
public:
  void report_metrics(const scheduler_cell_metrics& report) override
  {
    // do nothing.
  }
};

// Null notifier for mac metrics, used in case no mac metrics notifier is provided in the config.
mac_metrics_null_notifier null_mac_notifier;

// Null notifier for scheduler metrics, used in case no scheduler metrics notifier is provided in the config.
scheduler_ue_metrics_null_notifier null_sched_notifier;

} // namespace

du_manager_metrics_collector_impl::du_manager_metrics_collector_impl(mac_metrics_notifier*       mac_notifier_,
                                                                     scheduler_metrics_notifier* sched_notifier_) :
  mac_notifier(mac_notifier_ != nullptr ? *mac_notifier_ : null_mac_notifier),
  sched_notifier(sched_notifier_ != nullptr ? *sched_notifier_ : null_sched_notifier)
{
}

void du_manager_metrics_collector_impl::handle_mac_metrics_report(const mac_metric_report& report)
{
  mac_notifier.on_new_metrics_report(report);
}

void du_manager_metrics_collector_impl::handle_scheduler_metrics_report(const scheduler_cell_metrics& report)
{
  sched_notifier.report_metrics(report);
}
