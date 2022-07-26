/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_ue_context_release_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::f1ap;

f1ap_ue_context_release_procedure::f1ap_ue_context_release_procedure(f1ap_ue_context&                ue_ctxt_,
                                                                     const ue_context_release_cmd_s& command_,
                                                                     f1c_message_notifier&           f1c_notif_,
                                                                     f1ap_event_manager&             ev_mng_,
                                                                     srslog::basic_logger&           logger_) :
  ue_ctxt(ue_ctxt_), command(command_), f1c_notifier(f1c_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void f1ap_ue_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Send command to DU.
  send_f1_ue_context_release_command();

  // Await CU response.
  CORO_AWAIT_VALUE(f1_ue_ctxt_rel_outcome, ev_mng.f1ap_ue_context_release_complete);

  // Handle response from DU
  handle_f1ap_ue_context_release_complete(*f1_ue_ctxt_rel_outcome);

  CORO_RETURN();
}

void f1ap_ue_context_release_procedure::send_f1_ue_context_release_command()
{
  // Pack message into PDU
  f1c_msg f1c_ue_ctxt_rel_msg;
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

void f1ap_ue_context_release_procedure::handle_f1ap_ue_context_release_complete(
    const asn1::f1ap::ue_context_release_complete_s msg)
{
  logger.info("Received F1AP UE Context Release Complete.");

  if (msg->gnb_du_ue_f1_ap_id.value == ue_ctxt.du_ue_f1ap_id) {
    ue_ctxt.du_ue_f1ap_id = INVALID_F1AP_UE_ID;
    ue_ctxt.ue_index      = INVALID_UE_INDEX;
  }

  return;
}