/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1ap/du/f1ap_du.h"
#include "srsgnb/gateways/network_gateway.h"

namespace srsgnb {

/// This implementation forwards all F1AP messages through a network gateway. It is useful for distributed scenarios
/// where the F1 interfaces reside in different machines.
class remote_f1ap_pdu_handler : public f1ap_message_handler
{
  network_gateway_data_handler& network;

public:
  explicit remote_f1ap_pdu_handler(network_gateway_data_handler& network) : network(network) {}

  void handle_message(const asn1::f1ap::f1ap_pdu_c& msg) override
  {
    // Pack PDU into a temporary buffer.
    byte_buffer   tx_pdu;
    asn1::bit_ref bref(tx_pdu);
    if (msg.pack(bref) != asn1::SRSASN_SUCCESS) {
      return;
    }

    network.handle_pdu(tx_pdu);
  }
};

} // namespace srsgnb
