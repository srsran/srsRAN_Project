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

#include "initial_context_setup_routine.h"
#include "pdu_session_routine_helpers.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/rrc/rrc_ue.h"
#include <optional>

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

initial_context_setup_routine::initial_context_setup_routine(
    const ngap_init_context_setup_request&       request_,
    rrc_ue_interface&                            rrc_ue_,
    ngap_ue_radio_capability_management_handler& ngap_ue_radio_cap_handler_,
    ue_security_manager&                         security_mng_,
    f1ap_ue_context_manager&                     f1ap_ue_ctxt_mng_,
    cu_cp_ngap_handler&                          pdu_session_setup_handler_,
    srslog::basic_logger&                        logger_) :
  request(request_),
  rrc_ue(rrc_ue_),
  ngap_ue_radio_cap_handler(ngap_ue_radio_cap_handler_),
  security_mng(security_mng_),
  f1ap_ue_ctxt_mng(f1ap_ue_ctxt_mng_),
  pdu_session_setup_handler(pdu_session_setup_handler_),
  logger(logger_)
{
}

void initial_context_setup_routine::operator()(
    coro_context<async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("ue={}: \"{}\" initialized", request.ue_index, name());

  // Initialize security context
  if (!security_mng.init_security_context(request.security_context)) {
    handle_failure();
    CORO_EARLY_RETURN(make_unexpected(fail_msg));
  }

  // Get Security Mode Command from RRC UE
  {
    rrc_smc_ctxt = rrc_ue.get_security_mode_command_context();
    if (rrc_smc_ctxt.rrc_ue_security_mode_command_pdu.empty()) {
      handle_failure();
      CORO_EARLY_RETURN(make_unexpected(fail_msg));
    }
  }

  // Prepare F1AP UE Context Setup Request and call F1AP notifier
  {
    // Add remaining fields to UE Context Setup Request
    ue_context_setup_request.ue_index      = request.ue_index;
    ue_context_setup_request.sp_cell_id    = rrc_smc_ctxt.sp_cell_id;
    ue_context_setup_request.serv_cell_idx = 0; // TODO: Remove hardcoded value
    ue_context_setup_request.rrc_container = rrc_smc_ctxt.rrc_ue_security_mode_command_pdu.copy();

    // Call F1AP procedure
    CORO_AWAIT_VALUE(ue_context_setup_response,
                     f1ap_ue_ctxt_mng.handle_ue_context_setup_request(ue_context_setup_request, std::nullopt));
    // Handle UE Context Setup Response
    if (!ue_context_setup_response.success) {
      handle_failure();
      CORO_EARLY_RETURN(make_unexpected(fail_msg));
    }
  }

  // Await Security Mode Complete from RRC UE
  {
    CORO_AWAIT_VALUE(security_mode_command_result,
                     rrc_ue.handle_security_mode_complete_expected(rrc_smc_ctxt.transaction_id));
    if (!security_mode_command_result) {
      handle_failure();
      CORO_EARLY_RETURN(make_unexpected(fail_msg));
    }
  }

  // Start UE Capability Enquiry Procedure
  {
    CORO_AWAIT_VALUE(ue_capability_transfer_result,
                     rrc_ue.handle_rrc_ue_capability_transfer_request(ue_capability_transfer_request));

    // Handle UE Capability Transfer result
    if (not ue_capability_transfer_result) {
      logger.warning("ue={}: \"{}\" UE capability transfer failed", request.ue_index, name());
      handle_failure();
      CORO_EARLY_RETURN(make_unexpected(fail_msg));
    }
  }

  // Handle optional IEs

  // Handle PDU Session Resource Setup List Context Request
  /// NOTE: The handling of this includes the RRC Reconfiguration procedure
  if (request.pdu_session_res_setup_list_cxt_req.has_value()) {
    request.pdu_session_res_setup_list_cxt_req.value().ue_index     = request.ue_index;
    request.pdu_session_res_setup_list_cxt_req.value().serving_plmn = request.guami.plmn;
    if (request.ue_aggr_max_bit_rate.has_value()) {
      request.pdu_session_res_setup_list_cxt_req.value().ue_aggregate_maximum_bit_rate_dl =
          request.ue_aggr_max_bit_rate.value().ue_aggr_max_bit_rate_dl;
    } else {
      request.pdu_session_res_setup_list_cxt_req.value().ue_aggregate_maximum_bit_rate_dl = 0;
    }

    // Handle NAS PDUs from Initial Context Setup Request
    if (request.nas_pdu.has_value()) {
      request.pdu_session_res_setup_list_cxt_req.value().nas_pdu = request.nas_pdu.value().copy();
    }

    CORO_AWAIT_VALUE(pdu_session_setup_response,
                     pdu_session_setup_handler.handle_new_pdu_session_resource_setup_request(
                         request.pdu_session_res_setup_list_cxt_req.value()));

    resp_msg.pdu_session_res_setup_response_items  = pdu_session_setup_response.pdu_session_res_setup_response_items;
    resp_msg.pdu_session_res_failed_to_setup_items = pdu_session_setup_response.pdu_session_res_failed_to_setup_items;
  } else {
    // Handle NAS PDUs from Initial Context Setup Request
    if (request.nas_pdu.has_value()) {
      handle_nas_pdu(request.nas_pdu.value().copy());
    }
  }

  // Schedule transmission of UE Radio Capability Info Indication
  send_ue_radio_capability_info_indication();

  logger.info("ue={}: \"{}\" finished successfully", request.ue_index, name());
  CORO_RETURN(resp_msg);
}

void initial_context_setup_routine::handle_failure()
{
  fail_msg.cause = cause_protocol_t::unspecified;
  // Add failed PDU Sessions
  if (request.pdu_session_res_setup_list_cxt_req.has_value()) {
    for (const auto& pdu_session_item :
         request.pdu_session_res_setup_list_cxt_req.value().pdu_session_res_setup_items) {
      cu_cp_pdu_session_res_setup_failed_item failed_item;
      failed_item.pdu_session_id              = pdu_session_item.pdu_session_id;
      failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::unspecified;

      fail_msg.pdu_session_res_failed_to_setup_items.emplace(pdu_session_item.pdu_session_id, failed_item);
    }
  }

  logger.info("ue={}: \"{}\" failed", request.ue_index, name());
}

void initial_context_setup_routine::handle_nas_pdu(byte_buffer nas_pdu)
{
  logger.debug("ue={}: Forwarding NAS PDU to RRC", request.ue_index);
  rrc_ue.handle_dl_nas_transport_message(std::move(nas_pdu));
}

void initial_context_setup_routine::send_ue_radio_capability_info_indication()
{
  ue_radio_cap_info_indication.ue_index                  = request.ue_index;
  ue_radio_cap_info_indication.ue_cap_rat_container_list = rrc_ue.get_packed_ue_radio_access_cap_info();

  ngap_ue_radio_cap_handler.handle_tx_ue_radio_capability_info_indication_required(ue_radio_cap_info_indication);
}