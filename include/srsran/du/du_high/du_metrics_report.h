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

#include "srsran/f1ap/du/f1ap_du_metrics_collector.h"
#include "srsran/mac/mac_metrics.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include <chrono>

namespace srsran {
namespace srs_du {

/// \brief Report of the DU metrics.
struct du_metrics_report {
  unsigned                                           version = 0;
  std::chrono::time_point<std::chrono::steady_clock> start_time;
  std::chrono::milliseconds                          period;
  std::optional<f1ap_metrics_report>                 f1ap;
  std::optional<mac_metric_report>                   mac;
};

} // namespace srs_du
} // namespace srsran
