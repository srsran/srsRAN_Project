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

#include "srsran/cu_cp/mobility_management_metrics.h"

namespace srsran::srs_cu_cp {

class mobility_manager_metrics_aggregator
{
public:
  /// \brief Aggregates the metrics for the requested handover preparation.
  void aggregate_requested_handover_preparation();

  /// \brief Aggregates the metrics for the successful handover preparation.
  void aggregate_successful_handover_preparation();

  /// \brief Aggregates the metrics for the requested handover execution.
  void aggregate_requested_handover_execution();

  /// \brief Aggregates the metrics for the successful handover execution.
  void aggregate_successful_handover_execution();

  mobility_management_metrics request_metrics_report() const;

private:
  mobility_management_metrics aggregated_mobility_manager_metrics;
};

} // namespace srsran::srs_cu_cp
