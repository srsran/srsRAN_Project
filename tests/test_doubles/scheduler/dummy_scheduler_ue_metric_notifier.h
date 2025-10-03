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
public:
  const std::optional<scheduler_cell_metrics>& last() const { return last_metrics; }

  void request_metrics() { metrics_requested = true; }

  scheduler_cell_metrics& get_next() override { return current_metrics; }

  void commit(scheduler_cell_metrics& ptr) override
  {
    srsran_assert(&ptr == &current_metrics, "Invalid reference passed");
    last_metrics = current_metrics;
    current_metrics.ue_metrics.clear();
    current_metrics.events.clear();
  }

  bool is_sched_report_required(slot_point sl_tx) const override
  {
    bool ret          = metrics_requested;
    metrics_requested = false;
    return ret;
  }

private:
  scheduler_cell_metrics                current_metrics{};
  std::optional<scheduler_cell_metrics> last_metrics;
  mutable bool                          metrics_requested = false;
};

} // namespace srsran
