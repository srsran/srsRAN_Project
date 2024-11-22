/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split_7_2_o_du_application_unit_impl.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_translators.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_translator.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_translator.h"
#include "split_7_2_o_du_factory.h"
#include "split_7_2_o_du_unit_cli11_schema.h"
#include "split_7_2_o_du_unit_config_validator.h"
#include "split_7_2_o_du_unit_config_yaml_writer.h"
#include "split_7_2_o_du_unit_logger_registrator.h"

using namespace srsran;

void split_7_2_o_du_application_unit_impl::on_loggers_registration()
{
  register_split_7_2_o_du_loggers(unit_cfg);
}

void split_7_2_o_du_application_unit_impl::on_configuration_parameters_autoderivation(CLI::App& app)
{
  autoderive_split_7_2_o_du_parameters_after_parsing(app, unit_cfg);
}

bool split_7_2_o_du_application_unit_impl::on_configuration_validation(
    const os_sched_affinity_bitmask& available_cpus) const
{
  return validate_split_7_2_o_du_unit_config(unit_cfg, available_cpus);
}

split_7_2_o_du_application_unit_impl::split_7_2_o_du_application_unit_impl(std::string_view app_name)
{
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.f1ap.filename = fmt::format("/tmp/{}_f1ap.pcap", app_name);
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.f1u.filename  = fmt::format("/tmp/{}_f1u.pcap", app_name);
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.rlc.filename  = fmt::format("/tmp/{}_rlc.pcap", app_name);
  unit_cfg.odu_high_cfg.du_high_cfg.config.pcaps.mac.filename  = fmt::format("/tmp/{}_mac.pcap", app_name);
  // Note: do not update the default e2ap pcap filename.
}

void split_7_2_o_du_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_split_7_2_o_du_unit_config_schema(app, unit_cfg);
}

o_du_unit split_7_2_o_du_application_unit_impl::create_flexible_o_du_unit(const o_du_unit_dependencies& dependencies,
                                                                          bool                          use_multicell)
{
  return use_multicell ? multicell_split_7_2_du_factory(unit_cfg).create_flexible_o_du(dependencies)
                       : split_7_2_o_du_factory(unit_cfg).create_flexible_o_du(dependencies);
}

std::unique_ptr<flexible_o_du_application_unit> srsran::create_flexible_o_du_application_unit(std::string_view app_name)
{
  return std::make_unique<split_7_2_o_du_application_unit_impl>(app_name);
}

void split_7_2_o_du_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_split_7_2_o_du_unit_config_in_yaml_schema(node, unit_cfg);
}

void split_7_2_o_du_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  // OFH always runs in non blocking mode.
  bool     is_blocking_mode_enable = false;
  unsigned nof_cells               = unit_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.size();
  fill_o_du_high_worker_manager_config(config, unit_cfg.odu_high_cfg, is_blocking_mode_enable);
  fill_du_low_worker_manager_config(config, unit_cfg.du_low_cfg, is_blocking_mode_enable, nof_cells);
  auto cells = generate_du_cell_config(unit_cfg.odu_high_cfg.du_high_cfg.config);
  fill_ofh_worker_manager_config(config, unit_cfg.ru_cfg.config, cells);
}
