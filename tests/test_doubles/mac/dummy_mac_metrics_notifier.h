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

#include "srsran/mac/mac_metrics.h"

namespace srsran {

class dummy_mac_metrics_notifier : public mac_metrics_notifier
{
public:
  std::optional<mac_metric_report> last_report;

  void on_new_metrics_report(const mac_metric_report& report) override { last_report = report; }
};

} // namespace srsran
