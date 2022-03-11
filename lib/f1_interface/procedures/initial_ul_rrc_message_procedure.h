
#ifndef SRSGNB_F1AP_UE_CREATE_PROCEDURE_H
#define SRSGNB_F1AP_UE_CREATE_PROCEDURE_H

#include "../../ran/gnb_format.h"
#include "f1ap_du_context.h"
#include "srsgnb/mac/mac.h"
#include <memory>

namespace srsgnb {

class initial_ul_rrc_message_procedure
{
public:
  initial_ul_rrc_message_procedure(f1ap_du_context& f1ap_ctx_, ul_ccch_indication_message ccch_msg_) :
    f1ap_ctx(f1ap_ctx_), ccch_msg(std::move(ccch_msg_))
  {
    // 1. Create gNB-DU F1AP UE ID context
    f1ap_ctx.ue_ctxt_manager.emplace_back();
    f1ap_du_ue_context& ue_ctxt = f1ap_ctx.ue_ctxt_manager.back();
    ue_ctxt.gnb_du_f1ap_ue_id   = f1ap_ctx.ue_ctxt_manager.size() - 1;
    ue_ctxt.crnti               = ccch_msg.crnti;

    ue_index = ue_ctxt.gnb_du_f1ap_ue_id;
    log_ul_pdu(srslog::fetch_basic_logger("F1AP"),
               ccch_msg.crnti,
               ccch_msg.cell_index,
               "CCCH Indication. Creating UE ID={}",
               ue_index);

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
    if (not resp.result) {
      logger.warning("Failed to create ueId={} context in DU", resp.ue_index);
      f1ap_ctx.ue_ctxt_manager.pop_back();
      return;
    }

    // Communicate with CU
    send_initial_ul_rrc_message_transfer();
  }

private:
  static const char* name() { return "Initial UL RRC Message"; };

  void send_initial_ul_rrc_message_transfer()
  {
    // fill ASN.1 message

    log_proc_event(logger, ue_index, name(), "Message sent to the CU");

    // send message via socket
    //    gateway.push_pdu(std::move(ccch_msg));
  }

  f1ap_du_context&      f1ap_ctx;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("F1AP");

  du_ue_index_t              ue_index;
  ul_ccch_indication_message ccch_msg;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_UE_CREATE_PROCEDURE_H
