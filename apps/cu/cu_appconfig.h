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

#include "apps/services/buffer_pool/buffer_pool_appconfig.h"
#include "apps/services/logger/logger_appconfig.h"
#include "apps/services/network/udp_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_appconfig.h"
#include <string>

namespace srsran {
namespace srs_cu {

/// F1-U sockets configuration
struct cu_f1u_socket_appconfig {
  std::string   bind_addr = "127.0.10.1"; // Bind address used by the F1-U interface
  udp_appconfig udp_config;
};

/// F1-U configuration
struct cu_f1u_appconfig {
  std::vector<cu_f1u_socket_appconfig> f1u_socket_cfg;
};

/// F1AP configuration
struct cu_f1ap_appconfig {
  /// F1-C bind address
  std::string bind_addr = "127.0.10.1";
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
  /// F1-U
  srs_cu::cu_f1u_appconfig f1u_cfg;
  /// F1AP
  srs_cu::cu_f1ap_appconfig f1ap_cfg;
  /// Buffer pool configuration.
  buffer_pool_appconfig buffer_pool_config;
};

} // namespace srsran
