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

#include "split6_flexible_o_du_low_session.h"
#include "srsran/du/du_low/du_low.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/fapi_adaptor/fapi_operation_controller.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_sector_adaptor.h"
#include "srsran/phy/upper/upper_phy.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

split6_flexible_o_du_low_session::~split6_flexible_o_du_low_session()
{
  // Stop adaptor.
  slot_adaptor->get_operation_controller().stop();

  // Stop RU.
  ru->get_controller().get_operation_controller().stop();

  // Stop O-DU low.
  odu_low->get_operation_controller().stop();
}

void split6_flexible_o_du_low_session::set_dependencies(std::unique_ptr<fapi::slot_messages_adaptor> slot_msg_adaptor,
                                                        std::unique_ptr<srs_du::o_du_low>            du,
                                                        std::unique_ptr<radio_unit>                  radio,
                                                        unique_timer                                 timer)
{
  srsran_assert(slot_msg_adaptor, "Invalid FAPI slot message adaptor");
  srsran_assert(du, "Invalid O-DU low");
  srsran_assert(radio, "Invalid Radio Unit");

  slot_adaptor = std::move(slot_msg_adaptor);
  odu_low      = std::move(du);
  ru           = std::move(radio);

  // Connect the RU adaptor to the RU.
  ru_dl_rg_adapt.connect(ru->get_downlink_plane_handler());
  ru_ul_request_adapt.connect(ru->get_uplink_plane_handler());

  // Connect all the sectors of the DU low to the RU adaptors.
  for (unsigned i = 0; i != NOF_CELLS_SUPPORTED; ++i) {
    // Make connections between DU and RU.
    auto& upper = odu_low->get_du_low().get_upper_phy(i);
    ru_ul_adapt.map_handler(i, upper.get_rx_symbol_handler());
    ru_timing_adapt.map_handler(i, upper.get_timing_handler());
    ru_error_adapt.map_handler(i, upper.get_error_handler());

    // Connect adaptor with O-DU low.
    auto& fapi_adaptor = odu_low->get_phy_fapi_adaptor().get_sector_adaptor(i);
    fapi_adaptor.set_slot_time_message_notifier(slot_adaptor->get_slot_time_message_notifier());
    fapi_adaptor.set_slot_data_message_notifier(slot_adaptor->get_slot_data_message_notifier());
    fapi_adaptor.set_error_message_notifier(slot_adaptor->get_error_message_notifier());
  }

  odu_low->get_operation_controller().start();
  ru->get_controller().get_operation_controller().start();

  // Initialize metrics collector.
  if (report_period.count() != 0 && notifier) {
    metrics_collector = split6_o_du_low_metrics_collector_impl(
        odu_low->get_metrics_collector(), ru->get_metrics_collector(), notifier, std::move(timer), report_period);
  }
}
