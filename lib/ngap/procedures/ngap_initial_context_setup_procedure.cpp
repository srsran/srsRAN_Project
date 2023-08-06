/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ngap_initial_context_setup_procedure.h"
#include "../ngap_asn1_helpers.h"
#include "ngap_procedure_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_initial_context_setup_procedure::ngap_initial_context_setup_procedure(
    ngap_context_t&                                 context_,
    const ue_index_t                                ue_index_,
    const asn1::ngap::init_context_setup_request_s& request_,
    ngap_ue_manager&                                ue_manager_,
    ngap_message_notifier&                          amf_notif_,
    srslog::basic_logger&                           logger_) :
  context(context_),
  ue_index(ue_index_),
  request(request_),
  ue_manager(ue_manager_),
  amf_notifier(amf_notif_),
  logger(logger_)
{
  ue = ue_manager.find_ngap_ue(ue_index);
  srsran_assert(ue != nullptr, "ue={} Couldn't find UE", ue_index);
}

void ngap_initial_context_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={} Initial Context Setup Procedure started", ue_index);

  // Handle mandatory IEs
  CORO_AWAIT_VALUE(
      success,
      ue->get_rrc_ue_control_notifier().on_new_security_context(request->ue_security_cap, request->security_key));

  if (not success) {
    ngap_initial_context_failure_message fail_msg = {};
    fail_msg.cause.set_protocol();

    // Add failed PDU Sessions
    if (request->pdu_session_res_setup_list_cxt_req_present) {
      for (const auto& pdu_session_item : request->pdu_session_res_setup_list_cxt_req) {
        cu_cp_pdu_session_res_setup_failed_item failed_item;
        failed_item.pdu_session_id = uint_to_pdu_session_id(pdu_session_item.pdu_session_id);
        failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause = cause_t::radio_network;

        fail_msg.pdu_session_res_failed_to_setup_items.emplace(uint_to_pdu_session_id(pdu_session_item.pdu_session_id),
                                                               failed_item);
      }
    }

    send_initial_context_setup_failure(fail_msg, ue->get_amf_ue_id(), ue->get_ran_ue_id());

    // Release UE
    cu_cp_ngap_ue_context_release_command rel_cmd = {};
    rel_cmd.ue_index                              = ue->get_ue_index();
    rel_cmd.cause                                 = cause_t::protocol;

    ue->get_du_processor_control_notifier().on_new_ue_context_release_command(rel_cmd);

    // Remove UE
    ue_manager.remove_ngap_ue(ue_index);

    logger.debug("ue={} Initial Context Setup Procedure failed", ue_index);
    CORO_EARLY_RETURN();
  }

  // Handle GUAMI
  context.current_guami = asn1_to_guami(request->guami);

  // Handle optional IEs

  // Handle PDU Session Resource Setup List Context Request
  if (request->pdu_session_res_setup_list_cxt_req_present) {
    // Handle UE Aggregate Maximum Bitrate
    if (request->ue_aggr_max_bit_rate_present) {
      ue->set_aggregate_maximum_bit_rate_dl(request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl);
    }

    // Convert to common type
    pdu_session_setup_request.ue_index     = ue_index;
    pdu_session_setup_request.serving_plmn = request->guami.plmn_id.to_string();
    if (!fill_cu_cp_pdu_session_resource_setup_request(pdu_session_setup_request,
                                                       request->pdu_session_res_setup_list_cxt_req)) {
      logger.error("ue={} Conversion of PDU Session Resource Setup Request failed.", ue_index);
      CORO_EARLY_RETURN();
    }
    pdu_session_setup_request.ue_aggregate_maximum_bit_rate_dl = ue->get_aggregate_maximum_bit_rate_dl();

    // Handle mandatory IEs
    CORO_AWAIT_VALUE(
        pdu_session_response,
        ue->get_du_processor_control_notifier().on_new_pdu_session_resource_setup_request(pdu_session_setup_request));

    // Handle NAS PDUs
    for (const auto& session : request->pdu_session_res_setup_list_cxt_req) {
      if (!session.nas_pdu.empty()) {
        handle_nas_pdu(logger, session.nas_pdu, *ue);
      }
    }
  }

  if (request->nas_pdu_present) {
    handle_nas_pdu(logger, request->nas_pdu, *ue);
  }

  ngap_initial_context_response_message resp_msg = {};
  resp_msg.pdu_session_res_setup_response_items  = pdu_session_response.pdu_session_res_setup_response_items;
  resp_msg.pdu_session_res_failed_to_setup_items = pdu_session_response.pdu_session_res_failed_to_setup_items;

  send_initial_context_setup_response(resp_msg, ue->get_amf_ue_id(), ue->get_ran_ue_id());

  logger.debug("ue={} Initial Context Setup Procedure finished", ue_index);
  CORO_RETURN();
}

void ngap_initial_context_setup_procedure::send_initial_context_setup_response(
    const ngap_initial_context_response_message& msg,
    const amf_ue_id_t&                           amf_ue_id,
    const ran_ue_id_t&                           ran_ue_id)
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
  auto& init_ctxt_setup_resp           = ngap_msg.pdu.successful_outcome().value.init_context_setup_resp();
  init_ctxt_setup_resp->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  init_ctxt_setup_resp->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  fill_asn1_initial_context_setup_response(init_ctxt_setup_resp, msg);

  logger.info("ue={} Sending InitialContextSetupResponse", ue_index);
  amf_notifier.on_new_message(ngap_msg);
}

void ngap_initial_context_setup_procedure::send_initial_context_setup_failure(
    const ngap_initial_context_failure_message& msg,
    const amf_ue_id_t&                          amf_ue_id,
    const ran_ue_id_t&                          ran_ue_id)
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_unsuccessful_outcome();
  ngap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
  auto& init_ctxt_setup_fail           = ngap_msg.pdu.unsuccessful_outcome().value.init_context_setup_fail();
  init_ctxt_setup_fail->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  init_ctxt_setup_fail->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  // Fill PDU Session Resource Failed to Setup List
  fill_asn1_initial_context_setup_failure(init_ctxt_setup_fail, msg);

  logger.info("ue={} Sending InitialContextSetupFailure", ue_index);
  amf_notifier.on_new_message(ngap_msg);
}
