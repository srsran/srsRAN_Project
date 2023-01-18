/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngc_asn1_packer.h"

namespace srsgnb {

ngc_asn1_packer::ngc_asn1_packer(sctp_network_gateway_data_handler& gw_, ngc_message_handler& ngc_handler) :
  logger(srslog::fetch_basic_logger("NGC-ASN1-PCK")), gw(gw_), ngc(ngc_handler)
{
}

// Received packed NGAP PDU that needs to be unpacked and forwarded.
void ngc_asn1_packer::handle_packed_pdu(const byte_buffer& bytes)
{
  asn1::cbit_ref         bref(bytes);
  srs_cu_cp::ngc_message msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Couldn't unpack NGAP PDU");
    return;
  }

  // call packet handler
  ngc.handle_message(msg);
}

// Receive populated ASN1 struct that needs to be packed and forwarded.
void ngc_asn1_packer::handle_message(const srs_cu_cp::ngc_message& msg)
{
  // pack PDU into temporary buffer
  byte_buffer   tx_pdu;
  asn1::bit_ref bref(tx_pdu);
  if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack NGAP PDU");
    return;
  }

  gw.handle_pdu(tx_pdu);
}

} // namespace srsgnb
