/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_cu_up_e2_config_yaml_writer.h"
#include "apps/services/e2/e2_config_yaml_writer.h"
#include "o_cu_up_e2_config.h"

using namespace srsran;

static void fill_o_cu_up_e2_pcap_section(YAML::Node node, const o_cu_up_e2_pcap_config& config)
{
  node["e2ap_cu_up_filename"] = config.filename;
  node["e2ap_enable"]         = config.enabled;
}

void srsran::fill_o_cu_up_e2_config_in_yaml_schema(YAML::Node& node, const o_cu_up_e2_config& config)
{
  YAML::Node e2_node         = node["e2"];
  e2_node["enable_cu_up_e2"] = config.config.enable_unit_e2;
  fill_e2_config_in_yaml_schema(e2_node, config.config);

  // PCAPs.
  fill_o_cu_up_e2_pcap_section(node["pcap"], config.pcaps);
}
