/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_high/du_high_config_yaml_writer.h"
#include "e2/o_du_high_e2_config_yaml_writer.h"
#include "fapi/fapi_config_yaml_writer.h"
#include "o_du_high_unit_config.h"
#include <yaml-cpp/yaml.h>

namespace srsran {

/// Fills the given node with the O-DU high configuration values.
inline void fill_o_du_high_config_in_yaml_schema(YAML::Node& node, const o_du_high_unit_config& config)
{
  fill_du_high_config_in_yaml_schema(node, config.du_high_cfg.config);
  fill_fapi_config_in_yaml_schema(node, config.fapi_cfg);
  fill_o_du_high_e2_config_in_yaml_schema(node, config.e2_cfg);
}

} // namespace srsran
