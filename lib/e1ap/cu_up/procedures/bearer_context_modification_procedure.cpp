/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "bearer_context_modification_procedure.h"
#include "common/e1ap_asn1_converters.h"
#include "cu_up/e1ap_cu_up_asn1_helpers.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"

using namespace srsran;
using namespace srsran::srs_cu_up;

bearer_context_modification_procedure::bearer_context_modification_procedure(
    const e1ap_ue_context&                          ue_ctxt_,
    const asn1::e1ap::bearer_context_mod_request_s& request_,
    e1ap_message_notifier&                          pdu_notifier_,
    e1ap_cu_up_notifier&                            cu_up_notifier_) :
  ue_ctxt(ue_ctxt_), request(request_), pdu_notifier(pdu_notifier_), cu_up_notifier(cu_up_notifier_)
{
  prepare_failure_message();
}

void bearer_context_modification_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  fmt::print("CU-UP bearer context modification\n");

  bearer_context_mod.ue_index = ue_ctxt.ue_ids.ue_index;

  // security info
  if (request->security_info_present) {
    bearer_context_mod.security_info.emplace();
    bearer_context_mod.security_info->security_algorithm.ciphering_algo =
        e1ap_asn1_to_ciphering_algorithm(request->security_info.security_algorithm.ciphering_algorithm);

    if (request->security_info.security_algorithm.integrity_protection_algorithm_present) {
      bearer_context_mod.security_info->security_algorithm.integrity_protection_algorithm =
          e1ap_asn1_to_integrity_algorithm(request->security_info.security_algorithm.integrity_protection_algorithm);
    }
    bearer_context_mod.security_info->up_security_key.encryption_key =
        request->security_info.up_securitykey.encryption_key.copy();
    bearer_context_mod.security_info->up_security_key.integrity_protection_key =
        request->security_info.up_securitykey.integrity_protection_key.copy();
  }

  // sys bearer context mod request
  if (request->sys_bearer_context_mod_request_present) {
    // We only support NG-RAN Bearer
    if (request->sys_bearer_context_mod_request.type() !=
        asn1::e1ap::sys_bearer_context_mod_request_c::types::ng_ran_bearer_context_mod_request) {
      ue_ctxt.logger.log_error("Sending BearerContextModificationFailure. Cause: Not handling E-UTRAN Bearers");

      // send response
      pdu_notifier.on_new_message(e1ap_msg);
      return;
    }

    fill_e1ap_bearer_context_modification_request(bearer_context_mod, request);
  }

  CORO_AWAIT_VALUE(bearer_context_mod_response_msg,
                   cu_up_notifier.on_bearer_context_modification_request_received(bearer_context_mod));
  fmt::print("CU-UP bearer context modified\n");

  if (bearer_context_mod_response_msg.ue_index == INVALID_UE_INDEX) {
    ue_ctxt.logger.log_debug("Sending BearerContextModificationFailure: Cause: Invalid UE index");

    // send response
    pdu_notifier.on_new_message(e1ap_msg);
    return;
  }

  if (bearer_context_mod_response_msg.success) {
    e1ap_msg.pdu.set_successful_outcome();
    e1ap_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);
    e1ap_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->gnb_cu_cp_ue_e1ap_id =
        request->gnb_cu_cp_ue_e1ap_id;
    e1ap_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->gnb_cu_up_ue_e1ap_id =
        request->gnb_cu_up_ue_e1ap_id;
    e1ap_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->sys_bearer_context_mod_resp_present = true;
    fill_asn1_bearer_context_modification_response(
        e1ap_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->sys_bearer_context_mod_resp,
        bearer_context_mod_response_msg);
    ue_ctxt.logger.log_debug("Sending BearerContextModificationResponse");
    // send response
    pdu_notifier.on_new_message(e1ap_msg);
  } else {
    e1ap_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->cause =
        cause_to_asn1(bearer_context_mod_response_msg.cause.value());

    // send response
    ue_ctxt.logger.log_debug("Sending BearerContextModificationFailure");
    pdu_notifier.on_new_message(e1ap_msg);
  }
  CORO_RETURN();
}

void bearer_context_modification_procedure::prepare_failure_message()
{
  e1ap_msg.pdu.set_unsuccessful_outcome();
  e1ap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);
  e1ap_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->gnb_cu_cp_ue_e1ap_id =
      request->gnb_cu_cp_ue_e1ap_id;
  e1ap_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->gnb_cu_up_ue_e1ap_id =
      request->gnb_cu_up_ue_e1ap_id;
  e1ap_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->cause.set_protocol();
}
