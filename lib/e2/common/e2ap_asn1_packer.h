/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/byte_buffer.h"
#include "srsran/e2/e2.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/pcap/pcap.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// This E2AP packer class is used to pack outgoing and unpack incoming E2 message in ASN1 format.
class e2ap_asn1_packer : public e2ap_packer
{
public:
  explicit e2ap_asn1_packer(sctp_network_gateway_data_handler& gw, e2_message_handler& e2, dlt_pcap& pcap_);

  /// Received packed E2AP PDU that needs to be unpacked and forwarded.
  void handle_packed_pdu(const byte_buffer& pdu) override;

  /// Receive populated ASN1 struct that needs to be packed and forwarded.
  void handle_message(const e2_message& msg) override;

private:
  srslog::basic_logger&              logger;
  sctp_network_gateway_data_handler& gw;
  e2_message_handler&                e2;
  dlt_pcap&                          pcap;
};

} // namespace srsran
