/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dynamic_o_du_unit_config_yaml_writer.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_yaml_writer.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_yaml_writer.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_yaml_writer.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_yaml_writer.h"
#include "dynamic_o_du_unit_config.h"

using namespace srsran;

static void fill_ru_dummy_metrics_section(YAML::Node node, const ru_dummy_unit_metrics_config& config)
{
  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.metrics_cfg);

  auto metrics_node         = node["metrics"];
  metrics_node["enable_ru"] = config.enable_ru_metrics;
}

static void fill_ru_dummy_config(YAML::Node node, const ru_dummy_unit_config& config)
{
  fill_ru_dummy_metrics_section(node, config.metrics_cfg);

  auto ru_dummy_node                   = node["ru_dummy"];
  ru_dummy_node["dl_processing_delay"] = config.dl_processing_delay;
  ru_dummy_node["time_scaling"]        = config.time_scaling;

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

void srsran::fill_dynamic_o_du_unit_config_in_yaml_schema(YAML::Node& node, const dynamic_o_du_unit_config& config)
{
  fill_o_du_high_config_in_yaml_schema(node, config.odu_high_cfg);
  fill_du_low_config_in_yaml_schema(node, config.du_low_cfg);

  if (std::holds_alternative<ru_dummy_unit_config>(config.ru_cfg)) {
    fill_ru_dummy_config(node, std::get<ru_dummy_unit_config>(config.ru_cfg));
  } else if (std::holds_alternative<ru_ofh_unit_parsed_config>(config.ru_cfg)) {
    fill_ru_ofh_config_in_yaml_schema(node, std::get<ru_ofh_unit_parsed_config>(config.ru_cfg).config);
  } else if (std::holds_alternative<ru_sdr_unit_config>(config.ru_cfg)) {
    fill_ru_sdr_config_in_yaml_schema(node, std::get<ru_sdr_unit_config>(config.ru_cfg));
  }
}
