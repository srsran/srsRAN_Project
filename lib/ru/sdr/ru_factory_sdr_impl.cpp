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
#include "ru_lower_phy_error_adapter.h"
#include "ru_radio_event_handler.h"
#include "ru_sdr_impl.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/ru/sdr/ru_sdr_configuration.h"
#include "srsran/ru/sdr/ru_sdr_factory.h"

using namespace srsran;

/// Creates a radio session with the given parameters.
static std::unique_ptr<radio_session> create_radio_session(task_executor&                    executor,
                                                           radio_event_notifier&             radio_handler,
                                                           const radio_configuration::radio& config,
                                                           const std::string&                device_driver)
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

std::unique_ptr<radio_unit> srsran::create_sdr_ru(const ru_sdr_configuration& config,
                                                  const ru_sdr_dependencies&  dependencies)
{
  ru_sdr_impl_config ru_config = {.srate_MHz           = config.radio_cfg.sampling_rate_Hz * 1e-6,
                                  .start_time          = config.start_time,
                                  .are_metrics_enabled = config.are_metrics_enabled};

  ru_sdr_impl_dependencies ru_dependencies = {.rx_symbol_handler = dependencies.symbol_notifier,
                                              .timing_handler    = dependencies.timing_notifier,
                                              .logger            = dependencies.sector_dependencies.front().logger,
                                              .radio_logger      = dependencies.rf_logger,
                                              .error_notifier    = dependencies.error_notifier};

  auto ru = std::make_unique<ru_sdr_impl>(ru_config, ru_dependencies);

  auto radio = create_radio_session(
      dependencies.radio_exec, ru->get_radio_event_notifier(), config.radio_cfg, config.device_driver);
  report_error_if_not(radio, "Unable to create radio session.");

  ru->set_radio(std::move(radio));

  std::vector<std::unique_ptr<lower_phy_sector>> phy_sectors;
  for (unsigned sector_id = 0, sector_end = config.lower_phy_config.size(); sector_id != sector_end; ++sector_id) {
    const lower_phy_configuration&    lower_phy_cfg  = config.lower_phy_config[sector_id];
    const ru_sdr_sector_dependencies& ru_sector_deps = dependencies.sector_dependencies[sector_id];

    lower_phy_sector_dependencies lophy_sector_deps = {
        .logger               = ru_sector_deps.logger,
        .rx_task_executor     = ru_sector_deps.rx_task_executor,
        .tx_task_executor     = ru_sector_deps.tx_task_executor,
        .dl_task_executor     = ru_sector_deps.dl_task_executor,
        .ul_task_executor     = ru_sector_deps.ul_task_executor,
        .prach_async_executor = ru_sector_deps.prach_async_executor,
        .error_notifier       = ru->get_error_notifier(),
        // Only the first sector is used to report timing events using the RU notifier.
        .timing_notifier    = sector_id ? nullptr : &ru->get_timing_notifier(),
        .bb_gateway         = ru->get_baseband_gateway(sector_id),
        .rx_symbol_notifier = ru->get_rx_symbol_notifier()};

    phy_sectors.push_back(create_lower_phy_sector(lower_phy_cfg, lophy_sector_deps));
  }

  // Add lower PHY sector dependencies.
  ru->set_lower_phy_sectors(std::move(phy_sectors));

  return ru;
}
