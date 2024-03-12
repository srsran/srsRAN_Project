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

#include "ngap_pdu_session_resource_setup_procedure.h"
#include "../ngap/ngap_asn1_helpers.h"
#include "ngap_procedure_helpers.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_pdu_session_resource_setup_procedure::ngap_pdu_session_resource_setup_procedure(
    const cu_cp_pdu_session_resource_setup_request&    request_,
    const asn1::ngap::pdu_session_res_setup_request_s& asn1_request_,
    const ngap_ue_ids&                                 ue_ids_,
    ngap_rrc_ue_pdu_notifier&                          rrc_ue_pdu_notifier_,
    ngap_du_processor_control_notifier&                du_processor_ctrl_notifier_,
    ngap_message_notifier&                             amf_notif_,
    ngap_control_message_handler&                      ngap_ctrl_handler_,
    ngap_ue_logger&                                    logger_) :
  request(request_),
  asn1_request(asn1_request_),
  ue_ids(ue_ids_),
  rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_),
  du_processor_ctrl_notifier(du_processor_ctrl_notifier_),
  amf_notifier(amf_notif_),
  ngap_ctrl_handler(ngap_ctrl_handler_),
  logger(logger_)
{
}

void ngap_pdu_session_resource_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());

  // Verify PDU Session Resource Setup Request
  verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, logger);

  if (verification_outcome.request.pdu_session_res_setup_items.empty()) {
    logger.log_info("Validation of PduSessionResourceSetupRequest failed");
    response = verification_outcome.response;
  } else {
    // Handle mandatory IEs
    CORO_AWAIT_VALUE(
        response, du_processor_ctrl_notifier.on_new_pdu_session_resource_setup_request(verification_outcome.request));

    // TODO: Handle optional IEs
    if (asn1_request->nas_pdu_present) {
      handle_nas_pdu(logger, asn1_request->nas_pdu.copy(), rrc_ue_pdu_notifier);
    }

    // Combine validation response with DU processor response
    combine_pdu_session_resource_setup_response();
  }

  if (!response.pdu_session_res_failed_to_setup_items.empty()) {
    logger.log_warning("Some or all PduSessionResourceSetupItems failed to setup");
  }

  send_pdu_session_resource_setup_response();

  // Request UE release in case of a failure to cleanup CU-CP
  if (!response.pdu_session_res_failed_to_setup_items.empty()) {
    ue_context_release_request = {
        ue_ids.ue_index, {}, ngap_cause_radio_network_t::release_due_to_ngran_generated_reason};
    CORO_AWAIT(ngap_ctrl_handler.handle_ue_context_release_request(ue_context_release_request));
  }

  logger.log_debug("\"{}\" finalized", name());

  CORO_RETURN();
}

void ngap_pdu_session_resource_setup_procedure::combine_pdu_session_resource_setup_response()
{
  for (const auto& setup_item : verification_outcome.response.pdu_session_res_setup_response_items) {
    response.pdu_session_res_setup_response_items.emplace(setup_item.pdu_session_id, setup_item);
  }
  for (const auto& failed_item : verification_outcome.response.pdu_session_res_failed_to_setup_items) {
    response.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
  }
  if (!response.crit_diagnostics.has_value()) {
    response.crit_diagnostics = verification_outcome.response.crit_diagnostics;
  }
}

void ngap_pdu_session_resource_setup_procedure::send_pdu_session_resource_setup_response()
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_SETUP);

  fill_asn1_pdu_session_res_setup_response(ngap_msg.pdu.successful_outcome().value.pdu_session_res_setup_resp(),
                                           response);

  auto& pdu_session_res_setup_resp           = ngap_msg.pdu.successful_outcome().value.pdu_session_res_setup_resp();
  pdu_session_res_setup_resp->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ids.amf_ue_id);
  pdu_session_res_setup_resp->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ids.ran_ue_id);

  logger.log_info("Sending PduSessionResourceSetupResponse");
  amf_notifier.on_new_message(ngap_msg);
}
