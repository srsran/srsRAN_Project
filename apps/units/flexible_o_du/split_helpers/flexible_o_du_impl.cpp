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

#include "flexible_o_du_impl.h"
#include "srsran/du/du_low/du_low.h"
#include "srsran/du/du_low/o_du_low.h"
#include "srsran/du/o_du.h"
#include "srsran/phy/upper/upper_phy.h"
#include "srsran/ru/ru.h"
#include "srsran/ru/ru_controller.h"

using namespace srsran;

flexible_o_du_impl::flexible_o_du_impl(unsigned nof_cells_, flexible_o_du_metrics_notifier* notifier) :
  nof_cells(nof_cells_),
  ru_ul_adapt(nof_cells_),
  ru_timing_adapt(nof_cells_),
  ru_error_adapt(nof_cells_),
  odu_metrics_handler(notifier)
{
}

void flexible_o_du_impl::start()
{
  du->get_operation_controller().start();
  ru->get_controller().get_operation_controller().start();
}

void flexible_o_du_impl::stop()
{
  ru->get_controller().get_operation_controller().stop();
  du->get_operation_controller().stop();
}

void flexible_o_du_impl::add_ru(std::unique_ptr<radio_unit> active_ru)
{
  ru = std::move(active_ru);
  srsran_assert(ru, "Invalid Radio Unit");

  // Connect the RU adaptor to the RU.
  ru_dl_rg_adapt.connect(ru->get_downlink_plane_handler());
  ru_ul_request_adapt.connect(ru->get_uplink_plane_handler());

  // Update the RU metrics collector.
  if (auto* collector = ru->get_metrics_collector()) {
    odu_metrics_handler.set_ru_metrics_collector(*collector);
  }
}

void flexible_o_du_impl::add_du(std::unique_ptr<srs_du::o_du> active_du)
{
  du = std::move(active_du);
  srsran_assert(du, "Cannot set an invalid DU");

  // Connect all the sectors of the DU low to the RU adaptors.
  for (unsigned i = 0; i != nof_cells; ++i) {
    auto& upper = du->get_o_du_low().get_du_low().get_upper_phy(i);
    // Make connections between DU and RU.
    ru_ul_adapt.map_handler(i, upper.get_rx_symbol_handler());
    ru_timing_adapt.map_handler(i, upper.get_timing_handler());
    ru_error_adapt.map_handler(i, upper.get_error_handler());
  }
}
