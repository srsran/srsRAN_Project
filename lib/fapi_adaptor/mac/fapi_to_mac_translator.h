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

#include "srsgnb/fapi/slot_message_notifier.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {
namespace fapi_adaptor {

class slot_event_dispatcher;

/// \brief This class listens to FAPI slot based messages events converting them to MAC specific events and dispatching
/// them to the registered clients.
class fapi_to_mac_translator : public fapi::slot_message_notifier
{
public:
  fapi_to_mac_translator(subcarrier_spacing scs, slot_event_dispatcher& slot_dispatcher) :
    scs(scs), slot_dispatcher(slot_dispatcher)
  {
  }

  // See interface for documentation.
  void on_dl_tti_response(const fapi::dl_tti_response_message& msg) override;

  // See interface for documentation.
  void on_slot_indication(const fapi::slot_indication_message& msg) override;

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

private:
  // :TODO: subcarrier spacing should be retrieved from the cells configuration in the future.
  const subcarrier_spacing scs;
  slot_event_dispatcher&   slot_dispatcher;
};

} // namespace fapi_adaptor
} // namespace srsgnb
