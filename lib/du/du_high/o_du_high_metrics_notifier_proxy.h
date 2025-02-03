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

#include "srsran/du/du_high/o_du_high_metrics.h"
#include "srsran/du/du_high/o_du_high_metrics_notifier.h"
#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {
namespace srs_du {

/// Proxy class that listen to scheduler metrics and forwards them to the configured scheduler metrics notifier and O-DU
/// high metrics notifier.
class o_du_high_metrics_notifier_proxy : public scheduler_metrics_notifier
{
  scheduler_metrics_notifier* sched_notifier  = nullptr;
  o_du_high_metrics_notifier* odu_hi_notifier = nullptr;

public:
  explicit o_du_high_metrics_notifier_proxy(scheduler_metrics_notifier* notifier_);

  // See interface for documentation.
  void report_metrics(const scheduler_cell_metrics& report) override;

  /// Sets the O-DU high metrics notifier to the given one.
  void set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier);
};

} // namespace srs_du
} // namespace srsran
