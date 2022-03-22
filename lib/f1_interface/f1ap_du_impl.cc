
#include "f1ap_du_impl.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

f1ap_du_impl::f1ap_du_impl(f1ap_du_pdu_notifier& pdu_listener) : logger(srslog::fetch_basic_logger("F1AP")) {}

void f1ap_du_impl::f1ap_du_setup_request(const du_setup_params& params)
{
  logger.info("{}", __FUNCTION__);
  /// TODO: add impl
}

void f1ap_du_impl::f1ap_du_setup_response(const asn1::f1ap::f1_setup_resp_s& resp)
{
  logger.info("{}", __FUNCTION__);
  /// TODO: add impl
}

void f1ap_du_impl::f1ap_du_setup_failure(const asn1::f1ap::f1_setup_fail_s& fail)
{
  logger.info("{}", __FUNCTION__);
  /// TODO: add impl
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
