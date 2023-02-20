/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_asn1_packer.h"

namespace srsran {

ngap_asn1_packer::ngap_asn1_packer(sctp_network_gateway_data_handler& gw_,
                                   ngap_message_handler&              ngap_handler,
                                   ngap_pcap&                         pcap_) :
  logger(srslog::fetch_basic_logger("NGAP-ASN1-PCK")), gw(gw_), ngap(ngap_handler), pcap(pcap_)
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

} // namespace srsran
