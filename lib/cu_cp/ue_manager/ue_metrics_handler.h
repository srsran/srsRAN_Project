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

#include "srsran/cu_cp/cu_cp_metrics_handler.h"

namespace srsran {
namespace srs_cu_cp {

/// Interface used to capture the UE metrics from the CU-CP UE manager.
class ue_metrics_handler
{
public:
  virtual ~ue_metrics_handler() = default;

  /// \brief Handle new request for UE metrics.
  virtual std::vector<metrics_report::ue_info> handle_ue_metrics_report_request() const = 0;
};

} // namespace srs_cu_cp
} // namespace srsran