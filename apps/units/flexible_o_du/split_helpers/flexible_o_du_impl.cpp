/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

flexible_o_du_impl::flexible_o_du_impl(unsigned nof_cells_) :
  nof_cells(nof_cells_), ru_ul_adapt(nof_cells_), ru_timing_adapt(nof_cells_), ru_error_adapt(nof_cells_)
{
}

void flexible_o_du_impl::start()
{
  du->get_power_controller().start();
  ru->get_controller().start();
}

void flexible_o_du_impl::stop()
{
  ru->get_controller().stop();
  du->get_power_controller().stop();
}

void flexible_o_du_impl::add_ru(std::unique_ptr<radio_unit> active_ru)
{
  ru = std::move(active_ru);
  srsran_assert(ru, "Invalid Radio Unit");

  // Connect the RU adaptor to the RU.
  ru_dl_rg_adapt.connect(ru->get_downlink_plane_handler());
  ru_ul_request_adapt.connect(ru->get_uplink_plane_handler());
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
