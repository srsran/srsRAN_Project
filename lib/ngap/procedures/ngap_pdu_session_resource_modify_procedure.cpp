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

#include "ngap_pdu_session_resource_modify_procedure.h"
#include "../ngap/ngap_asn1_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_pdu_session_resource_modify_procedure::ngap_pdu_session_resource_modify_procedure(
    const cu_cp_pdu_session_resource_modify_request& request_,
    ngap_ue&                                         ue_,
    ngap_du_processor_control_notifier&              du_processor_ctrl_notif_,
    ngap_message_notifier&                           amf_notif_,
    srslog::basic_logger&                            logger_) :
  request(request_),
  ue(ue_),
  du_processor_ctrl_notifier(du_processor_ctrl_notif_),
  amf_notifier(amf_notif_),
  logger(logger_)
{
}

void ngap_pdu_session_resource_modify_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={} PDU Session Resource Modify Procedure started", ue.get_ue_index());

  // Handle mandatory IEs
  CORO_AWAIT_VALUE(response, du_processor_ctrl_notifier.on_new_pdu_session_resource_modify_request(request));

  // TODO: Handle optional IEs

  send_pdu_session_resource_modify_response();

  logger.debug("ue={} PDU Session Resource Modify Procedure finished", ue.get_ue_index());
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
  pdu_session_res_setup_resp->amf_ue_ngap_id = amf_ue_id_to_uint(ue.get_amf_ue_id());
  pdu_session_res_setup_resp->ran_ue_ngap_id = ran_ue_id_to_uint(ue.get_ran_ue_id());

  logger.info("ue={} Sending PduSessionResourceModifyResponse", ue.get_ue_index());
  amf_notifier.on_new_message(ngap_msg);
}
