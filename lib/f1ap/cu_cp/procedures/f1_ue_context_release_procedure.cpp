/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_ue_context_release_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::f1ap;

f1_ue_context_release_procedure::f1_ue_context_release_procedure(f1ap_ue_context_list&                  ue_ctx_list_,
                                                                 const f1ap_ue_context_release_command& cmd_,
                                                                 f1c_message_notifier&                  f1c_notif_,
                                                                 f1c_event_manager&                     ev_mng_,
                                                                 srslog::basic_logger&                  logger_) :
  ue_ctxt_list(ue_ctx_list_),
  ue_ctxt(ue_ctxt_list[cmd_.ue_index]),
  f1c_notifier(f1c_notif_),
  ev_mng(ev_mng_),
  logger(logger_)
{
  command->gnb_cu_ue_f1_ap_id.value = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.cu_ue_f1ap_id);
  command->gnb_du_ue_f1_ap_id.value = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.du_ue_f1ap_id);
  command->cause.value              = cmd_.cause;
}

void f1_ue_context_release_procedure::operator()(coro_context<async_task<ue_index_t>>& ctx)
{
  CORO_BEGIN(ctx);

  // Send command to DU.
  send_ue_context_release_command();

  // Await CU response.
  CORO_AWAIT_VALUE(f1_ue_ctxt_rel_outcome, ev_mng.f1ap_ue_context_release_complete);

  // Handle response from DU and return UE index
  CORO_RETURN(create_ue_context_release_complete(*f1_ue_ctxt_rel_outcome));
}

void f1_ue_context_release_procedure::send_ue_context_release_command()
{
  // Pack message into PDU
  f1c_message f1c_ue_ctxt_rel_msg;
  f1c_ue_ctxt_rel_msg.pdu.set_init_msg();
  f1c_ue_ctxt_rel_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  f1c_ue_ctxt_rel_msg.pdu.init_msg().value.ue_context_release_cmd() = command;

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    f1c_ue_ctxt_rel_msg.pdu.to_json(js);
    logger.debug("Containerized UE Context Release Command message: {}", js.to_string());
  }

  // send DL RRC message
  f1c_notifier.on_new_message(f1c_ue_ctxt_rel_msg);
}

ue_index_t f1_ue_context_release_procedure::create_ue_context_release_complete(
    const asn1::f1ap::ue_context_release_complete_s& msg)
{
  logger.info("Received F1AP UE Context Release Complete.");

  ue_index_t ret = INVALID_UE_INDEX;

  if (msg->gnb_du_ue_f1_ap_id.value == gnb_du_ue_f1ap_id_to_uint(ue_ctxt.du_ue_f1ap_id)) {
    ret = ue_ctxt.ue_index;
    ue_ctxt_list.remove_ue(ue_ctxt.cu_ue_f1ap_id);
  }

  return ret;
}