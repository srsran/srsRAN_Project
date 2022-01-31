
#ifndef SRSGNB_F1AP_UE_CREATE_PROCEDURE_H
#define SRSGNB_F1AP_UE_CREATE_PROCEDURE_H

#include "f1ap_du_context.h"
#include "srsgnb/mac/mac.h"
#include <memory>

namespace srsgnb {

class initial_ul_rrc_message_procedure
{
public:
  initial_ul_rrc_message_procedure(f1ap_du_context& f1ap_ctx, ul_ccch_indication_message ccch_msg_) :
    ccch_msg(std::move(ccch_msg_))
  {
    // 1. Create gNB-DU F1AP UE ID context
    f1ap_ctx.ue_ctxt_manager.emplace_back();
    f1ap_du_ue_context& ue_ctxt = f1ap_ctx.ue_ctxt_manager.back();
    ue_ctxt.gnb_du_f1ap_ue_id   = f1ap_ctx.ue_ctxt_manager.size() - 1;
    ue_ctxt.crnti               = ccch_msg.crnti;

    ue_index = ue_ctxt.gnb_du_f1ap_ue_id;
    srslog::fetch_basic_logger("F1AP").info(
        "UL\t0x%x\tCCCH indication. Creating UE ID=%d", ccch_msg.crnti, ue_ctxt.gnb_du_f1ap_ue_id);

    // 2. Initiate UE creation in DU manager
    du_ue_create_message msg{};
    msg.cell_index = msg.cell_index;
    msg.ue_index   = ue_index;
    msg.crnti      = ccch_msg.crnti;
    f1ap_ctx.du_manager->ue_create(msg);
  }

  void du_ue_create_response(const du_ue_create_response_message& resp)
  {
    // Handle result

    // Communicate with CU
    send_initial_ul_rrc_message_transfer();
  }

private:
  void send_initial_ul_rrc_message_transfer()
  {
    // fill ASN.1 message

    srslog::fetch_basic_logger("F1AP").info("UL\tUE ID=%d. Initial UL RRC message to CU", ue_index);

    // send message via socket
    //    gateway.push_pdu(std::move(ccch_msg));
  }

  du_ue_index_t              ue_index;
  ul_ccch_indication_message ccch_msg;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_UE_CREATE_PROCEDURE_H
