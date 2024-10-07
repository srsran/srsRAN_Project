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

#include "dynamic_du_application_unit_impl.h"
#include "dynamic_du_factory.h"
#include "dynamic_du_translators.h"
#include "dynamic_du_unit_cli11_schema.h"
#include "dynamic_du_unit_config_validator.h"
#include "dynamic_du_unit_config_yaml_writer.h"
#include "dynamic_du_unit_logger_registrator.h"

using namespace srsran;

void dynamic_du_application_unit_impl::on_loggers_registration()
{
  register_dynamic_du_loggers(unit_cfg);
}

void dynamic_du_application_unit_impl::on_configuration_parameters_autoderivation(CLI::App& app)
{
  autoderive_dynamic_du_parameters_after_parsing(app, unit_cfg);
}

bool dynamic_du_application_unit_impl::on_configuration_validation(
    const os_sched_affinity_bitmask& available_cpus) const
{
  return validate_dynamic_du_unit_config(unit_cfg, available_cpus);
}

dynamic_du_application_unit_impl::dynamic_du_application_unit_impl(std::string_view app_name)
{
  unit_cfg.du_high_cfg.config.pcaps.e2ap.filename = fmt::format("/tmp/{}_e2ap.pcap", app_name);
  unit_cfg.du_high_cfg.config.pcaps.f1ap.filename = fmt::format("/tmp/{}_f1ap.pcap", app_name);
  unit_cfg.du_high_cfg.config.pcaps.f1u.filename  = fmt::format("/tmp/{}_f1u.pcap", app_name);
  unit_cfg.du_high_cfg.config.pcaps.rlc.filename  = fmt::format("/tmp/{}_rlc.pcap", app_name);
  unit_cfg.du_high_cfg.config.pcaps.mac.filename  = fmt::format("/tmp/{}_mac.pcap", app_name);
}

void dynamic_du_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_dynamic_du_unit_config_schema(app, unit_cfg);
}

du_unit dynamic_du_application_unit_impl::create_flexible_du_unit(const du_unit_dependencies& dependencies)
{
  return create_dynamic_du(unit_cfg, dependencies);
}

std::unique_ptr<flexible_du_application_unit> srsran::create_flexible_du_application_unit(std::string_view app_name)
{
  return std::make_unique<dynamic_du_application_unit_impl>(app_name);
}

void dynamic_du_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_dynamic_du_unit_config_in_yaml_schema(node, unit_cfg);
}

void dynamic_du_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  fill_dynamic_du_worker_manager_config(config, unit_cfg);
}
