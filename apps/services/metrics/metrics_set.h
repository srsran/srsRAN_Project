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

namespace srsran {
namespace app_services {

class metrics_properties;

/// General purpouse metrics set.
class metrics_set
{
public:
  /// Default destructor.
  virtual ~metrics_set() = default;

  /// Returns this metrics properties.
  virtual const metrics_properties& get_properties() const = 0;
};

} // namespace app_services
} // namespace srsran
