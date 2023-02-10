/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2ap_asn1_packer.h"
#include "srsgnb/asn1/e2ap/e2ap.h"

using namespace srsgnb;

e2ap_asn1_packer::e2ap_asn1_packer(sctp_network_gateway_data_handler& gw_, e2_message_handler& e2_handler) :
  logger(srslog::fetch_basic_logger("E2-ASN1-PCK")), gw(gw_), e2(e2_handler)
{
}

void e2ap_asn1_packer::handle_packed_pdu(const byte_buffer& bytes)
{
  logger.info("Received PDU of {} bytes", bytes.length());

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

  gw.handle_pdu(tx_pdu);
}
