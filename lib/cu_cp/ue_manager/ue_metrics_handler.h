/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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