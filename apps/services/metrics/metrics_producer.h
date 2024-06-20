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

class metrics_properties;

/// \brief Metrics producer.
///
/// Produces metrics that will be consumed by the corresponding metrics consumers.
class metrics_producer
{
public:
  /// Default destructor.
  virtual ~metrics_producer() = default;
};

} // namespace app_services
} // namespace srsran
