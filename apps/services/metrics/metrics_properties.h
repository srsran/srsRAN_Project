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

#include <string_view>

namespace srsran {
namespace app_services {

/// Metrics properties.
class metrics_properties
{
public:
  /// Default destructor.
  virtual ~metrics_properties() = default;

  /// Returns the metrics name.
  virtual std::string_view name() const = 0;
};

} // namespace app_services
} // namespace srsran
