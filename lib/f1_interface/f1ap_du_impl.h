
#ifndef SRSGNB_F1AP_DU_IMPL_H
#define SRSGNB_F1AP_DU_IMPL_H

#include "f1ap_packet_handler.h"
#include "procedures/initial_ul_rrc_message_procedure.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include <memory>

namespace srsgnb {

class f1ap_du_impl final : public f1ap_du_interface
{
public:
  f1ap_du_impl(f1ap_du_pdu_notifier& pdu_listener, du_manager_interface_f1ap& du_manager);

  void ul_ccch_message_indication(const ul_ccch_indication_message& msg) override;
  void ue_creation_response(const du_ue_create_response_message& resp) override;

  void ul_rrc_message_transfer(const ul_rrc_transfer_message& msg) override;
  void ul_rrc_message_delivery_report(const ul_rrc_message_delivery_status& report) override {}

  void push_sdu(const byte_buffer& sdu) override {}

private:
  srslog::basic_logger& logger;

  f1ap_du_context ctxt;

  //  std::unique_ptr<f1ap_du_packet_handler> packet_handler;
  std::unique_ptr<initial_ul_rrc_message_procedure> init_ul_rrc_msg_proc;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_DU_IMPL_H
