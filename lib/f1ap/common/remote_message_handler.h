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

#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/gateways/network_gateway.h"

namespace srsran {

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

} // namespace srsran
