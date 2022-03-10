
#include "f1ap_packet_handler.h"

namespace srsgnb {

void f1ap_du_packet_handler::handle(const byte_buffer& bytes)
{
  std::printf(
      "[F1AP-PKT-HANDLER] F1AP entry point has received a new control packet of size = %u bytes, passing to the "
      "UL procedure\n",
      (unsigned)bytes.size());

  asn1::f1ap::f1_ap_pdu_c pdu;
  {
    asn1::cbit_ref bref(bytes.data(), bytes.size());
    pdu.unpack(bref);
  }

  procedure->handle_ul_data_packet(pdu);
}

} // namespace srsgnb
