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

  /// This method can be called periodically to process accumulated metrics and produce the metrics to be consumed by
  /// the corresponding metrics consumers.
  virtual void on_new_report_period() = 0;
};

} // namespace app_services
} // namespace srsran
