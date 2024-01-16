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

#include "ngap_asn1_packer.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srs_cu_cp;

ngap_asn1_packer::ngap_asn1_packer(sctp_network_gateway_data_handler& gw_,
                                   ngap_message_handler&              ngap_handler,
                                   dlt_pcap&                          pcap_) :
  logger(srslog::fetch_basic_logger("NGAP")), gw(gw_), ngap(ngap_handler), pcap(pcap_)
{
}

// Received packed NGAP PDU that needs to be unpacked and forwarded.
void ngap_asn1_packer::handle_packed_pdu(const byte_buffer& bytes)
{
  if (pcap.is_write_enabled()) {
    pcap.push_pdu(bytes.copy());
  }
  asn1::cbit_ref          bref(bytes);
  srs_cu_cp::ngap_message msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Couldn't unpack PDU");
    return;
  }

  // call packet handler
  ngap.handle_message(msg);
}

// Receive populated ASN1 struct that needs to be packed and forwarded.
void ngap_asn1_packer::handle_message(const srs_cu_cp::ngap_message& msg)
{
  if (logger.debug.enabled()) {
    asn1::json_writer js;
    msg.pdu.to_json(js);
    logger.debug("Tx NGAP PDU: {}", js.to_string());
  }

  // pack PDU into temporary buffer
  byte_buffer   tx_pdu;
  asn1::bit_ref bref(tx_pdu);
  if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack PDU");
    return;
  }

  if (pcap.is_write_enabled()) {
    pcap.push_pdu(tx_pdu.copy());
  }
  gw.handle_pdu(tx_pdu);
}
