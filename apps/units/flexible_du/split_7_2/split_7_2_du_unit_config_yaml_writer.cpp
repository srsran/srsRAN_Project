/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split_7_2_du_unit_config_yaml_writer.h"
#include "apps/units/flexible_du/du_high/du_high_config_yaml_writer.h"
#include "apps/units/flexible_du/du_low/du_low_config_yaml_writer.h"
#include "apps/units/flexible_du/fapi/fapi_config_yaml_writer.h"
#include "apps/units/flexible_du/split_7_2/helpers/ru_ofh_config_yaml_writer.h"
#include "split_7_2_du_unit_config.h"

using namespace srsran;

void srsran::fill_split_7_2_du_unit_config_in_yaml_schema(YAML::Node& node, const split_7_2_du_unit_config& config)
{
  fill_du_high_config_in_yaml_schema(node, config.du_high_cfg.config);
  fill_du_low_config_in_yaml_schema(node, config.du_low_cfg);
  fill_fapi_config_in_yaml_schema(node, config.fapi_cfg);
  fill_ru_ofh_config_in_yaml_schema(node, config.ru_cfg.config);
}
