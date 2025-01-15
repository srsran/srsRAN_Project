/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "metrics_logger_appconfig_yaml_writer.h"
#include "metrics_logger_appconfig.h"
#include "srsran/srslog/logger.h"

using namespace srsran;

void srsran::fill_metrics_logger_appconfig_in_yaml_schema(YAML::Node& node, const metrics_logger_appconfig& config)
{
  YAML::Node log_node       = node["log"];
  log_node["metrics_level"] = srslog::basic_level_to_string(config.level);
  log_node["hex_max_size"]  = config.hex_max_size;
}
