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

#include "metrics_config_yaml_writer.h"
#include "metrics_config.h"

using namespace srsran;
using namespace app_helpers;

void srsran::app_helpers::fill_metrics_appconfig_in_yaml_schema(YAML::Node& node, const metrics_config& config)
{
  YAML::Node log_node        = node["metrics"];
  log_node["enable_log"]     = config.enable_log_metrics;
  log_node["enable_json"]    = config.enable_json_metrics;
  log_node["enable_verbose"] = config.enable_verbose;
}
