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

  std::vector<uint8_t> unpack_buffer;
  unpack_buffer.resize(128);

  asn1::cbit_ref          bref(unpack_buffer.data(), unpack_buffer.size());
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
  std::array<uint8_t, 128> pack_buffer;
  asn1::bit_ref            bref(pack_buffer.data(), pack_buffer.size());
  if (pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack F1AP PDU");
    return;
  }

  // convert into byte buffer and send
  byte_buffer tx_pdu(pack_buffer.begin(), pack_buffer.begin() + bref.distance_bytes());
  gw.handle_pdu(tx_pdu);
}

} // namespace srsgnb
