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

namespace srsran {
namespace app_services {

class metrics_set;

/// Notifies new metrics.
class metrics_notifier
{
public:
  /// Default destructor.
  virtual ~metrics_notifier() = default;

  /// Notifies the given metrics.
  virtual void on_new_metric(const metrics_set& metric) = 0;
};

} // namespace app_services
} // namespace srsran
