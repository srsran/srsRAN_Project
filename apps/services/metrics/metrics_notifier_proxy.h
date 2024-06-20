/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/services/metrics/metrics_notifier.h"

namespace srsran {
namespace app_services {

class metrics_set;

/// Metrics notifier proxy implementation.
class metrics_notifier_proxy_impl : public metrics_notifier
{
  metrics_notifier* proxy = nullptr;

public:
  // See interface for documentation.
  void on_new_metric(const metrics_set& metric) override
  {
    if (proxy) {
      proxy->on_new_metric(metric);
    }
  }

  void connect(metrics_notifier& notifier) { proxy = &notifier; }
};

} // namespace app_services
} // namespace srsran
