/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

std::unique_ptr<cu_cp_application_unit> srsran::create_cu_cp_application_unit()
{
  return std::make_unique<cu_cp_application_unit_impl>();
}
