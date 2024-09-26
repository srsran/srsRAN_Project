/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_application_unit_impl.h"
#include "cu_up_builder.h"
#include "cu_up_logger_registrator.h"
#include "cu_up_unit_config_cli11_schema.h"
#include "cu_up_unit_config_translators.h"
#include "cu_up_unit_config_validator.h"
#include "cu_up_unit_config_yaml_writer.h"

using namespace srsran;

void cu_up_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_cu_up_unit_config_schema(app, unit_cfg);
}

bool cu_up_application_unit_impl::on_configuration_validation(const os_sched_affinity_bitmask& available_cpus) const
{
  return validate_cu_up_unit_config(unit_cfg);
}

void cu_up_application_unit_impl::on_loggers_registration()
{
  register_cu_up_loggers(unit_cfg.loggers);
}
std::unique_ptr<srs_cu_up::cu_up_interface>

cu_up_application_unit_impl::create_cu_up_unit(const cu_up_unit_dependencies& dependencies)
{
  return build_cu_up(unit_cfg, dependencies);
}

void cu_up_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_cu_up_config_in_yaml_schema(node, unit_cfg);
}

void cu_up_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  fill_cu_up_worker_manager_config(config, unit_cfg);
}

std::unique_ptr<cu_up_application_unit> srsran::create_cu_up_application_unit()
{
  return std::make_unique<cu_up_application_unit_impl>();
}
