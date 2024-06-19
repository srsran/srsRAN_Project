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

#include "apps/gnb/gnb_appconfig.h"
#include <string>

namespace srsran {
namespace srs_cu {

/// Configuration of logging functionalities.
struct log_appconfig {
  /// Path to log file or "stdout" to print to console.
  std::string filename = "/tmp/cu.log";
  /// Default log level for all layers.
  std::string all_level = "warning";
  /// Generic log level assigned to library components without layer-specific level.
  std::string lib_level     = "warning";
  std::string e2ap_level    = "warning";
  std::string config_level  = "none";
  std::string metrics_level = "none";
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Set to a valid file path to enable tracing and write the trace to the file.
  std::string tracing_filename;
};

/// NR-U configuration
struct cu_nru_appconfig {
  std::string bind_addr       = "127.0.10.1"; // Bind address used by the F1-U interface
  int         udp_rx_max_msgs = 256; // Max number of UDP packets received by a single syscall on the F1-U interface.
};

/// F1AP configuration
struct cu_f1ap_appconfig {
  /// F1-C bind address
  std::string bind_address = "127.0.10.1";
};
} // namespace srs_cu

/// Monolithic gnb application configuration.
struct cu_appconfig {
  /// Logging configuration.
  srs_cu::log_appconfig log_cfg;

  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;

  /// NR-U
  srs_cu::cu_nru_appconfig nru_cfg;

  /// F1AP
  srs_cu::cu_f1ap_appconfig f1ap_cfg;

  /// Buffer pool configuration.
  buffer_pool_appconfig buffer_pool_config;

  /// TODO fill in the rest of the configuration
};

} // namespace srsran
