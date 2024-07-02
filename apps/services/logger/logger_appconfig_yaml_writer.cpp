/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "logger_appconfig_yaml_writer.h"
#include "logger_appconfig.h"

using namespace srsran;

void srsran::fill_logger_appconfig_in_yaml_schema(YAML::Node& node, const logger_appconfig& config)
{
  YAML::Node log_node       = node["log"];
  log_node["filename"]      = config.filename;
  log_node["lib_level"]     = srslog::basic_level_to_string(config.lib_level);
  log_node["e2ap_level"]    = srslog::basic_level_to_string(config.e2ap_level);
  log_node["config_level"]  = srslog::basic_level_to_string(config.config_level);
  log_node["metrics_level"] = srslog::basic_level_to_string(config.metrics_level);
  log_node["hex_max_size"]  = config.hex_max_size;

  if (!config.tracing_filename.empty()) {
    log_node["tracing_filename"] = config.tracing_filename;
  }
}
