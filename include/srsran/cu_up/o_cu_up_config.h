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

#include "srsran/cu_up/cu_up_config.h"
#include "srsran/e2/e2_cu.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/gateways/e2_connection_client.h"

namespace srsran {
namespace srs_cu_up {

/// O-RAN CU-UP configuration.
struct o_cu_up_config {
  /// CU-UP configuration.
  cu_up_config cu_up_cfg;
  /// E2AP configuration.
  e2ap_configuration e2ap_cfg;
};

/// O-RAN CU-UP dependencies.
struct o_cu_up_dependencies {
  /// CU-UP dependencies.
  cu_up_dependencies cu_dependencies;
  /// E2 CU metrics interface.
  e2_cu_metrics_interface* e2_cu_metric_iface = nullptr;
  /// E2 connection client.
  e2_connection_client* e2_client = nullptr;
};

} // namespace srs_cu_up
} // namespace srsran
