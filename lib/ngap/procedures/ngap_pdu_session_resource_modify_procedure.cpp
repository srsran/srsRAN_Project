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

#include "ngap_pdu_session_resource_modify_procedure.h"
#include "../ngap/ngap_asn1_helpers.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_pdu_session_resource_modify_procedure::ngap_pdu_session_resource_modify_procedure(
    const cu_cp_pdu_session_resource_modify_request& request_,
    const ngap_ue_ids&                               ue_ids_,
    ngap_du_processor_control_notifier&              du_processor_ctrl_notif_,
    ngap_message_notifier&                           amf_notif_,
    ngap_control_message_handler&                    ngap_ctrl_handler_,
    ngap_ue_logger&                                  logger_) :
  request(request_),
  ue_ids(ue_ids_),
  du_processor_ctrl_notifier(du_processor_ctrl_notif_),
  amf_notifier(amf_notif_),
  ngap_ctrl_handler(ngap_ctrl_handler_),
  logger(logger_)
{
}

void ngap_pdu_session_resource_modify_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());

  // Handle mandatory IEs
  CORO_AWAIT_VALUE(response, du_processor_ctrl_notifier.on_new_pdu_session_resource_modify_request(request));

  // TODO: Handle optional IEs

  send_pdu_session_resource_modify_response();

  // Request UE release in case of a failure to cleanup CU-CP
  if (!response.pdu_session_res_failed_to_modify_list.empty()) {
    send_ue_context_release_request();
  }

  logger.log_debug("\"{}\" finalized", name());

  CORO_RETURN();
}

void ngap_pdu_session_resource_modify_procedure::send_pdu_session_resource_modify_response()
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_MODIFY);

  fill_asn1_pdu_session_res_modify_response(ngap_msg.pdu.successful_outcome().value.pdu_session_res_modify_resp(),
                                            response);

  auto& pdu_session_res_setup_resp           = ngap_msg.pdu.successful_outcome().value.pdu_session_res_modify_resp();
  pdu_session_res_setup_resp->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ids.amf_ue_id);
  pdu_session_res_setup_resp->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ids.ran_ue_id);

  logger.log_info("Sending PduSessionResourceModifyResponse");
  amf_notifier.on_new_message(ngap_msg);
}

void ngap_pdu_session_resource_modify_procedure::send_ue_context_release_request()
{
  cu_cp_ue_context_release_request ue_context_release_request{
      ue_ids.ue_index, {}, cause_radio_network_t::release_due_to_ngran_generated_reason};
  ngap_ctrl_handler.handle_ue_context_release_request(ue_context_release_request);
}
