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
#include "metrics_appconfig.h"

using namespace srsran;
using namespace app_services;

void srsran::app_services::fill_metrics_appconfig_in_yaml_schema(YAML::Node& node, const metrics_appconfig& config)
{
  YAML::Node metrics_node                     = node["metrics"];
  YAML::Node periodicity_node                 = node["periodicity"];
  periodicity_node["app_usage_report_period"] = config.app_usage_report_period;
}
