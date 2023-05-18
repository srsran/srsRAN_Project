/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_factory_generic_impl.h"
#include "adaptors/ru_timing_adaptor.h"
#include "adaptors/rx_symbol_adaptor.h"
#include "lower_phy_factory.h"
#include "ru_generic_impl.h"
#include "ru_radio_notifier_sample.h"
#include "srsran/phy/adapters/phy_error_adapter.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/ru/ru_config.h"

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

/// Validates the pointer members of the given configuration.
static bool validate_config(ru_config& config)
{
  if (!config.symbol_notifier) {
    return false;
  }

  if (!config.timing_notifier) {
    return false;
  }

  if (!config.rf_logger) {
    return false;
  }

  if (!config.radio_exec) {
    return false;
  }

  return true;
}

std::unique_ptr<radio_unit> ru_factory_generic_impl::create(ru_config& config)
{
  // Check the pointers inside the config.
  if (!validate_config(config)) {
    return nullptr;
  }

  std::unique_ptr<radio_notification_handler> radio_event_logger;
  if (config.rf_logger->warning.enabled()) {
    radio_event_logger = std::make_unique<ru_radio_notification_handler_logger>(nullptr, *config.rf_logger);
  }

  // Create radio.
  std::unique_ptr<ru_radio_notification_handler_counter> radio_event_counter =
      std::make_unique<ru_radio_notification_handler_counter>(std::move(radio_event_logger));

  auto radio = build_radio(*config.radio_exec, *radio_event_counter, config.radio_cfg, config.device_driver);
  report_fatal_error_if_not(radio, "Unable to create radio session.");

  // Create adaptors.
  std::unique_ptr<phy_error_adapter>    phy_err_printer = std::make_unique<phy_error_adapter>("info");
  std::unique_ptr<ru_rx_symbol_adaptor> ru_rx_adaptor = std::make_unique<ru_rx_symbol_adaptor>(*config.symbol_notifier);
  std::unique_ptr<ru_timing_adaptor>    ru_time_adaptor = std::make_unique<ru_timing_adaptor>(*config.timing_notifier);

  // Update the config with the adaptors.
  lower_phy_configuration& low_cfg = config.lower_phy_config;
  low_cfg.bb_gateway               = &radio->get_baseband_gateway();
  low_cfg.rx_symbol_notifier       = ru_rx_adaptor.get();
  low_cfg.timing_notifier          = ru_time_adaptor.get();
  low_cfg.error_notifier           = phy_err_printer.get();

  // Create lower PHY.
  auto lower = create_lower_phy(config.lower_phy_config, config.max_nof_prach_concurrent_requests);
  report_fatal_error_if_not(lower, "Unable to create lower PHY.");

  // Create RU and return.
  ru_generic_impl_config ru_cfg;
  ru_cfg.srate_MHz           = config.lower_phy_config.srate.to_MHz();
  ru_cfg.radio_event_counter = std::move(radio_event_counter);
  ru_cfg.phy_err_printer     = std::move(phy_err_printer);
  ru_cfg.ru_rx_adaptor       = std::move(ru_rx_adaptor);
  ru_cfg.ru_time_adaptor     = std::move(ru_time_adaptor);
  ru_cfg.radio               = std::move(radio);
  ru_cfg.low_phy             = std::move(lower);

  return std::make_unique<ru_generic_impl>(std::move(ru_cfg));
}

std::unique_ptr<ru_factory> srsran::create_ru_factory()
{
  return std::make_unique<ru_factory_generic_impl>();
}
