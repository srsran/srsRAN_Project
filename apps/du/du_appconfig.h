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

/// Configuration of logging functionalities.
struct log_appconfig {
  /// Path to log file or "stdout" to print to console.
  std::string filename = "/tmp/du.log";
  /// Default log level for all layers.
  std::string all_level = "warning";
  /// Generic log level assigned to library components without layer-specific level.
  std::string lib_level     = "warning";
  std::string e2ap_level    = "warning";
  std::string config_level  = "none";
  std::string metrics_level = "none";
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Set to true to log broadcasting messages and all PRACH opportunities.
  bool broadcast_enabled = false;
  /// Set to a valid file path to enable tracing and write the trace to the file.
  std::string tracing_filename;
};

/// Configuration of the F1-C interface of the DU.
struct f1ap_appconfig {
  /// CU-CP F1-C address the DU will connect to.
  std::string cu_cp_address = "127.0.10.1";
  /// DU F1-C bind address.
  std::string bind_address = "127.0.10.2";
};

struct nru_appconfig {
  unsigned pdu_queue_size = 2048;
  /// IP address to bind the F1-U interface to.
  std::string bind_address = "127.0.10.2";
};

/// Metrics report configuration.
struct metrics_appconfig {
  /// JSON metrics reporting.
  bool        enable_json_metrics      = false;
  std::string addr                     = "127.0.0.1";
  uint16_t    port                     = 55555;
  bool        autostart_stdout_metrics = false;
  unsigned    stdout_metrics_period    = 1000; // Statistics report period in milliseconds
};

} // namespace srs_du

/// DU application configuration.
struct du_appconfig {
  /// Logging configuration.
  srs_du::log_appconfig log_cfg;
  /// Metrics configuration.
  srs_du::metrics_appconfig metrics_cfg;
  /// E2 configuration.
  e2_appconfig e2_cfg;
  /// F1-C configuration.
  srs_du::f1ap_appconfig f1ap_cfg;
  /// F1-U configuration.
  srs_du::nru_appconfig nru_cfg;
  /// Buffer pool configuration.
  buffer_pool_appconfig buffer_pool_config;
  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;
  /// HAL configuration.
  std::optional<hal_appconfig> hal_config;
};

} // namespace srsran
