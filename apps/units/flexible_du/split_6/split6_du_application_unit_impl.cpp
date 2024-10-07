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

#include "split6_du_application_unit_impl.h"
#include "apps/units/flexible_du/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/du_high/du_high_config_yaml_writer.h"
#include "apps/units/flexible_du/fapi/fapi_config_translator.h"
#include "apps/units/flexible_du/fapi/fapi_config_yaml_writer.h"
#include "split6_du_factory.h"
#include "split6_du_unit_cli11_schema.h"
#include "split6_du_unit_config_validator.h"
#include "split6_du_unit_logger_registrator.h"

using namespace srsran;

split6_du_application_unit_impl::split6_du_application_unit_impl(std::string_view app_name)
{
  unit_cfg.du_high_cfg.config.pcaps.e2ap.filename = fmt::format("/tmp/{}_e2ap.pcap", app_name);
  unit_cfg.du_high_cfg.config.pcaps.f1ap.filename = fmt::format("/tmp/{}_f1ap.pcap", app_name);
  unit_cfg.du_high_cfg.config.pcaps.f1u.filename  = fmt::format("/tmp/{}_f1u.pcap", app_name);
  unit_cfg.du_high_cfg.config.pcaps.rlc.filename  = fmt::format("/tmp/{}_rlc.pcap", app_name);
  unit_cfg.du_high_cfg.config.pcaps.mac.filename  = fmt::format("/tmp/{}_mac.pcap", app_name);
}

void split6_du_application_unit_impl::on_loggers_registration()
{
  register_split6_du_loggers(unit_cfg);
  plugin->on_loggers_registration();
}

void split6_du_application_unit_impl::on_configuration_parameters_autoderivation(CLI::App& app)
{
  autoderive_split6_du_parameters_after_parsing(app, unit_cfg);
}

bool split6_du_application_unit_impl::on_configuration_validation(const os_sched_affinity_bitmask& available_cpus) const
{
  if (!plugin->on_configuration_validation()) {
    return false;
  }

  return validate_split6_du_unit_config(unit_cfg, available_cpus);
}

void split6_du_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_split6_du_unit_config_schema(app, unit_cfg);
  plugin->on_parsing_configuration_registration(app);
}

du_unit split6_du_application_unit_impl::create_flexible_du_unit(const du_unit_dependencies& dependencies)
{
  auto fapi_ctrl = plugin->create_fapi_adaptor(dependencies);
  report_error_if_not(!fapi_ctrl.empty(), "Could not create FAPI adaptor");
  auto du_impl = create_du_split6(unit_cfg, dependencies, std::move(fapi_ctrl));
  report_error_if_not(du_impl.unit, "Could not create split 6 DU");

  return du_impl;
}

void split6_du_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_du_high_config_in_yaml_schema(node, unit_cfg.du_high_cfg.config);
  fill_fapi_config_in_yaml_schema(node, unit_cfg.fapi_cfg);
}

void split6_du_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  // Split 6 always runs in non blocking mode.
  bool     is_blocking_mode_enable = false;
  unsigned nof_cells               = unit_cfg.du_high_cfg.config.cells_cfg.size();
  fill_du_high_worker_manager_config(config, unit_cfg.du_high_cfg.config, is_blocking_mode_enable);
  fill_fapi_worker_manager_config(config, unit_cfg.fapi_cfg, nof_cells);
}
