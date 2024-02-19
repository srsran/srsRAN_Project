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

/// Interface used to capture the DU metrics from a single CU-CP DU.
class du_metrics_handler
{
public:
  virtual ~du_metrics_handler() = default;

  /// \brief Handle new request for metrics relative to a connected DU.
  virtual metrics_report::du_info handle_du_metrics_report_request() const = 0;
};

/// Interface used to capture the DU metrics from all the connected DUs to the CU-CP.
class du_repository_metrics_handler
{
public:
  virtual ~du_repository_metrics_handler() = default;

  /// \brief Handle new metrics request for all the DU nodes connected to the CU-CP.
  virtual std::vector<metrics_report::du_info> handle_du_metrics_report_request() const = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
