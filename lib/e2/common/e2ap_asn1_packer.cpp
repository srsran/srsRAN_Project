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

#include "e2ap_asn1_packer.h"
#include "srsran/asn1/e2ap/e2ap.h"

using namespace srsran;

e2ap_asn1_packer::e2ap_asn1_packer(sctp_network_gateway_data_handler& gw_,
                                   e2_message_handler&                e2_handler,
                                   dlt_pcap&                          pcap_) :
  logger(srslog::fetch_basic_logger("E2-ASN1-PCK")), gw(gw_), e2(e2_handler), pcap(pcap_)
{
}

void e2ap_asn1_packer::handle_packed_pdu(const byte_buffer& bytes)
{
  logger.info("Received PDU of {} bytes", bytes.length());

  if (pcap.is_write_enabled()) {
    pcap.push_pdu(bytes.copy());
  }

  asn1::cbit_ref bref(bytes);
  e2_message     msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Couldn't unpack E2 PDU");
    return;
  }

  // Call packet handler.
  e2.handle_message(msg);
}

void e2ap_asn1_packer::handle_message(const e2_message& msg)
{
  // pack PDU into temporary buffer
  byte_buffer   tx_pdu;
  asn1::bit_ref bref(tx_pdu);
  if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack E2 PDU");
    return;
  }

  if (pcap.is_write_enabled()) {
    pcap.push_pdu(tx_pdu.copy());
  }

  gw.handle_pdu(tx_pdu);
}
