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

#include "dynamic_du_unit_config_yaml_writer.h"
#include "apps/units/flexible_du/du_high/du_high_config_yaml_writer.h"
#include "apps/units/flexible_du/du_low/du_low_config_yaml_writer.h"
#include "apps/units/flexible_du/fapi/fapi_config_yaml_writer.h"
#include "apps/units/flexible_du/split_7_2/ru_ofh_config_yaml_writer.h"
#include "apps/units/flexible_du/split_8/ru_sdr_config_yaml_writer.h"
#include "dynamic_du_unit_config.h"

using namespace srsran;

static void fill_ru_dummy_config(YAML::Node node, const ru_dummy_unit_config& config)
{
  auto ru_dummy_node                   = node["ru_dummy"];
  ru_dummy_node["dl_processing_delay"] = config.dl_processing_delay;

  auto expert_exec_node = node["expert_execution"];

  auto cell_affinities_node = expert_exec_node["cell_affinities"];
  while (config.cell_affinities.size() > cell_affinities_node.size()) {
    cell_affinities_node.push_back(YAML::Node());
  }

  unsigned index = 0;
  for (auto cell : cell_affinities_node) {
    const auto& expert = config.cell_affinities[index];

    if (expert.ru_cpu_cfg.mask.any()) {
      cell["ru_cpus"] = fmt::format("{:,}", span<const size_t>(expert.ru_cpu_cfg.mask.get_cpu_ids()));
    }
    cell["ru_pinning"] = to_string(expert.ru_cpu_cfg.pinning_policy);

    ++index;
  }
}

void srsran::fill_dynamic_du_unit_config_in_yaml_schema(YAML::Node& node, const dynamic_du_unit_config& config)
{
  fill_du_high_config_in_yaml_schema(node, config.du_high_cfg.config);
  fill_du_low_config_in_yaml_schema(node, config.du_low_cfg);
  fill_fapi_config_in_yaml_schema(node, config.fapi_cfg);

  if (std::holds_alternative<ru_dummy_unit_config>(config.ru_cfg)) {
    fill_ru_dummy_config(node, std::get<ru_dummy_unit_config>(config.ru_cfg));
  } else if (std::holds_alternative<ru_ofh_unit_parsed_config>(config.ru_cfg)) {
    fill_ru_ofh_config_in_yaml_schema(node, std::get<ru_ofh_unit_parsed_config>(config.ru_cfg).config);
  } else if (std::holds_alternative<ru_sdr_unit_config>(config.ru_cfg)) {
    fill_ru_sdr_config_in_yaml_schema(node, std::get<ru_sdr_unit_config>(config.ru_cfg));
  }
}
