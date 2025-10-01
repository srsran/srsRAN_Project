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
#include "apps/helpers/hal/hal_appconfig.h"
#include "apps/helpers/logger/logger_appconfig.h"
#include "apps/helpers/tracing/tracer_appconfig.h"
#include "apps/services/app_execution_metrics/executor_metrics_config.h"
#include "apps/services/app_resource_usage/app_resource_usage_config.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig.h"
#include "apps/services/metrics/metrics_appconfig.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "apps/services/worker_manager/worker_manager_appconfig.h"
#include <optional>

namespace srsran {

namespace srs_du_low {
/// Metrics report configuration.
struct metrics_appconfig {
  app_services::app_resource_usage_config rusage_config;
  app_services::metrics_appconfig         metrics_service_cfg;
  app_services::executor_metrics_config   executors_metrics_cfg;
};

} // namespace srs_du_low

/// DU low application configuration.
struct du_low_appconfig {
  /// Default constructor to update the log filename.
  du_low_appconfig() { log_cfg.filename = "/tmp/du_low.log"; }
  /// Loggers configuration.
  logger_appconfig log_cfg;
  /// Tracers configuration.
  tracer_appconfig trace_cfg;
  /// Metrics configuration.
  srs_du_low::metrics_appconfig metrics_cfg;
  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;
  /// Remote control configuration.
  remote_control_appconfig remote_control_config;
  /// HAL configuration.
  std::optional<hal_appconfig> hal_config;
  /// Dryrun mode enabled flag.
  bool enable_dryrun = false;
};

} // namespace srsran
