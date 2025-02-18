/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "metrics_config_yaml_writer.h"
#include "metrics_config.h"

using namespace srsran;
using namespace app_helpers;

void srsran::app_helpers::fill_metrics_appconfig_in_yaml_schema(YAML::Node& node, const metrics_config& config)
{
  YAML::Node log_node            = node["metrics"];
  log_node["enable_log_metrics"] = config.enable_log_metrics;

  // JSON configuration.
  log_node["enable_json_metrics"] = config.json_config.enable_json_metrics;
  log_node["addr"]                = config.json_config.addr;
  log_node["port"]                = config.json_config.port;
}
