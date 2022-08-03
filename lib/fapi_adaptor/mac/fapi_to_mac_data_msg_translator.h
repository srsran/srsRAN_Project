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

namespace srsgnb {
namespace fapi_adaptor {

/// \brief This class listens to FAPI slot based data specific message events and translates them to the suitable data
/// types for the MAC layer.
class fapi_to_mac_data_msg_translator : public fapi::slot_data_message_notifier
{
public:
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
};

} // namespace fapi_adaptor
} // namespace srsgnb
