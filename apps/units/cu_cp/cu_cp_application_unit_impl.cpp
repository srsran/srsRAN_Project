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

#include "cu_cp_application_unit_impl.h"
#include "cu_cp_builder.h"
#include "cu_cp_config_translators.h"
#include "cu_cp_logger_registrator.h"
#include "cu_cp_unit_config_cli11_schema.h"
#include "cu_cp_unit_config_validator.h"
#include "cu_cp_unit_config_yaml_writer.h"

using namespace srsran;

cu_cp_application_unit_impl::cu_cp_application_unit_impl(std::string_view app_name)
{
  unit_cfg.pcap_cfg.ngap.filename = fmt::format("/tmp/{}_ngap.pcap", app_name);
  unit_cfg.pcap_cfg.e1ap.filename = fmt::format("/tmp/{}_e1ap.pcap", app_name);
  unit_cfg.pcap_cfg.f1ap.filename = fmt::format("/tmp/{}_f1ap.pcap", app_name);
}

void cu_cp_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_cu_cp_unit_config_schema(app, unit_cfg);
}

void cu_cp_application_unit_impl::on_configuration_parameters_autoderivation(CLI::App& app)
{
  autoderive_cu_cp_parameters_after_parsing(app, unit_cfg);
}

bool cu_cp_application_unit_impl::on_configuration_validation(const os_sched_affinity_bitmask& available_cpus) const
{
  return validate_cu_cp_unit_config(unit_cfg);
}

void cu_cp_application_unit_impl::on_loggers_registration()
{
  register_cu_cp_loggers(unit_cfg.loggers);
}

cu_cp_unit cu_cp_application_unit_impl::create_cu_cp(cu_cp_build_dependencies& dependencies)
{
  return build_cu_cp(unit_cfg, dependencies);
}

void cu_cp_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_cu_cp_config_in_yaml_schema(node, unit_cfg);
}

void cu_cp_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  fill_cu_cp_worker_manager_config(config, unit_cfg);
}

std::unique_ptr<cu_cp_application_unit> srsran::create_cu_cp_application_unit(std::string_view app_name)
{
  return std::make_unique<cu_cp_application_unit_impl>(app_name);
}
