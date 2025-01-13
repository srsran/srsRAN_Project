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
