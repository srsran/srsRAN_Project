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

#include "srsran/cu_cp/cu_configurator.h"
#include "srsran/cu_cp/cu_cp_configuration.h"

namespace srsran {
namespace srs_cu_cp {

/// O-RAN CU-CP configuration.
struct o_cu_cp_config {
  /// CU-CP configuration.
  cu_cp_configuration cu_cp_config;
  /// E2AP configuration.
  e2ap_configuration e2ap_config;
};

/// O-RAN CU-CP dependencies.
struct o_cu_cp_dependencies {
  /// E2 connection client.
  e2_connection_client* e2_client = nullptr;
  /// E2 CU metrics interface.
  e2_cu_metrics_interface* e2_cu_metric_iface = nullptr;
  // E2 CU configurator.
  cu_configurator* cu_cfg = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
