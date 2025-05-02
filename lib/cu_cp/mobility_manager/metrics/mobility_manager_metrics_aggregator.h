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
