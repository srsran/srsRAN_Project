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
#include "srsran/du/du_high/du_manager/du_manager_params.h"
#include "srsran/du/du_high/du_metrics_notifier.h"

namespace srsran {

class mac_metrics_notifier;
class scheduler_metrics_notifier;

namespace srs_du {

class du_manager_metrics_collector_impl final : public du_manager_metrics_collector
{
public:
  du_manager_metrics_collector_impl(const du_manager_params::metrics_config_params& params_,
                                    task_executor&                                  du_mng_exec_,
                                    timer_manager&                                  timers_,
                                    f1ap_metrics_collector&                         f1ap_collector_,
                                    mac_metrics_notifier*                           mac_notifier_,
                                    scheduler_metrics_notifier*                     sched_notifier_);

  void handle_mac_metrics_report(const mac_metric_report& report) override;

  void handle_scheduler_metrics_report(const scheduler_cell_metrics& report) override;

private:
  void trigger_report();

  const du_manager_params::metrics_config_params params;
  task_executor&                                 du_mng_exec;
  timer_manager&                                 timers;
  mac_metrics_notifier*                          mac_notifier   = nullptr;
  scheduler_metrics_notifier*                    sched_notifier = nullptr;
  f1ap_metrics_collector&                        f1ap_collector;

  unsigned          next_version = 0;
  du_metrics_report next_report{};
};

} // namespace srs_du
} // namespace srsran
