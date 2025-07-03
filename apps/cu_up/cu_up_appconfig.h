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

#include "apps/helpers/f1u/f1u_appconfig.h"
#include "apps/helpers/logger/logger_appconfig.h"
#include "apps/services/app_resource_usage/app_resource_usage_config.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig.h"
#include "apps/services/metrics/metrics_appconfig.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "apps/services/worker_manager/worker_manager_appconfig.h"
#include <string>

namespace srsran {
namespace srs_cu {

/// Metrics report configuration.
struct metrics_appconfig {
  app_services::app_resource_usage_config rusage_config;
  app_services::metrics_appconfig         metrics_service_cfg;
};

} // namespace srs_cu

/// CU application configuration.
struct cu_up_appconfig {
  /// Default constructor to update the log filename.
  cu_up_appconfig() { log_cfg.filename = "/tmp/cu_up.log"; }
  /// Loggers configuration.
  logger_appconfig log_cfg;
  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;
  /// F1-U configuration.
  f1u_sockets_appconfig f1u_cfg;
  /// Buffer pool configuration.
  buffer_pool_appconfig buffer_pool_config;
  /// Remote control configuration.
  remote_control_appconfig remote_control_config;
  /// Metrics configuration.
  srs_cu::metrics_appconfig metrics_cfg;
  /// Dryrun mode enabled flag.
  bool enable_dryrun = false;
};

} // namespace srsran

