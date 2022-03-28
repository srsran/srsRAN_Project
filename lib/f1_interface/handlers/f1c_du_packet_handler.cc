
#include "f1c_du_packet_handler.h"

namespace srsgnb {

f1c_du_packet_handler::f1c_du_packet_handler(std::unique_ptr<f1ap_packet_procedures> procedure_) :
  logger(srslog::fetch_basic_logger("F1C-DU"))
{}

void f1c_du_packet_handler::handle(const asn1::f1ap::f1_ap_pdu_c& pdu)
{
  logger.info("Received F1AP PDU type={}", pdu.type().to_string());

  // procedure->handle_ul_data_packet(pdu);
}

} // namespace srsgnb
