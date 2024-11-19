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
