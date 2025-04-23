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
namespace ofh {

struct tx_data_flow_perf_metrics;

/// Open Fronthaul transmitter data flow performance metrics collector interface.
class data_flow_message_encoding_metrics_collector
{
public:
  virtual ~data_flow_message_encoding_metrics_collector() = default;

  /// Collect the performance metrics of a data flow.
  virtual void collect_metrics(tx_data_flow_perf_metrics& metric) = 0;
};

} // namespace ofh
} // namespace srsran
