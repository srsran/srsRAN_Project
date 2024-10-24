/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split_8_du_unit_config_yaml_writer.h"
#include "apps/units/flexible_du/o_du_high/o_du_high_unit_config_yaml_writer.h"
#include "apps/units/flexible_du/o_du_low/du_low_config_yaml_writer.h"
#include "apps/units/flexible_du/split_8/helpers/ru_sdr_config_yaml_writer.h"
#include "split_8_du_unit_config.h"

using namespace srsran;

void srsran::fill_split_8_du_unit_config_in_yaml_schema(YAML::Node& node, const split_8_du_unit_config& config)
{
  fill_o_du_high_config_in_yaml_schema(node, config.odu_high_cfg);
  fill_du_low_config_in_yaml_schema(node, config.du_low_cfg);
  fill_ru_sdr_config_in_yaml_schema(node, config.ru_cfg);
}
