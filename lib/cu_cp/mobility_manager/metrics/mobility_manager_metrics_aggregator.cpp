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

#include "mobility_manager_metrics_aggregator.h"

using namespace srsran;
using namespace srs_cu_cp;

void mobility_manager_metrics_aggregator::aggregate_requested_handover_preparation()
{
  ++aggregated_mobility_manager_metrics.nof_handover_preparations_requested;
}

void mobility_manager_metrics_aggregator::aggregate_successful_handover_preparation()
{
  ++aggregated_mobility_manager_metrics.nof_successful_handover_preparations;
}

/// \brief Aggregates the metrics for the requested handover execution.
void mobility_manager_metrics_aggregator::aggregate_requested_handover_execution()
{
  ++aggregated_mobility_manager_metrics.nof_handover_executions_requested;
}

/// \brief Aggregates the metrics for the successful handover execution.
void mobility_manager_metrics_aggregator::aggregate_successful_handover_execution()
{
  ++aggregated_mobility_manager_metrics.nof_successful_handover_executions;
}

mobility_management_metrics mobility_manager_metrics_aggregator::request_metrics_report() const
{
  return aggregated_mobility_manager_metrics;
}
