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

#include "apps/services/buffer_pool/buffer_pool_appconfig.h"
#include "apps/services/f1u/f1u_cli11_schema.h"
#include "apps/services/logger/logger_appconfig.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "apps/services/worker_manager/worker_manager_appconfig.h"
#include <string>

namespace srsran {
namespace srs_cu {

/// F1AP configuration.
struct cu_f1ap_appconfig {
  /// F1-C bind address.
  std::string bind_addr = "127.0.10.1";
};

/// Metrics report configuration.
struct metrics_appconfig {
  std::string addr = "127.0.0.1";
  uint16_t    port = 55555;
  /// Resource usage report period in milliseconds.
  unsigned rusage_report_period = 0;
  bool     enable_json_metrics  = false;
};

} // namespace srs_cu

/// CU application configuration.
struct cu_appconfig {
  /// Default constructor to update the log filename.
  cu_appconfig() { log_cfg.filename = "/tmp/cu.log"; }
  /// Loggers configuration.
  logger_appconfig log_cfg;
  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;
  /// F1-U configuration.
  f1u_sockets_appconfig f1u_cfg;
  /// F1AP configuration.
  srs_cu::cu_f1ap_appconfig f1ap_cfg;
  /// Buffer pool configuration.
  buffer_pool_appconfig buffer_pool_config;
  /// Remote control configuration.
  remote_control_appconfig remote_control_config;
  /// Metrics configuration.
  srs_cu::metrics_appconfig metrics_cfg;
};

} // namespace srsran
