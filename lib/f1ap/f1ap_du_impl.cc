
#include "f1ap_du_impl.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

f1ap_du_impl::f1ap_du_impl(f1ap_du_pdu_notifier& pdu_listener, du_manager_interface_f1ap& du_manager) :
  logger(srslog::fetch_basic_logger("F1AP"))
{
  ctxt.du_manager = &du_manager;
}

void f1ap_du_impl::ul_ccch_message_indication(const ul_ccch_indication_message& msg)
{
  init_ul_rrc_msg_proc = std::make_unique<initial_ul_rrc_message_procedure>(ctxt, msg);
}

void f1ap_du_impl::ue_creation_response(const du_ue_create_response_message& resp)
{
  init_ul_rrc_msg_proc->du_ue_create_response(resp);
}

void f1ap_du_impl::ul_rrc_message_transfer(const ul_rrc_transfer_message& msg)
{
  logger.info("UL   ueId={} lcid={}: \"RRC Message Transfer\"", msg.ue_index, msg.lcid);
}

} // namespace srsgnb
