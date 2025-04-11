/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_high/du_manager/du_manager.h"

namespace srsran {

class mac_metrics_notifier;
class scheduler_metrics_notifier;

namespace srs_du {

class du_manager_metrics_collector_impl final : public du_manager_metrics_collector
{
public:
  du_manager_metrics_collector_impl(mac_metrics_notifier* mac_notifier_, scheduler_metrics_notifier* sched_notifier_);

  void handle_mac_metrics_report(const mac_metric_report& report) override;

  void handle_scheduler_metrics_report(const scheduler_cell_metrics& report) override;

private:
  mac_metrics_notifier&       mac_notifier;
  scheduler_metrics_notifier& sched_notifier;
};

} // namespace srs_du
} // namespace srsran
