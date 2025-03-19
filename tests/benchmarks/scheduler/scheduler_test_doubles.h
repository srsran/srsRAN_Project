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

#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

class sched_cfg_dummy_notifier : public sched_configuration_notifier
{
public:
  void on_cell_removal_complete(du_cell_index_t cell_index) override {}
  void on_ue_config_complete(du_ue_index_t ue_index, bool ue_creation_result) override {}
  void on_ue_delete_response(du_ue_index_t ue_index) override {}
};

class sched_dummy_metric_notifier final : public scheduler_metrics_notifier
{
public:
  void report_metrics(const scheduler_cell_metrics& metrics) override {}
};

} // namespace srsran
