/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  auto radio = create_radio_session(*config.radio_exec, *radio_event_counter, config.radio_cfg, config.device_driver);
  report_error_if_not(radio, "Unable to create radio session.");

  ru_generic_impl_config       ru_config = {config.lower_phy_config.front().srate.to_MHz(), config.are_metrics_enabled};
  ru_generic_impl_dependencies ru_dependencies = {*config.symbol_notifier,
                                                  *config.timing_notifier,
                                                  *config.lower_phy_config.front().logger,
                                                  *config.error_notifier,
                                                  std::move(radio_event_counter),
                                                  std::move(radio)};

  auto ru = std::make_unique<ru_generic_impl>(ru_config, std::move(ru_dependencies));

  std::vector<std::unique_ptr<lower_phy_sector>> phy_sectors;
  for (unsigned sector_id = 0, sector_end = config.lower_phy_config.size(); sector_id != sector_end; ++sector_id) {
    lower_phy_configuration& low_cfg = config.lower_phy_config[sector_id];

    // Update the config with the adapters.
    low_cfg.bb_gateway         = &ru->get_baseband_gateway(sector_id);
    low_cfg.rx_symbol_notifier = &ru->get_rx_symbol_notifier();

    phy_sectors.push_back(create_low_phy_sector(config.max_nof_prach_concurrent_requests,
                                                low_cfg,
                                                ru->get_error_notifier(),
                                                sector_id ? nullptr : &ru->get_timing_notifier()));
  }

  // Add lower PHY sector dependencies.
  ru->set_lower_phy_sectors(std::move(phy_sectors));

  return ru;
}
