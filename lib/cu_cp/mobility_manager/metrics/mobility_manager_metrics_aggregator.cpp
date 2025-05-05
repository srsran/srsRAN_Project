/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

mobility_management_metrics mobility_manager_metrics_aggregator::request_metrics_report() const
{
  return aggregated_mobility_manager_metrics;
}
