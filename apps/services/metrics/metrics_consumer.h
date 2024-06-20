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
class metrics_properties;

/// \brief Metrics subscriber.
///
/// A subscriber will listen to a configured metric.
class metrics_consumer
{
public:
  /// Default destructor.
  virtual ~metrics_consumer() = default;

  /// Handles the given metrics.
  virtual void handle_metric(const metrics_set& metric) = 0;
};

} // namespace app_services
} // namespace srsran
