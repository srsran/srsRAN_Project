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

#include "split6_o_du_low_application_unit_impl.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_translator.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_yaml_writer.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_translator.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_yaml_writer.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_translator.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_yaml_writer.h"
#include "split6_o_du_low_unit_cli11_schema.h"
#include "split6_o_du_low_unit_config_validator.h"
#include "split6_o_du_low_unit_logger_registrator.h"

using namespace srsran;

split6_o_du_low_application_unit_impl::split6_o_du_low_application_unit_impl(
    std::string_view                        app_name,
    std::unique_ptr<split6_o_du_low_plugin> plugin_) :
  plugin(std::move(plugin_))
{
  srsran_assert(plugin, "Invalid split 6 O-DU low plugin");
}

void split6_o_du_low_application_unit_impl::on_loggers_registration()
{
  register_split6_o_du_low_loggers(unit_cfg);
  plugin->on_loggers_registration();
}

bool split6_o_du_low_application_unit_impl::are_metrics_enabled() const
{
  return unit_cfg.du_low_cfg.metrics_cfg.common_metrics_cfg.enabled();
}

void split6_o_du_low_application_unit_impl::on_configuration_parameters_autoderivation(CLI::App& app)
{
  autoderive_split6_o_du_low_parameters_after_parsing(app, unit_cfg);
}

bool split6_o_du_low_application_unit_impl::on_configuration_validation(
    const os_sched_affinity_bitmask& available_cpus) const
{
  if (!plugin->on_configuration_validation()) {
    return false;
  }

  return validate_split6_o_du_low_unit_config(unit_cfg, available_cpus);
}

void split6_o_du_low_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_split6_o_du_low_unit_config_schema(app, unit_cfg);
  plugin->on_parsing_configuration_registration(app);
}

void split6_o_du_low_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_du_low_config_in_yaml_schema(node, unit_cfg.du_low_cfg);

  if (auto* ru = std::get_if<ru_ofh_unit_parsed_config>(&unit_cfg.ru_cfg)) {
    fill_ru_ofh_config_in_yaml_schema(node, ru->config);
  } else if (auto* ru = std::get_if<ru_sdr_unit_config>(&unit_cfg.ru_cfg)) {
    fill_ru_sdr_config_in_yaml_schema(node, *ru);
  }
}

void split6_o_du_low_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  // Split 6 always runs in non blocking mode.
  bool is_blocking_mode_enable = false;
  plugin->fill_worker_manager_config(config);

  const unsigned nof_cells = 1U;
  fill_du_low_worker_manager_config(config, unit_cfg.du_low_cfg, is_blocking_mode_enable, nof_cells);

  if (auto* ru = std::get_if<ru_sdr_unit_config>(&unit_cfg.ru_cfg)) {
    fill_sdr_worker_manager_config(config, *ru);
  } else if (auto* ru = std::get_if<ru_ofh_unit_parsed_config>(&unit_cfg.ru_cfg)) {
    fill_ofh_worker_manager_config(config, ru->config, cells);
  }
}
