/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
    e1ap_cu_up_manager_notifier&                    cu_up_notifier_) :
  ue_ctxt(ue_ctxt_), request(request_), pdu_notifier(pdu_notifier_), cu_up_notifier(cu_up_notifier_)
{
  prepare_failure_message();
}

void bearer_context_modification_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  bearer_context_mod.ue_index = ue_ctxt.ue_ids.ue_index;

  if (not validate_request()) {
    pdu_notifier.on_new_message(e1ap_msg);
    CORO_EARLY_RETURN();
  }

  fill_e1ap_bearer_context_modification_request(bearer_context_mod, request);

  // Here, we transfer to the UE control executor to safely delete PDU sessions,
  // change keys, perform PDCP retransmissions, etc.
  CORO_AWAIT_VALUE(bearer_context_mod_response_msg,
                   cu_up_notifier.on_bearer_context_modification_request_received(bearer_context_mod));

  // Could not find UE
  if (bearer_context_mod_response_msg.ue_index == INVALID_UE_INDEX) {
    ue_ctxt.logger.log_error("Sending BearerContextModificationFailure: Cause: Invalid UE index");
    pdu_notifier.on_new_message(e1ap_msg);
    CORO_EARLY_RETURN();
  }

  // PDU sessions failed to setup
  if (not bearer_context_mod_response_msg.success) {
    e1ap_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->cause =
        cause_to_asn1(bearer_context_mod_response_msg.cause.value());
    ue_ctxt.logger.log_warning("Sending BearerContextModificationFailure");
    pdu_notifier.on_new_message(e1ap_msg);
    CORO_EARLY_RETURN();
  }

  // Bearer modification successful
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
  pdu_notifier.on_new_message(e1ap_msg);
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

bool bearer_context_modification_procedure::validate_request()
{
  // We only support NG-RAN bearers
  if (request->sys_bearer_context_mod_request_present &&
      request->sys_bearer_context_mod_request.type() !=
          asn1::e1ap::sys_bearer_context_mod_request_c::types::ng_ran_bearer_context_mod_request) {
    ue_ctxt.logger.log_error("Sending BearerContextModificationFailure. Cause: Not handling E-UTRAN Bearers");
    return false;
  }
  return true;
}
