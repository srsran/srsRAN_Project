/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
