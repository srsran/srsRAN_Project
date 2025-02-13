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
#include "apps/services/hal/hal_appconfig.h"
#include "apps/services/logger/logger_appconfig.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "apps/services/worker_manager/worker_manager_appconfig.h"
#include <optional>

namespace srsran {
namespace srs_du {

/// Configuration of the F1-C interface of the DU.
struct f1ap_appconfig {
  /// CU-CP F1-C address the DU will connect to.
  std::string cu_cp_address = "127.0.10.1";
  /// DU F1-C bind address.
  std::string bind_address = "127.0.10.2";
};

struct f1u_appconfig {
  unsigned              pdu_queue_size = 2048;
  f1u_sockets_appconfig f1u_sockets;
};

/// Metrics report configuration.
struct metrics_appconfig {
  std::string addr = "127.0.0.1";
  uint16_t    port = 55555;
  /// Resource usage report period in milliseconds.
  unsigned rusage_report_period = 0;
  bool     enable_json_metrics  = false;
};

} // namespace srs_du

/// DU application configuration.
struct du_appconfig {
  /// Default constructor to update the log filename.
  du_appconfig() { log_cfg.filename = "/tmp/du.log"; }
  /// Loggers configuration.
  logger_appconfig log_cfg;
  /// Metrics configuration.
  srs_du::metrics_appconfig metrics_cfg;
  /// F1-C configuration.
  srs_du::f1ap_appconfig f1ap_cfg;
  /// F1-U configuration.
  srs_du::f1u_appconfig f1u_cfg;
  /// Buffer pool configuration.
  buffer_pool_appconfig buffer_pool_config;
  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;
  /// HAL configuration.
  std::optional<hal_appconfig> hal_config;
  /// Remote control configuration.
  remote_control_appconfig remote_control_config;
};

} // namespace srsran
