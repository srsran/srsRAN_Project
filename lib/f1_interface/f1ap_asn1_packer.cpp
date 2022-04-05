#include "f1ap_asn1_packer.h"
#include "srsgnb/asn1/f1ap.h"

namespace srsgnb {

f1ap_asn1_packer::f1ap_asn1_packer(network_gateway_data_handler& gw_, f1c_pdu_handler& f1c_) :
  logger(srslog::fetch_basic_logger("F1C-ASN1-PCK")), gw(gw_), f1c(f1c_)
{}

// Received packed F1AP PDU that needs to be unpacked and forwarded.
void f1ap_asn1_packer::handle_packed_pdu(const byte_buffer& bytes)
{
  logger.info("Received PDU of {} bytes", bytes.length());

  asn1::cbit_ref          bref(bytes);
  asn1::f1ap::f1_ap_pdu_c pdu;
  if (pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Couldn't unpack F1C PDU");
    return;
  }

  // call packet handler
  f1c.handle_unpacked_pdu(pdu);
}

// Receive populated ASN1 struct that needs to be packed and forwarded.
void f1ap_asn1_packer::handle_unpacked_pdu(const asn1::f1ap::f1_ap_pdu_c& pdu)
{
  // pack PDU into temporary buffer
  byte_buffer   tx_pdu;
  asn1::bit_ref bref(tx_pdu);
  if (pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack F1AP PDU");
    return;
  }

  gw.handle_pdu(tx_pdu);
}

} // namespace srsgnb
