/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lower_phy_factory.h"
#include "ru_config_validator.h"
#include "ru_generic_impl.h"
#include "ru_radio_notifier_sample.h"
#include "srsran/phy/adapters/phy_error_adapter.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/ru/ru_generic_factory.h"

using namespace srsran;

/// Builds a radio session with the given parameters.
static std::unique_ptr<radio_session> build_radio(task_executor&              executor,
                                                  radio_notification_handler& radio_handler,
                                                  radio_configuration::radio& config,
                                                  const std::string&          device_driver)
{
  std::unique_ptr<radio_factory> factory = create_radio_factory(device_driver);
  if (!factory) {
    return nullptr;
  }

  if (!factory->get_configuration_validator().is_configuration_valid(config)) {
    report_error("Invalid radio configuration.\n");
  }

  return factory->create(config, executor, radio_handler);
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
  auto radio_event_counter = std::make_unique<ru_radio_notification_handler_counter>(std::move(radio_event_logger));

  auto radio = build_radio(*config.radio_exec, *radio_event_counter, config.radio_cfg, config.device_driver);
  report_fatal_error_if_not(radio, "Unable to create radio session.");

  // Create adapters.
  auto phy_err_printer = std::make_unique<phy_error_adapter>("info");
  auto ru_rx_adapter   = std::make_unique<ru_rx_symbol_adapter>(*config.symbol_notifier);
  auto ru_time_adapter = std::make_unique<ru_timing_adapter>(*config.timing_notifier);

  // Update the config with the adapters.
  lower_phy_configuration& low_cfg = config.lower_phy_config;
  low_cfg.bb_gateway               = &radio->get_baseband_gateway();
  low_cfg.rx_symbol_notifier       = ru_rx_adapter.get();
  low_cfg.timing_notifier          = ru_time_adapter.get();
  low_cfg.error_notifier           = phy_err_printer.get();

  // Create lower PHY factory.
  auto lphy_factory = create_lower_phy_factory(config.lower_phy_config, config.max_nof_prach_concurrent_requests);
  report_fatal_error_if_not(lphy_factory, "Failed to create lower PHY factory.");

  // Create lower PHY.
  auto lower = lphy_factory->create(config.lower_phy_config);
  report_fatal_error_if_not(lower, "Unable to create lower PHY.");

  // Create RU and return.
  ru_generic_impl_config ru_cfg;
  ru_cfg.srate_MHz           = config.lower_phy_config.srate.to_MHz();
  ru_cfg.radio_event_counter = std::move(radio_event_counter);
  ru_cfg.phy_err_printer     = std::move(phy_err_printer);
  ru_cfg.ru_rx_adapter       = std::move(ru_rx_adapter);
  ru_cfg.ru_time_adapter     = std::move(ru_time_adapter);
  ru_cfg.radio               = std::move(radio);
  ru_cfg.low_phy             = std::move(lower);

  return std::make_unique<ru_generic_impl>(std::move(ru_cfg));
}
