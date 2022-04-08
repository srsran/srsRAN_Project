
#ifndef SRSGNB_F1AP_DU_IMPL_H
#define SRSGNB_F1AP_DU_IMPL_H

#include "f1ap_du_context.h"
#include "handlers/f1c_du_packet_handler.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include <memory>

namespace srsgnb {

class f1ap_du_impl final : public f1_du_interface
{
public:
  f1ap_du_impl(f1c_pdu_handler& f1c_pdu_handler_);

  lazy_task<du_setup_result> f1ap_du_setup_request(const du_setup_params& params) override;

  lazy_task<f1ap_du_ue_create_response> handle_ue_creation_request(const f1ap_du_ue_create_request& msg) override;

  void ul_rrc_message_delivery_report(const ul_rrc_message_delivery_status& report) override {}

  void handle_pdu(f1_rx_pdu pdu) override;

  void handle_unpacked_pdu(const asn1::f1ap::f1_ap_pdu_c& pdu) override;

  void handle_connection_loss() override {}

private:
  srslog::basic_logger& logger;
  f1c_pdu_handler&      f1c;

  f1ap_du_context ctxt;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_DU_IMPL_H
