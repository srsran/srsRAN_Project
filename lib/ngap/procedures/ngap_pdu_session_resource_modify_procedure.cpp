/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "../ngap_asn1_converters.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_pdu_session_resource_modify_procedure::ngap_pdu_session_resource_modify_procedure(
    const cu_cp_pdu_session_resource_modify_request&    request_,
    const asn1::ngap::pdu_session_res_modify_request_s& asn1_request_,
    const ngap_ue_ids&                                  ue_ids_,
    ngap_cu_cp_notifier&                                cu_cp_notifier_,
    ngap_message_notifier&                              amf_notifier_,
    ngap_control_message_handler&                       ngap_ctrl_handler_,
    ngap_ue_logger&                                     logger_) :

  request(request_),
  asn1_request(asn1_request_),
  ue_ids(ue_ids_),
  cu_cp_notifier(cu_cp_notifier_),
  amf_notifier(amf_notifier_),
  ngap_ctrl_handler(ngap_ctrl_handler_),
  logger(logger_)
{
}

void ngap_pdu_session_resource_modify_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" started...", name());

  // Verify PDU Session Resource Modify Request.
  verification_outcome = verify_pdu_session_resource_modify_request(request, asn1_request, logger);

  if (verification_outcome.request.pdu_session_res_modify_items.empty()) {
    logger.log_info("Validation of PDUSessionResourceModifyRequest failed");
    response = verification_outcome.response;
  } else {
    // Handle mandatory IEs.
    CORO_AWAIT_VALUE(response, cu_cp_notifier.on_new_pdu_session_resource_modify_request(verification_outcome.request));

    // TODO: Handle optional IEs.

    // Combine validation response with DU processor response.
    combine_pdu_session_resource_modify_response();
  }

  if (!response.pdu_session_res_failed_to_modify_list.empty()) {
    logger.log_info("Some or all PDUSessionResourceModifyItems failed to setup");
  }

  if (send_pdu_session_resource_modify_response()) {
    // Request UE release in case of a failure to cleanup CU-CP.
    if (!response.pdu_session_res_failed_to_modify_list.empty()) {
      ue_context_release_request = {
          ue_ids.ue_index, {}, ngap_cause_radio_network_t::release_due_to_ngran_generated_reason};
      CORO_AWAIT(ngap_ctrl_handler.handle_ue_context_release_request(ue_context_release_request));
    }

    logger.log_debug("\"{}\" finished successfully", name());
  } else {
    logger.log_debug("\"{}\" failed", name());
  }

  CORO_RETURN();
}

void ngap_pdu_session_resource_modify_procedure::combine_pdu_session_resource_modify_response()
{
  for (const auto& modify_item : verification_outcome.response.pdu_session_res_modify_list) {
    response.pdu_session_res_modify_list.emplace(modify_item.pdu_session_id, modify_item);
  }
  for (const auto& failed_item : verification_outcome.response.pdu_session_res_failed_to_modify_list) {
    response.pdu_session_res_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
  }
}

/// \brief Convert common type PDU Session Resource Modify Response message to NGAP PDU Session Resource Modify
/// Response message.
/// \param[out] resp The ASN1 NGAP PDU Session Resource Modify Response message.
/// \param[in]  cu_cp_resp The CU-CP PDU Session Resource Modify Response message.
/// \return True on success, otherwise false.
static bool fill_asn1_pdu_session_res_modify_response(asn1::ngap::pdu_session_res_modify_resp_s&        resp,
                                                      const cu_cp_pdu_session_resource_modify_response& cu_cp_resp)
{
  // Fill PDU Session Resource Modify Response List
  if (!cu_cp_resp.pdu_session_res_modify_list.empty()) {
    resp->pdu_session_res_modify_list_mod_res_present = true;

    for (const auto& cu_cp_resp_item : cu_cp_resp.pdu_session_res_modify_list) {
      asn1::ngap::pdu_session_res_modify_item_mod_res_s resp_item;
      if (!pdu_session_res_modify_response_item_to_asn1(resp_item, cu_cp_resp_item)) {
        return false;
      }
      resp->pdu_session_res_modify_list_mod_res.push_back(resp_item);
    }
  }

  // Fill PDU Session Resource failed to modify list
  if (!cu_cp_resp.pdu_session_res_failed_to_modify_list.empty()) {
    resp->pdu_session_res_failed_to_modify_list_mod_res_present = true;
    for (const auto& cu_cp_resp_item : cu_cp_resp.pdu_session_res_failed_to_modify_list) {
      asn1::ngap::pdu_session_res_failed_to_modify_item_mod_res_s resp_item;
      if (!pdu_session_res_failed_to_modify_item_to_asn1(resp_item, cu_cp_resp_item)) {
        return false;
      }
      resp->pdu_session_res_failed_to_modify_list_mod_res.push_back(resp_item);
    }
  }

  return true;
}

bool ngap_pdu_session_resource_modify_procedure::send_pdu_session_resource_modify_response()
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_MODIFY);

  auto& pdu_session_res_modify_resp           = ngap_msg.pdu.successful_outcome().value.pdu_session_res_modify_resp();
  pdu_session_res_modify_resp->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ids.amf_ue_id);
  pdu_session_res_modify_resp->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ids.ran_ue_id);

  // TODO: needs more handling in the coro above?
  if (not fill_asn1_pdu_session_res_modify_response(pdu_session_res_modify_resp, response)) {
    logger.log_warning("Unable to fill ASN1 contents of PDUSessionResourceModifyResponse", name());
    return false;
  }

  // Forward message to AMF.
  if (!amf_notifier.on_new_message(ngap_msg)) {
    logger.log_warning("AMF notifier is not set. Cannot send PDUSessionResourceModifyResponse");
    return false;
  }

  return true;
}
