/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/e2/e2.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// This E2AP packer class is used to pack outgoing and unpack incoming E2 message in ASN1 format.
class e2ap_asn1_packer
{
public:
  explicit e2ap_asn1_packer(sctp_network_gateway_data_handler& gw, e2_message_handler& e2, dlt_pcap& pcap_);

  /// Received packed E2AP PDU that needs to be unpacked and forwarded.
  void handle_packed_pdu(const byte_buffer& pdu);

  /// Receive populated ASN1 struct that needs to be packed and forwarded.
  void handle_message(const e2_message& msg);

private:
  srslog::basic_logger&              logger;
  sctp_network_gateway_data_handler& gw;
  e2_message_handler&                e2;
  dlt_pcap&                          pcap;
};

} // namespace srsran
