/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
class dummy_scheduler_ue_metrics_notifier : public scheduler_ue_metrics_notifier
{
public:
  std::vector<scheduler_ue_metrics> last_ue_metrics;

  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override
  {
    last_ue_metrics = std::vector<scheduler_ue_metrics>(ue_metrics.begin(), ue_metrics.end());
  }
};

} // namespace srsran