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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/e2/e2.h"
#include "srsgnb/gateways/sctp_network_gateway.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

class e2ap_asn1_packer : public e2_message_handler
{
public:
  explicit e2ap_asn1_packer(sctp_network_gateway_data_handler& gw, e2_message_handler& e2);

  /// Received packed E2AP PDU that needs to be unpacked and forwarded.
  void handle_packed_pdu(const byte_buffer& pdu);

  /// Receive populated ASN1 struct that needs to be packed and forwarded.
  void handle_message(const e2_message& msg) override;

private:
  srslog::basic_logger&              logger;
  sctp_network_gateway_data_handler& gw;
  e2_message_handler&                e2;
};

} // namespace srsgnb
