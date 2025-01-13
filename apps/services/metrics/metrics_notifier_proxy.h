/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
