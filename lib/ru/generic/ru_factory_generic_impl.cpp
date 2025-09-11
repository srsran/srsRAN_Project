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

#include "lower_phy/lower_phy_factory.h"
#include "ru_config_validator.h"
#include "ru_generic_error_adapter.h"
#include "ru_generic_impl.h"
#include "ru_radio_notifier_handler.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/ru/generic/ru_generic_factory.h"

using namespace srsran;

/// Creates a radio session with the given parameters.
static std::unique_ptr<radio_session> create_radio_session(task_executor&              executor,
                                                           radio_notification_handler& radio_handler,
                                                           radio_configuration::radio& config,
                                                           const std::string&          device_driver)
{
  print_available_radio_factories();

  std::unique_ptr<radio_factory> factory = create_radio_factory(device_driver);
  if (!factory) {
    return nullptr;
  }

  if (!factory->get_configuration_validator().is_configuration_valid(config)) {
    report_error("Invalid radio configuration.");
  }

  return factory->create(config, executor, radio_handler);
}

std::unique_ptr<radio_unit> srsran::create_generic_ru(ru_generic_configuration& config)
{
  // Check the pointers inside the config.
  if (!is_valid(config)) {
    return nullptr;
  }

  ru_generic_impl_config ru_config = {
      config.lower_phy_config.front().srate.to_MHz(), config.start_time, config.are_metrics_enabled};
  ru_generic_impl_dependencies ru_dependencies = {*config.symbol_notifier,
                                                  *config.timing_notifier,
                                                  *config.lower_phy_config.front().logger,
                                                  *config.rf_logger,
                                                  *config.error_notifier};

  auto ru = std::make_unique<ru_generic_impl>(ru_config, std::move(ru_dependencies));

  auto radio = create_radio_session(
      *config.radio_exec, ru->get_radio_notification_handler(), config.radio_cfg, config.device_driver);
  report_error_if_not(radio, "Unable to create radio session.");

  ru->set_radio(std::move(radio));

  std::vector<std::unique_ptr<lower_phy_sector>> phy_sectors;
  for (unsigned sector_id = 0, sector_end = config.lower_phy_config.size(); sector_id != sector_end; ++sector_id) {
    lower_phy_configuration& low_cfg = config.lower_phy_config[sector_id];

    // Update the config with the adapters.
    low_cfg.bb_gateway         = &ru->get_baseband_gateway(sector_id);
    low_cfg.rx_symbol_notifier = &ru->get_rx_symbol_notifier();

    phy_sectors.push_back(
        create_low_phy_sector(low_cfg, ru->get_error_notifier(), sector_id ? nullptr : &ru->get_timing_notifier()));
  }

  // Add lower PHY sector dependencies.
  ru->set_lower_phy_sectors(std::move(phy_sectors));

  return ru;
}
