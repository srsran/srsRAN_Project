/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/fapi/slot_data_message_notifier.h"
#include "srsgnb/mac/mac_cell_control_information_handler.h"
#include "srsgnb/mac/mac_cell_rach_handler.h"
#include "srsgnb/mac/mac_pdu_handler.h"

namespace srsgnb {
namespace fapi_adaptor {

/// \brief FAPI-to-MAC data message translator.
///
/// This class listens to slot-based, data-specific FAPI message events and translates them into the suitable data types
/// for the MAC layer.
class fapi_to_mac_data_msg_translator : public fapi::slot_data_message_notifier
{
public:
  explicit fapi_to_mac_data_msg_translator(subcarrier_spacing scs);

  // See interface for documentation.
  void on_dl_tti_response(const fapi::dl_tti_response_message& msg) override;

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
  std::reference_wrapper<mac_cell_rach_handler>                rach_handler;
  std::reference_wrapper<mac_pdu_handler>                      pdu_handler;
  std::reference_wrapper<mac_cell_control_information_handler> crc_handler;

  // :TODO: subcarrier spacing should be retrieved from the cells configuration in the future.
  const subcarrier_spacing scs;
};

} // namespace fapi_adaptor
} // namespace srsgnb
