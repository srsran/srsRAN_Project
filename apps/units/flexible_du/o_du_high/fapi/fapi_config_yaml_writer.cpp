/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "fapi_config_yaml_writer.h"
#include "fapi_config.h"

using namespace srsran;

static void fill_du_low_log_section(YAML::Node node, srslog::basic_levels level)
{
  node["fapi_level"] = srslog::basic_level_to_string(level);
}

void srsran::fill_fapi_config_in_yaml_schema(YAML::Node& node, const fapi_unit_config& config)
{
  fill_du_low_log_section(node["log"], config.fapi_level);
  auto fapi_node                  = node["fapi"];
  fapi_node["l2_nof_slots_ahead"] = config.l2_nof_slots_ahead;
}
