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

#include "split6_o_du_low_application_unit_impl.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_translator.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_yaml_writer.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_translator.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_yaml_writer.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_translator.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_yaml_writer.h"
#include "metrics/split6_flexible_o_du_low_metrics_builder.h"
#include "split6_constants.h"
#include "split6_flexible_o_du_low.h"
#include "split6_flexible_o_du_low_session_factory.h"
#include "split6_o_du_low_unit_cli11_schema.h"
#include "split6_o_du_low_unit_config_validator.h"
#include "split6_o_du_low_unit_logger_registrator.h"

using namespace srsran;

split6_o_du_low_application_unit_impl::split6_o_du_low_application_unit_impl(
    std::string_view                        app_name,
    std::unique_ptr<split6_o_du_low_plugin> plugin_) :
  plugin(std::move(plugin_))
{
  srsran_assert(plugin, "Invalid split 6 O-DU low plugin");
}

void split6_o_du_low_application_unit_impl::on_loggers_registration()
{
  register_split6_o_du_low_loggers(unit_cfg);
  plugin->on_loggers_registration();
}

bool split6_o_du_low_application_unit_impl::are_metrics_enabled() const
{
  return unit_cfg.du_low_cfg.metrics_cfg.common_metrics_cfg.enabled();
}

void split6_o_du_low_application_unit_impl::on_configuration_parameters_autoderivation(CLI::App& app)
{
  autoderive_split6_o_du_low_parameters_after_parsing(app, unit_cfg);
}

bool split6_o_du_low_application_unit_impl::on_configuration_validation() const
{
  if (!plugin->on_configuration_validation()) {
    return false;
  }

  return validate_split6_o_du_low_unit_config(unit_cfg);
}

void split6_o_du_low_application_unit_impl::on_parsing_configuration_registration(CLI::App& app)
{
  configure_cli11_with_split6_o_du_low_unit_config_schema(app, unit_cfg);
  plugin->on_parsing_configuration_registration(app);
}

void split6_o_du_low_application_unit_impl::dump_config(YAML::Node& node) const
{
  fill_du_low_config_in_yaml_schema(node, unit_cfg.du_low_cfg);

  if (auto* ru_ofh = std::get_if<ru_ofh_unit_parsed_config>(&unit_cfg.ru_cfg)) {
    fill_ru_ofh_config_in_yaml_schema(node, ru_ofh->config);
  } else if (auto* ru_sdr = std::get_if<ru_sdr_unit_config>(&unit_cfg.ru_cfg)) {
    fill_ru_sdr_config_in_yaml_schema(node, *ru_sdr);
  }
}

void split6_o_du_low_application_unit_impl::fill_worker_manager_config(worker_manager_config& config)
{
  // Split 6 always runs in non blocking mode.
  bool is_blocking_mode_enable = false;
  plugin->fill_worker_manager_config(config);

  config.config_affinities.resize(split6_du_low::NOF_CELLS_SUPPORTED);
  std::vector<unsigned> max_antennas_supported_per_cell = {split6_du_low::NOF_TX_ANTENNA_SUPPORTED};
  fill_du_low_worker_manager_config(config,
                                    unit_cfg.du_low_cfg,
                                    is_blocking_mode_enable,
                                    max_antennas_supported_per_cell,
                                    max_antennas_supported_per_cell);

  if (auto* ru_sdr = std::get_if<ru_sdr_unit_config>(&unit_cfg.ru_cfg)) {
    fill_sdr_worker_manager_config(config, *ru_sdr);
  } else if (auto* ru_ofh = std::get_if<ru_ofh_unit_parsed_config>(&unit_cfg.ru_cfg)) {
    fill_ofh_worker_manager_config(config, ru_ofh->config);
  }
}

split6_o_du_low_unit
split6_o_du_low_application_unit_impl::create_flexible_o_du_low(worker_manager&                 workers,
                                                                app_services::metrics_notifier& metrics_notifier,
                                                                timer_manager&                  timers,
                                                                srslog::basic_logger&           logger)
{
  split6_o_du_low_unit output;

  // :TODO: Cyclic prefix and subcarrier spacing are hardcoded.
  std::chrono::nanoseconds symbol_duration(static_cast<int64_t>(
      SUBFRAME_DURATION_MSEC * 1e6 /
      (get_nsymb_per_slot(cyclic_prefix::NORMAL) * get_nof_slots_per_subframe(subcarrier_spacing::kHz30))));

  // :TODO: 0 is hardcoded to the cell id. Difficult to add here the PCI as it is a parameter that will be readed
  // dynamically after the CONFIG.request FAPI message.
  auto notifier = build_split6_flexible_o_du_low_metrics_config(
      output.metrics, metrics_notifier, unit_cfg.du_low_cfg.metrics_cfg.common_metrics_cfg, {0}, symbol_duration);

  // Split 6 flexible O-DU low manager dependencies.
  split6_flexible_o_du_low_dependencies dependencies;
  dependencies.config_interface_collection = fapi::create_fapi_config_message_interface_collection(logger);
  dependencies.config_adaptor =
      plugin->create_config_messages_adaptor(dependencies.config_interface_collection->get_config_message_gateway(),
                                             *workers.split6_exec,
                                             *workers.split6_crtl_exec);
  dependencies.odu_low_session_factory = std::make_unique<split6_flexible_o_du_low_session_factory>(
      unit_cfg,
      workers,
      timers,
      notifier,
      plugin->create_slot_messages_adaptor_factory(*workers.split6_exec, *workers.split6_crtl_exec));

  auto odu_low = std::make_unique<split6_flexible_o_du_low>(std::move(dependencies));

  output.odu_low = std::move(odu_low);

  return output;
}

std::unique_ptr<split6_o_du_low_application_unit_impl>
srsran::create_flexible_o_du_low_application_unit(std::string_view app_name)
{
  return std::make_unique<split6_o_du_low_application_unit_impl>(app_name, create_split6_o_du_low_plugin(app_name));
}
