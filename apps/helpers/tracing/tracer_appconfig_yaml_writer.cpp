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

#include "tracer_appconfig_yaml_writer.h"
#include "tracer_appconfig.h"

using namespace srsran;

void srsran::fill_tracer_appconfig_in_yaml_schema(YAML::Node& node, const tracer_appconfig& config)
{
  YAML::Node log_node                         = node["trace"];
  log_node["filename"]                        = config.filename;
  log_node["max_tracing_events_per_file"]     = config.max_tracing_events_per_file;
  log_node["nof_tracing_events_after_severe"] = config.nof_tracing_events_after_severe;
}
