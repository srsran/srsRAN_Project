/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_pdu_session_resource_modify_procedure.h"
#include "../ngap/ngap_asn1_helpers.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_pdu_session_resource_modify_procedure::ngap_pdu_session_resource_modify_procedure(
    const cu_cp_pdu_session_resource_modify_request&    request_,
    const asn1::ngap::pdu_session_res_modify_request_s& asn1_request_,
    const ngap_ue_ids&                                  ue_ids_,
    ngap_du_processor_control_notifier&                 du_processor_ctrl_notifier_,
    ngap_message_notifier&                              amf_notifier_,
    ngap_control_message_handler&                       ngap_ctrl_handler_,
    ngap_ue_logger&                                     logger_) :

  request(request_),
  asn1_request(asn1_request_),
  ue_ids(ue_ids_),
  du_processor_ctrl_notifier(du_processor_ctrl_notifier_),
  amf_notifier(amf_notifier_),
  ngap_ctrl_handler(ngap_ctrl_handler_),
  logger(logger_)
{
}

void ngap_pdu_session_resource_modify_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());

  // Verify PDU Session Resource Modify Request
  verification_outcome = verify_pdu_session_resource_modify_request(request, asn1_request, logger);

  if (verification_outcome.request.pdu_session_res_modify_items.empty()) {
    logger.log_info("Validation of PduSessionResourceModifyRequest failed");
    response = verification_outcome.response;
  } else {
    // Handle mandatory IEs
    CORO_AWAIT_VALUE(
        response, du_processor_ctrl_notifier.on_new_pdu_session_resource_modify_request(verification_outcome.request));

    // TODO: Handle optional IEs

    // Combine validation response with DU processor response
    combine_pdu_session_resource_modify_response();
  }

  if (!response.pdu_session_res_failed_to_modify_list.empty()) {
    logger.log_warning("Some or all PduSessionResourceModifyItems failed to setup");
  }

  send_pdu_session_resource_modify_response();

  // Request UE release in case of a failure to cleanup CU-CP
  if (!response.pdu_session_res_failed_to_modify_list.empty()) {
    ue_context_release_request = {
        ue_ids.ue_index, {}, ngap_cause_radio_network_t::release_due_to_ngran_generated_reason};
    CORO_AWAIT(ngap_ctrl_handler.handle_ue_context_release_request(ue_context_release_request));
  }

  logger.log_debug("\"{}\" finalized", name());

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

void ngap_pdu_session_resource_modify_procedure::send_pdu_session_resource_modify_response()
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
    return;
  }

  amf_notifier.on_new_message(ngap_msg);
}
