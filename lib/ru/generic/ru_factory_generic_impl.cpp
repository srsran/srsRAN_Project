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

#include "lower_phy_factory.h"
#include "ru_config_validator.h"
#include "ru_generic_impl.h"
#include "ru_radio_notifier_handler.h"
#include "srsran/phy/adapters/phy_error_adapter.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/ru/ru_generic_factory.h"

using namespace srsran;

namespace {

/// Lower PHY timing notifier dummy.
class lower_phy_timing_notifier_dummy : public lower_phy_timing_notifier
{
public:
  void on_tti_boundary(const lower_phy_timing_context& context) override {}

  void on_ul_half_slot_boundary(const lower_phy_timing_context& context) override {}

  void on_ul_full_slot_boundary(const lower_phy_timing_context& context) override {}
};

} // namespace

/// Builds a radio session with the given parameters.
static std::unique_ptr<radio_session> build_radio(task_executor&              executor,
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
    report_error("Invalid radio configuration.\n");
  }

  return factory->create(config, executor, radio_handler);
}

static unsigned get_statistics_time_interval_in_slots(unsigned interval_seconds, subcarrier_spacing scs)
{
  return get_nof_slots_per_subframe(scs) * SUBFRAME_DURATION_MSEC * 1000 * interval_seconds;
}

std::unique_ptr<radio_unit> srsran::create_generic_ru(ru_generic_configuration& config)
{
  // Check the pointers inside the config.
  if (!is_valid(config)) {
    return nullptr;
  }

  std::unique_ptr<radio_notification_handler> radio_event_logger;
  if (config.rf_logger->warning.enabled()) {
    radio_event_logger = std::make_unique<ru_radio_notification_handler_logger>(nullptr, *config.rf_logger);
  }

  // Create radio.
  auto radio_event_counter = std::make_unique<ru_radio_notification_handler_counter>(
      std::move(radio_event_logger),
      *config.statistics_printer_executor,
      get_statistics_time_interval_in_slots(config.statistics_print_interval_s, config.lower_phy_config.front().scs));

  auto radio = build_radio(*config.radio_exec, *radio_event_counter, config.radio_cfg, config.device_driver);
  report_error_if_not(radio, "Unable to create radio session.");

  // Create received symbol adapter.
  auto ru_rx_adapter = std::make_unique<ru_rx_symbol_adapter>(*config.symbol_notifier);

  // Create RU and return.
  ru_generic_impl_config ru_cfg;
  ru_cfg.srate_MHz     = config.lower_phy_config.front().srate.to_MHz();
  ru_cfg.ru_rx_adapter = std::move(ru_rx_adapter);
  ru_cfg.radio         = std::move(radio);

  for (unsigned sector_id = 0, sector_end = config.lower_phy_config.size(); sector_id != sector_end; ++sector_id) {
    lower_phy_configuration& low_cfg = config.lower_phy_config[sector_id];
    low_cfg.sector_id                = sector_id;
    // Update the config with the adapters.
    low_cfg.bb_gateway         = &ru_cfg.radio->get_baseband_gateway(sector_id);
    low_cfg.rx_symbol_notifier = ru_cfg.ru_rx_adapter.get();

    ru_cfg.ru_time_adapter.push_back(
        (sector_id == 0) ? static_cast<std::unique_ptr<lower_phy_timing_notifier>>(std::make_unique<ru_timing_adapter>(
                               *config.timing_notifier, std::move(radio_event_counter)))
                         : static_cast<std::unique_ptr<lower_phy_timing_notifier>>(
                               std::make_unique<lower_phy_timing_notifier_dummy>()));
    low_cfg.timing_notifier = ru_cfg.ru_time_adapter.back().get();

    ru_cfg.phy_err_printer.push_back(std::make_unique<phy_error_adapter>(*low_cfg.logger));
    low_cfg.error_notifier = ru_cfg.phy_err_printer.back().get();

    // Create lower PHY factory.
    auto lphy_factory = create_lower_phy_factory(low_cfg, config.max_nof_prach_concurrent_requests);
    report_error_if_not(lphy_factory, "Failed to create lower PHY factory.");

    // Create lower PHY.
    ru_cfg.low_phy.push_back(lphy_factory->create(low_cfg));
    report_error_if_not(ru_cfg.low_phy.back(), "Unable to create lower PHY.");
  }

  return std::make_unique<ru_generic_impl>(std::move(ru_cfg));
}
