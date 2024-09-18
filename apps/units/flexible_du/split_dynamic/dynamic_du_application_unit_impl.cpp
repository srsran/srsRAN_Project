/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

void dynamic_du_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_dynamic_du_unit_config_schema(app, unit_cfg);
}

du_unit dynamic_du_application_unit_impl::create_flexible_du_unit(const du_unit_dependencies& dependencies)
{
  return create_dynamic_du(unit_cfg, dependencies);
}

std::unique_ptr<flexible_du_application_unit> srsran::create_flexible_du_application_unit()
{
  return std::make_unique<dynamic_du_application_unit_impl>();
}

void dynamic_du_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_dynamic_du_unit_config_in_yaml_schema(node, unit_cfg);
}

void dynamic_du_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  fill_dynamic_du_worker_manager_config(config, unit_cfg);
}
