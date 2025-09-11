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

#include "ru_ofh_impl.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include "srsran/ofh/transmitter/ofh_transmitter.h"
#include "srsran/ru/ru_uplink_plane.h"

using namespace srsran;

ru_ofh_impl::ru_ofh_impl(const ru_ofh_impl_config& config, ru_ofh_impl_dependencies&& dependencies) :
  timing_notifier(config.nof_slot_offset_du_ru, config.nof_symbols_per_slot, config.scs, *dependencies.timing_notifier),
  error_handler(*dependencies.error_notifier),
  rx_symbol_handler(*dependencies.rx_symbol_notifier),
  ofh_timing_mngr(std::move(dependencies.timing_mngr)),
  controller(*dependencies.logger),
  metrics_collector(ofh_timing_mngr->get_metrics_collector())
{
  srsran_assert(ofh_timing_mngr, "Invalid Open Fronthaul timing manager");
}

ru_downlink_plane_handler& ru_ofh_impl::get_downlink_plane_handler()
{
  return downlink_handler;
}

ru_uplink_plane_handler& ru_ofh_impl::get_uplink_plane_handler()
{
  return uplink_handler;
}

ru_metrics_collector* ru_ofh_impl::get_metrics_collector()
{
  return metrics_collector.disabled() ? nullptr : &metrics_collector;
}

void ru_ofh_impl::set_ofh_sectors(std::vector<std::unique_ptr<ofh::sector>> ofh_sectors)
{
  srsran_assert(!ofh_sectors.empty(), "Invalid OFH sectors");

  sectors = std::move(ofh_sectors);

  controller.set_sector_controllers(
      [](ofh::operation_controller* timing_ctrl, span<std::unique_ptr<ofh::sector>> elems) {
        std::vector<ofh::operation_controller*> controllers;
        // Insert first the timing controller.
        controllers.push_back(timing_ctrl);

        for (const auto& elem : elems) {
          controllers.push_back(&elem.get()->get_operation_controller());
        }
        return controllers;
      }(&ofh_timing_mngr->get_controller(), sectors));

  downlink_handler = ru_downlink_plane_handler_proxy([](span<std::unique_ptr<ofh::sector>> sectors_) {
    std::vector<ofh::downlink_handler*> out;
    for (const auto& sector : sectors_) {
      out.emplace_back(&sector.get()->get_transmitter().get_downlink_handler());
    }
    return out;
  }(sectors));

  uplink_handler = ru_uplink_plane_handler_proxy([](span<std::unique_ptr<ofh::sector>> sectors_) {
    std::vector<ofh::uplink_request_handler*> out;
    for (const auto& sector : sectors_) {
      out.emplace_back(&sector.get()->get_transmitter().get_uplink_request_handler());
    }
    return out;
  }(sectors));

  metrics_collector = ru_ofh_metrics_collector_impl(ofh_timing_mngr->get_metrics_collector(),
                                                    [](span<std::unique_ptr<ofh::sector>> sectors_) {
                                                      std::vector<ofh::metrics_collector*> out;
                                                      for (const auto& sector : sectors_) {
                                                        if (auto* collector = sector->get_metrics_collector()) {
                                                          out.emplace_back(collector);
                                                        }
                                                      }
                                                      return out;
                                                    }(sectors));

  // Subscribe the OTA symbol boundary notifiers.
  std::vector<ofh::ota_symbol_boundary_notifier*> notifiers;

  // Add first the timing notifier.
  notifiers.push_back(&timing_notifier);

  // Add the sectors notifiers.
  for (auto& sector : sectors) {
    notifiers.push_back(&sector->get_transmitter().get_ota_symbol_boundary_notifier());
    if (auto* notifier = sector->get_receiver().get_ota_symbol_boundary_notifier()) {
      notifiers.push_back(notifier);
    }
  }

  ofh_timing_mngr->get_ota_symbol_boundary_notifier_manager().subscribe(notifiers);
}

ru_controller& ru_ofh_impl::get_controller()
{
  return controller;
}
