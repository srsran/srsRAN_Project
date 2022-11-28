/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_asn1_packer.h"
#include "srsgnb/asn1/e1ap/e1ap.h"

namespace srsgnb {

e1ap_asn1_packer::e1ap_asn1_packer(network_gateway_data_handler& gw_, e1_message_handler& e1_handler) :
  logger(srslog::fetch_basic_logger("E1-ASN1-PCK")), gw(gw_), e1(e1_handler)
{
}

// Received packed E1AP PDU that needs to be unpacked and forwarded.
void e1ap_asn1_packer::handle_packed_pdu(const byte_buffer& bytes)
{
  logger.info("Received PDU of {} bytes", bytes.length());

  asn1::cbit_ref bref(bytes);
  e1_message     msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Couldn't unpack E1 PDU");
    return;
  }

  // call packet handler
  e1.handle_message(msg);
}

// Receive populated ASN1 struct that needs to be packed and forwarded.
void e1ap_asn1_packer::handle_message(const e1_message& msg)
{
  // pack PDU into temporary buffer
  byte_buffer   tx_pdu;
  asn1::bit_ref bref(tx_pdu);
  if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack E1AP PDU");
    return;
  }

  gw.handle_pdu(tx_pdu);
}

} // namespace srsgnb
