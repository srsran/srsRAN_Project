/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/ofh/transmitter/ofh_transmitter.h"
#include "srsran/ru/ru_uplink_plane.h"

using namespace srsran;

ru_ofh_impl::ru_ofh_impl(srslog::basic_logger& logger, ru_ofh_impl_dependencies&& dependencies) :
  sectors(std::move(dependencies.sectors)),
  ul_data_notifier(std::move(dependencies.ul_data_notifier)),
  symbol_notifier(std::move(dependencies.symbol_notifier)),
  controller(logger,
             std::move(dependencies.timing_controller),
             [](span<std::unique_ptr<ofh::sector>> elems) {
               std::vector<ofh::controller*> controllers;
               for (const auto& elem : elems) {
                 controllers.push_back(&elem.get()->get_controller());
               }
               return controllers;
             }(sectors)),
  downlink_handler([](span<std::unique_ptr<ofh::sector>> sects) {
    std::vector<ofh::downlink_handler*> out;
    for (unsigned i = 0, e = sects.size(); i != e; ++i) {
      out.emplace_back(&sects[i].get()->get_transmitter().get_downlink_handler());
    }
    return out;
  }(sectors)),
  uplink_handler([](span<std::unique_ptr<ofh::sector>> sects) {
    std::vector<ofh::uplink_request_handler*> out;
    for (unsigned i = 0, e = sects.size(); i != e; ++i) {
      out.emplace_back(&sects[i].get()->get_transmitter().get_uplink_request_handler());
    }
    return out;
  }(sectors))
{
  srsran_assert(std::all_of(sectors.begin(), sectors.end(), [](const auto& elem) { return elem != nullptr; }),
                "Invalid sector");
  srsran_assert(ul_data_notifier, "Invalid uplink data notifier");
  srsran_assert(symbol_notifier, "Invalid symbol notifier");
}

ru_downlink_plane_handler& ru_ofh_impl::get_downlink_plane_handler()
{
  return downlink_handler;
}

ru_uplink_plane_handler& ru_ofh_impl::get_uplink_plane_handler()
{
  return uplink_handler;
}

ru_controller& ru_ofh_impl::get_controller()
{
  return controller;
}
