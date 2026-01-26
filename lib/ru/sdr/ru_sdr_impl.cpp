/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "ru_sdr_impl.h"

using namespace srsran;

ru_sdr_impl::ru_sdr_impl(const ru_sdr_impl_config& config, const ru_sdr_impl_dependencies& dependencies) :
  are_metrics_enabled(config.are_metrics_enabled),
  radio_metrics_collector(),
  radio_event_logger(dependencies.radio_logger),
  radio_event_dispatcher({&radio_metrics_collector, &radio_event_logger}),
  error_adapter(dependencies.logger, dependencies.error_notifier),
  rx_adapter(dependencies.rx_symbol_handler),
  timing_adapter(dependencies.timing_handler),
  metrics_collector(radio_metrics_collector),
  radio_unit_controller(config.srate_MHz, config.start_time),
  ru_downlink_hdlr({}),
  ru_uplink_request_hdlr({})
{
}

void ru_sdr_impl::set_lower_phy_sectors(std::vector<std::unique_ptr<lower_phy_sector>> lower_phy_sectors)
{
  phy_sectors = std::move(lower_phy_sectors);

  srsran_assert(!phy_sectors.empty(), "SDR Radio Unit received an empty list of sectors");

  radio_unit_controller.set_lower_phy_sectors([this]() -> std::vector<lower_phy_sector*> {
    std::vector<lower_phy_sector*> sectors;
    sectors.reserve(phy_sectors.size());
    for (auto& sector : phy_sectors) {
      sectors.push_back(sector.get());
    }
    return sectors;
  }());

  metrics_collector.set_lower_phy_sectors([this]() -> std::vector<lower_phy_sector_metrics_collector*> {
    std::vector<lower_phy_sector_metrics_collector*> collectors;
    collectors.reserve(phy_sectors.size());
    for (auto& sector : phy_sectors) {
      collectors.push_back(&sector->get_metrics_collector());
    }
    return collectors;
  }());

  ru_downlink_hdlr = ru_lower_phy_downlink_handler_impl([this]() -> std::vector<lower_phy_downlink_handler*> {
    std::vector<lower_phy_downlink_handler*> handlers;
    handlers.reserve(phy_sectors.size());
    for (auto& sector : phy_sectors) {
      handlers.push_back(&sector->get_downlink_handler());
    }
    return handlers;
  }());

  ru_uplink_request_hdlr =
      ru_lower_phy_uplink_request_handler_impl([this]() -> std::vector<lower_phy_uplink_request_handler*> {
        std::vector<lower_phy_uplink_request_handler*> handlers;
        handlers.reserve(phy_sectors.size());
        for (auto& sector : phy_sectors) {
          handlers.push_back(&sector->get_uplink_request_handler());
        }
        return handlers;
      }());
}
