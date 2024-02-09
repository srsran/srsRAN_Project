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

#include "srsran/cu_cp/cu_cp_metrics_handler.h"

namespace srsran {
namespace srs_cu_cp {

/// Interface used to capture the DU metrics from a single CU-CP DU.
class du_metrics_handler
{
public:
  virtual ~du_metrics_handler() = default;

  /// \brief Handle new request for UE metrics.
  virtual metrics_report::du_info handle_du_metrics_report_request() const = 0;
};

/// Interface used to capture the DU metrics from all the connected DUs to the CU-CP.
class du_repository_metrics_handler
{
public:
  virtual ~du_repository_metrics_handler() = default;

  /// \brief Handle new request for UE metrics.
  virtual std::vector<metrics_report::du_info> handle_du_metrics_report_request() const = 0;
};

} // namespace srs_cu_cp
} // namespace srsran