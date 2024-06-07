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

#include "../gnb/gnb_appconfig.h" // TODO: Remove
#include "apps/services/os_sched_affinity_manager.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/support/executors/unique_thread.h"
#include <optional>

namespace srsran {
namespace srs_du {

/// Configuration of the F1-C interface of the DU.
struct f1c_appconfig {
  /// CU-CP F1-C address the DU will connect to.
  std::string cu_cp_address = "127.0.10.1";
  /// DU F1-C bind address.
  std::string bind_address = "127.0.10.2";
};

struct f1u_appconfig {
  unsigned pdu_queue_size = 2048;
  /// IP address to bind the F1-U interface to.
  std::string bind_address = "127.0.10.2";
};

} // namespace srs_du

/// DU application configuration.
struct du_appconfig {
  /// Logging configuration.
  log_appconfig log_cfg;
  /// Metrics configuration.
  metrics_appconfig metrics_cfg;
  /// E2 configuration.
  e2_appconfig e2_cfg;
  /// F1-C configuration.
  srs_du::f1c_appconfig f1c_cfg;
  /// F1-U configuration.
  srs_du::f1u_appconfig f1u_cfg;
  /// Buffer pool configuration.
  buffer_pool_appconfig buffer_pool_config;
  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;
  /// HAL configuration.
  std::optional<hal_appconfig> hal_config;
};

} // namespace srsran
