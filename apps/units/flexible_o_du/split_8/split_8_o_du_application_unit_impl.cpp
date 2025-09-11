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

#include "split_8_o_du_application_unit_impl.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_translators.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_translator.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_translator.h"
#include "split_8_o_du_factory.h"
#include "split_8_o_du_unit_cli11_schema.h"
#include "split_8_o_du_unit_config_validator.h"
#include "split_8_o_du_unit_config_yaml_writer.h"
#include "split_8_o_du_unit_logger_registrator.h"

using namespace srsran;

void split_8_o_du_application_unit_impl::on_loggers_registration()
{
  register_split_8_o_du_loggers(unit_cfg);
}

bool split_8_o_du_application_unit_impl::are_metrics_enabled() const
{
  return unit_cfg.odu_high_cfg.du_high_cfg.config.metrics.layers_cfg.are_metrics_enabled() ||
         unit_cfg.du_low_cfg.metrics_cfg.enable_du_low || unit_cfg.ru_cfg.metrics_cfg.enable_ru_metrics;
}

void split_8_o_du_application_unit_impl::on_configuration_parameters_autoderivation(CLI::App& app)
{
  autoderive_split_8_o_du_parameters_after_parsing(app, unit_cfg);
}

bool split_8_o_du_application_unit_impl::on_configuration_validation() const
{
  return validate_split_8_o_du_unit_config(unit_cfg);
}

split_8_o_du_application_unit_impl::split_8_o_du_application_unit_impl(std::string_view app_name)
{
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.f1ap.filename = fmt::format("/tmp/{}_f1ap.pcap", app_name);
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.f1u.filename  = fmt::format("/tmp/{}_f1u.pcap", app_name);
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.rlc.filename  = fmt::format("/tmp/{}_rlc.pcap", app_name);
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.mac.filename  = fmt::format("/tmp/{}_mac.pcap", app_name);
  // Note: do not update the default e2ap pcap filename.
}

void split_8_o_du_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_split_8_o_du_unit_config_schema(app, unit_cfg);
}

o_du_unit split_8_o_du_application_unit_impl::create_flexible_o_du_unit(const o_du_unit_dependencies& dependencies)
{
  return split8_du_factory(unit_cfg).create_flexible_o_du(dependencies);
}

std::unique_ptr<flexible_o_du_application_unit> srsran::create_flexible_o_du_application_unit(std::string_view app_name)
{
  return std::make_unique<split_8_o_du_application_unit_impl>(app_name);
}

void split_8_o_du_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_split_8_o_du_unit_config_in_yaml_schema(node, unit_cfg);
}

void split_8_o_du_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  bool     is_blocking_mode_enable = unit_cfg.ru_cfg.device_driver == "zmq";
  unsigned nof_cells               = unit_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.size();
  fill_du_high_worker_manager_config(config, unit_cfg.odu_high_cfg.du_high_cfg.config, is_blocking_mode_enable);
  std::vector<unsigned> nof_dl_antennas;
  std::vector<unsigned> nof_ul_antennas;
  for (const auto& cell : unit_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg) {
    nof_dl_antennas.push_back(cell.cell.nof_antennas_dl);
    nof_ul_antennas.push_back(cell.cell.nof_antennas_ul);
  }
  fill_du_low_worker_manager_config(
      config, unit_cfg.du_low_cfg, is_blocking_mode_enable, nof_dl_antennas, nof_ul_antennas);
  fill_fapi_worker_manager_config(config, unit_cfg.odu_high_cfg.fapi_cfg, nof_cells);
  fill_sdr_worker_manager_config(config, unit_cfg.ru_cfg);
}
