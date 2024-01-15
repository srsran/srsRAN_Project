/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "srsran/fapi/slot_data_message_notifier.h"
#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/mac/mac_cell_rach_handler.h"
#include "srsran/mac/mac_pdu_handler.h"

namespace srsran {
namespace fapi_adaptor {

/// \brief FAPI-to-MAC data message translator.
///
/// This class listens to slot-based, data-specific FAPI message events and translates them into the suitable data types
/// for the MAC layer.
class fapi_to_mac_data_msg_translator : public fapi::slot_data_message_notifier
{
public:
  explicit fapi_to_mac_data_msg_translator(subcarrier_spacing scs_);

  // See interface for documentation.
  void on_rx_data_indication(const fapi::rx_data_indication_message& msg) override;

  // See interface for documentation.
  void on_crc_indication(const fapi::crc_indication_message& msg) override;

  // See interface for documentation.
  void on_uci_indication(const fapi::uci_indication_message& msg) override;

  // See interface for documentation.
  void on_srs_indication(const fapi::srs_indication_message& msg) override;

  // See interface for documentation.
  void on_rach_indication(const fapi::rach_indication_message& msg) override;

  /// Sets the given cell-specific MAC RACH handler. This handler will be used to receive new RACH notifications.
  void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler);

  /// Sets the given cell-specific MAC PDU handler. This handler will be used to receive new PDU notifications.
  void set_cell_pdu_handler(mac_pdu_handler& handler);

  /// Sets the given cell-specific MAC CRC handler. This handler will be used to receive new CRC notifications.
  void set_cell_crc_handler(mac_cell_control_information_handler& handler);

private:
  const subcarrier_spacing                                     scs;
  std::reference_wrapper<mac_cell_rach_handler>                rach_handler;
  std::reference_wrapper<mac_pdu_handler>                      pdu_handler;
  std::reference_wrapper<mac_cell_control_information_handler> crc_handler;
};

} // namespace fapi_adaptor
} // namespace srsran
