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

#include "o_cu_up_e2_config_yaml_writer.h"
#include "apps/helpers/e2/e2_config_yaml_writer.h"
#include "o_cu_up_e2_config.h"

using namespace srsran;

static void fill_o_cu_up_e2_pcap_section(YAML::Node node, const o_cu_up_e2_pcap_config& config)
{
  node["e2ap_cu_up_filename"] = config.filename;
  node["e2ap_cu_up_enable"]   = config.enabled;
}

void srsran::fill_o_cu_up_e2_config_in_yaml_schema(YAML::Node& node, const o_cu_up_e2_config& config)
{
  YAML::Node e2_node         = node["e2"];
  e2_node["enable_cu_up_e2"] = config.base_config.enable_unit_e2;
  fill_e2_config_in_yaml_schema(e2_node, config.base_config);

  // PCAPs.
  fill_o_cu_up_e2_pcap_section(node["pcap"], config.pcaps);
}
