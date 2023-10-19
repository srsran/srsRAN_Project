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
#include "srsran/ngap/ngap.h"
#include "srsran/ran/cause.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_initial_context_setup_procedure::ngap_initial_context_setup_procedure(
    const ngap_init_context_setup_request& request_,
    const ngap_ue_context&                 ue_ctxt_,
    ngap_rrc_ue_control_notifier&          rrc_ue_ctrl_notifier_,
    ngap_rrc_ue_pdu_notifier&              rrc_ue_pdu_notifier_,
    ngap_du_processor_control_notifier&    du_processor_ctrl_notifier_,
    ngap_message_notifier&                 amf_notifier_,
    srslog::basic_logger&                  logger_) :
  request(request_),
  ue_ctxt(ue_ctxt_),
  rrc_ue_ctrl_notifier(rrc_ue_ctrl_notifier_),
  rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_),
  du_processor_ctrl_notifier(du_processor_ctrl_notifier_),
  amf_notifier(amf_notifier_),
  logger(logger_)
{
}

void ngap_initial_context_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={} ran_ue_id={} amf_ue_id={}: \"{}\" initialized",
               ue_ctxt.ue_ids.ue_index,
               ue_ctxt.ue_ids.ran_ue_id,
               ue_ctxt.ue_ids.amf_ue_id,
               name());

  // Handle mandatory IEs
  CORO_AWAIT_VALUE(success, rrc_ue_ctrl_notifier.on_new_security_context(request.security_context));

  if (not success) {
    fail_msg.cause = cause_protocol_t::unspecified;

    // Add failed PDU Sessions
    if (request.pdu_session_res_setup_list_cxt_req.has_value()) {
      for (const auto& pdu_session_item :
           request.pdu_session_res_setup_list_cxt_req.value().pdu_session_res_setup_items) {
        cu_cp_pdu_session_res_setup_failed_item failed_item;
        failed_item.pdu_session_id              = pdu_session_item.pdu_session_id;
        failed_item.unsuccessful_transfer.cause = cause_radio_network_t::unspecified;

        fail_msg.pdu_session_res_failed_to_setup_items.emplace(pdu_session_item.pdu_session_id, failed_item);
      }
    }

    send_initial_context_setup_failure(fail_msg, ue_ctxt.ue_ids.amf_ue_id, ue_ctxt.ue_ids.ran_ue_id);

    logger.debug("ue={} ran_ue_id={} amf_ue_id={}: \"{}\" failed",
                 ue_ctxt.ue_ids.ue_index,
                 ue_ctxt.ue_ids.ran_ue_id,
                 ue_ctxt.ue_ids.amf_ue_id,
                 name());

    CORO_EARLY_RETURN();
  }

  // Handle optional IEs

  // Handle PDU Session Resource Setup List Context Request
  if (request.pdu_session_res_setup_list_cxt_req.has_value()) {
    request.pdu_session_res_setup_list_cxt_req.value().ue_index     = ue_ctxt.ue_ids.ue_index;
    request.pdu_session_res_setup_list_cxt_req.value().serving_plmn = request.guami.plmn;
    request.pdu_session_res_setup_list_cxt_req.value().ue_aggregate_maximum_bit_rate_dl =
        ue_ctxt.aggregate_maximum_bit_rate_dl;

    // Handle mandatory IEs
    CORO_AWAIT_VALUE(pdu_session_response,
                     du_processor_ctrl_notifier.on_new_pdu_session_resource_setup_request(
                         request.pdu_session_res_setup_list_cxt_req.value()));

    // Handle NAS PDUs
    for (auto& session : request.pdu_session_res_setup_list_cxt_req.value().pdu_session_res_setup_items) {
      if (!session.pdu_session_nas_pdu.empty()) {
        handle_nas_pdu(logger, std::move(session.pdu_session_nas_pdu), rrc_ue_pdu_notifier);
      }
    }
  }

  if (request.nas_pdu.has_value()) {
    handle_nas_pdu(logger, std::move(request.nas_pdu.value()), rrc_ue_pdu_notifier);
  }

  resp_msg.pdu_session_res_setup_response_items  = pdu_session_response.pdu_session_res_setup_response_items;
  resp_msg.pdu_session_res_failed_to_setup_items = pdu_session_response.pdu_session_res_failed_to_setup_items;

  send_initial_context_setup_response(resp_msg, ue_ctxt.ue_ids.amf_ue_id, ue_ctxt.ue_ids.ran_ue_id);

  logger.debug("ue={} ran_ue_id={} amf_ue_id={}: \"{}\" finalized",
               ue_ctxt.ue_ids.ue_index,
               ue_ctxt.ue_ids.ran_ue_id,
               ue_ctxt.ue_ids.amf_ue_id,
               name());
  CORO_RETURN();
}

void ngap_initial_context_setup_procedure::send_initial_context_setup_response(
    const ngap_init_context_setup_response& msg,
    const amf_ue_id_t&                      amf_ue_id,
    const ran_ue_id_t&                      ran_ue_id)
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
  auto& init_ctxt_setup_resp           = ngap_msg.pdu.successful_outcome().value.init_context_setup_resp();
  init_ctxt_setup_resp->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  init_ctxt_setup_resp->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  fill_asn1_initial_context_setup_response(init_ctxt_setup_resp, msg);

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Sending InitialContextSetupResponse",
              ue_ctxt.ue_ids.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_ids.amf_ue_id);
  amf_notifier.on_new_message(ngap_msg);
}

void ngap_initial_context_setup_procedure::send_initial_context_setup_failure(
    const ngap_init_context_setup_failure& msg,
    const amf_ue_id_t&                     amf_ue_id,
    const ran_ue_id_t&                     ran_ue_id)
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_unsuccessful_outcome();
  ngap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
  auto& init_ctxt_setup_fail           = ngap_msg.pdu.unsuccessful_outcome().value.init_context_setup_fail();
  init_ctxt_setup_fail->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  init_ctxt_setup_fail->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  // Fill PDU Session Resource Failed to Setup List
  fill_asn1_initial_context_setup_failure(init_ctxt_setup_fail, msg);

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Sending InitialContextSetupFailure",
              ue_ctxt.ue_ids.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_ids.amf_ue_id);
  amf_notifier.on_new_message(ngap_msg);
}
