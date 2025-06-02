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

#include "split6_flexible_o_du_low_impl.h"
#include "srsran/du/du_low/du_low.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/phy/upper/upper_phy.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

split6_flexible_o_du_low_impl::~split6_flexible_o_du_low_impl()
{
  ru->get_controller().get_operation_controller().stop();
  odu_low->get_operation_controller().stop();

  // :TODO: plugin needs to be stopped?
}

void split6_flexible_o_du_low_impl::set_dependencies(std::unique_ptr<split6_slot_configurator_plugin> slot,
                                                     std::unique_ptr<srs_du::o_du_low>                du,
                                                     std::unique_ptr<radio_unit>                      radio)
{
  srsran_assert(slot, "Invalid split 6 slot plugin");
  srsran_assert(du, "Invalid O-DU low");
  srsran_assert(radio, "Invalid Radio Unit");

  slot_plugin = std::move(slot);
  odu_low     = std::move(du);
  ru          = std::move(radio);

  // Connect the RU adaptor to the RU.
  ru_dl_rg_adapt.connect(ru->get_downlink_plane_handler());
  ru_ul_request_adapt.connect(ru->get_uplink_plane_handler());

  // :TODO: add metrics collector for the RU?

  // Connect all the sectors of the DU low to the RU adaptors.
  for (unsigned i = 0; i != NOF_CELLS_SUPPORTED; ++i) {
    auto& upper = odu_low->get_du_low().get_upper_phy(i);
    // Make connections between DU and RU.
    ru_ul_adapt.map_handler(i, upper.get_rx_symbol_handler());
    ru_timing_adapt.map_handler(i, upper.get_timing_handler());
    ru_error_adapt.map_handler(i, upper.get_error_handler());
  }

  // :TODO: plugin needs to be started?

  odu_low->get_operation_controller().start();
  ru->get_controller().get_operation_controller().start();
}
