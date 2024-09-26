/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split_7_2_du_application_unit_impl.h"
#include "apps/units/flexible_du/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/du_low/du_low_config_translator.h"
#include "apps/units/flexible_du/fapi/fapi_config_translator.h"
#include "apps/units/flexible_du/split_7_2/helpers/ru_ofh_config_translator.h"
#include "split_7_2_du_factory.h"
#include "split_7_2_du_unit_cli11_schema.h"
#include "split_7_2_du_unit_config_validator.h"
#include "split_7_2_du_unit_config_yaml_writer.h"
#include "split_7_2_du_unit_logger_registrator.h"

using namespace srsran;

void split_7_2_du_application_unit_impl::on_loggers_registration()
{
  register_split_7_2_du_loggers(unit_cfg);
}

void split_7_2_du_application_unit_impl::on_configuration_parameters_autoderivation(CLI::App& app)
{
  autoderive_split_7_2_du_parameters_after_parsing(app, unit_cfg);
}

bool split_7_2_du_application_unit_impl::on_configuration_validation(
    const os_sched_affinity_bitmask& available_cpus) const
{
  return validate_split_7_2_du_unit_config(unit_cfg, available_cpus);
}

void split_7_2_du_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_split_7_2_du_unit_config_schema(app, unit_cfg);
}

du_unit split_7_2_du_application_unit_impl::create_flexible_du_unit(const du_unit_dependencies& dependencies)
{
  return create_split_7_2_du(unit_cfg, dependencies);
}

std::unique_ptr<flexible_du_application_unit> srsran::create_flexible_du_application_unit()
{
  return std::make_unique<split_7_2_du_application_unit_impl>();
}

void split_7_2_du_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_split_7_2_du_unit_config_in_yaml_schema(node, unit_cfg);
}

void split_7_2_du_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  // OFH always runs in non blocking mode.
  bool     is_blocking_mode_enable = false;
  unsigned nof_cells               = unit_cfg.du_high_cfg.config.cells_cfg.size();
  fill_du_high_worker_manager_config(config, unit_cfg.du_high_cfg.config, is_blocking_mode_enable);
  fill_du_low_worker_manager_config(config, unit_cfg.du_low_cfg, is_blocking_mode_enable, nof_cells);
  fill_fapi_worker_manager_config(config, unit_cfg.fapi_cfg, nof_cells);
  auto cells = generate_du_cell_config(unit_cfg.du_high_cfg.config);
  fill_ofh_worker_manager_config(config, unit_cfg.ru_cfg.config, cells);
}