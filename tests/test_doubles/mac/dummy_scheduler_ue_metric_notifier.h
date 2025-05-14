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

#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

/// Sink for scheduler UE metrics.
class dummy_scheduler_ue_metrics_notifier : public scheduler_metrics_notifier
{
public:
  scheduler_cell_metrics last_metrics;

  void report_metrics(const scheduler_cell_metrics& metrics) override { last_metrics = metrics; }
};

class dummy_scheduler_cell_metrics_notifier : public scheduler_cell_metrics_notifier
{
private:
  scheduler_cell_metrics last_metrics;

  scheduler_cell_metrics& get_next() override { return last_metrics; }

  void commit(scheduler_cell_metrics& ptr) override
  {
    srsran_assert(&ptr == &last_metrics, "Invalid reference passed");
  }
};

} // namespace srsran
