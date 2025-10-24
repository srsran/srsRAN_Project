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
#include "metrics_appconfig.h"

using namespace srsran;
using namespace app_services;

void srsran::app_services::fill_metrics_appconfig_in_yaml_schema(YAML::Node& node, const metrics_appconfig& config)
{
  YAML::Node metrics_node                     = node["metrics"];
  YAML::Node periodicity_node                 = metrics_node["periodicity"];
  periodicity_node["app_usage_report_period"] = config.app_usage_report_period;
}
