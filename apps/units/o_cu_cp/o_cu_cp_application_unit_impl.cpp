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

#include "o_cu_cp_application_unit_impl.h"
#include "cu_cp/cu_cp_config_translators.h"
#include "cu_cp/cu_cp_logger_registrator.h"
#include "cu_cp/cu_cp_unit_config_cli11_schema.h"
#include "cu_cp/cu_cp_unit_config_validator.h"
#include "cu_cp/cu_cp_unit_config_yaml_writer.h"
#include "e2/o_cu_cp_e2_config_cli11_schema.h"
#include "e2/o_cu_cp_e2_config_translators.h"
#include "e2/o_cu_cp_e2_config_yaml_writer.h"

using namespace srsran;

o_cu_cp_application_unit_impl::o_cu_cp_application_unit_impl(std::string_view app_name)
{
  unit_cfg.cucp_cfg.pcap_cfg.ngap.filename = fmt::format("/tmp/{}_ngap.pcap", app_name);
  unit_cfg.cucp_cfg.pcap_cfg.e1ap.filename = fmt::format("/tmp/{}_e1ap.pcap", app_name);
  unit_cfg.cucp_cfg.pcap_cfg.f1ap.filename = fmt::format("/tmp/{}_f1ap.pcap", app_name);
  // Note: do not update the default e2ap pcap filename.
}

void o_cu_cp_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_cu_cp_unit_config_schema(app, unit_cfg.cucp_cfg);
  configure_cli11_with_o_cu_cp_e2_config_schema(app, unit_cfg.e2_cfg);
}

void o_cu_cp_application_unit_impl::on_configuration_parameters_autoderivation(CLI::App& app)
{
  autoderive_cu_cp_parameters_after_parsing(app, unit_cfg.cucp_cfg);
  autoderive_o_cu_cp_e2_parameters_after_parsing(unit_cfg.e2_cfg);
}

bool o_cu_cp_application_unit_impl::on_configuration_validation() const
{
  return validate_cu_cp_unit_config(unit_cfg.cucp_cfg);
}

void o_cu_cp_application_unit_impl::on_loggers_registration()
{
  register_cu_cp_loggers(unit_cfg.cucp_cfg.loggers);
}

bool o_cu_cp_application_unit_impl::are_metrics_enabled() const
{
  return unit_cfg.cucp_cfg.metrics.layers_cfg.are_metrics_enabled();
}

o_cu_cp_unit o_cu_cp_application_unit_impl::create_o_cu_cp(o_cu_cp_unit_dependencies& dependencies)
{
  return build_o_cu_cp(unit_cfg, dependencies);
}

void o_cu_cp_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_cu_cp_config_in_yaml_schema(node, unit_cfg.cucp_cfg);
  fill_o_cu_cp_e2_config_in_yaml_schema(node, unit_cfg.e2_cfg);
}

void o_cu_cp_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  fill_cu_cp_worker_manager_config(config, unit_cfg.cucp_cfg);
  fill_o_cu_cp_e2_worker_manager_config(config, unit_cfg.e2_cfg);
}

std::unique_ptr<o_cu_cp_application_unit> srsran::create_o_cu_cp_application_unit(std::string_view app_name)
{
  return std::make_unique<o_cu_cp_application_unit_impl>(app_name);
}
