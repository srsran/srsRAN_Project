/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ng_pdu_session_resource_setup_procedure.h"
#include "ng_procedure_helpers.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::ngap;

ng_pdu_session_resource_setup_procedure::ng_pdu_session_resource_setup_procedure(
    ngc_ue&                             ue_,
    pdu_session_resource_setup_message& msg_,
    ngc_e1_control_notifier&            e1_notif_,
    ngc_message_notifier&               amf_notif_,
    srslog::basic_logger&               logger_) :
  ue(ue_), msg(msg_), e1_notifier(e1_notif_), amf_notifier(amf_notif_), logger(logger_)
{
}

void ng_pdu_session_resource_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("PDU Session Resource Setup started");

  // Handle mendatorty IEs
  CORO_AWAIT_VALUE(response, e1_notifier.on_new_pdu_session_resource_setup_request(msg));

  // TODO: Handle optional IEs

  send_pdu_session_resource_setup_response();

  logger.debug("Initial Context Setup Procedure finished");
  CORO_RETURN();
}

void ng_pdu_session_resource_setup_procedure::send_pdu_session_resource_setup_response()
{
  ngc_message ngc_msg = {};

  ngc_msg.pdu.set_successful_outcome();
  ngc_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
  auto& init_ctxt_setup_resp                 = ngc_msg.pdu.successful_outcome().value.init_context_setup_resp();
  init_ctxt_setup_resp->amf_ue_ngap_id.value = amf_ue_id_to_uint(ue.get_amf_ue_id());
  init_ctxt_setup_resp->ran_ue_ngap_id.value = ran_ue_id_to_uint(ue.get_ran_ue_id());

  // Fill PDU Session Resource Setup Response List
  if (!response.cu_cp_pdu_session_res_setup_response_items.empty()) {
    init_ctxt_setup_resp->pdu_session_res_setup_list_cxt_res_present = true;
    init_ctxt_setup_resp->pdu_session_res_setup_list_cxt_res->resize(
        response.cu_cp_pdu_session_res_setup_response_items.size());
    for (auto& cu_cp_resp_item : response.cu_cp_pdu_session_res_setup_response_items) {
      asn1::ngap::pdu_session_res_setup_item_cxt_res_s res_item;
      res_item.pdu_session_id = cu_cp_resp_item.pdu_session_id;
      res_item.pdu_session_res_setup_resp_transfer.resize(
          cu_cp_resp_item.pdu_session_resource_setup_response_transfer.length());
      std::copy(cu_cp_resp_item.pdu_session_resource_setup_response_transfer.begin(),
                cu_cp_resp_item.pdu_session_resource_setup_response_transfer.end(),
                res_item.pdu_session_res_setup_resp_transfer.begin());

      init_ctxt_setup_resp->pdu_session_res_setup_list_cxt_res->push_back(res_item);
    }
  }

  // Fill PDU Session Resource Failed to Setup List
  if (!response.cu_cp_pdu_session_res_setup_failed_items.empty()) {
    init_ctxt_setup_resp->pdu_session_res_failed_to_setup_list_cxt_res_present = true;
    init_ctxt_setup_resp->pdu_session_res_failed_to_setup_list_cxt_res->resize(
        response.cu_cp_pdu_session_res_setup_failed_items.size());
    for (auto& cu_cp_setup_failed_item : response.cu_cp_pdu_session_res_setup_failed_items) {
      asn1::ngap::pdu_session_res_failed_to_setup_item_cxt_res_s res_item;
      res_item.pdu_session_id = cu_cp_setup_failed_item.pdu_session_id;
      res_item.pdu_session_res_setup_unsuccessful_transfer.resize(
          cu_cp_setup_failed_item.pdu_session_resource_setup_unsuccessful_transfer.length());
      std::copy(cu_cp_setup_failed_item.pdu_session_resource_setup_unsuccessful_transfer.begin(),
                cu_cp_setup_failed_item.pdu_session_resource_setup_unsuccessful_transfer.end(),
                res_item.pdu_session_res_setup_unsuccessful_transfer.begin());

      init_ctxt_setup_resp->pdu_session_res_failed_to_setup_list_cxt_res->push_back(res_item);
    }
  }

  // TODO: Fill Criticality Diagnostics
  // if (response.crit_diagnostics.has_value()) {
  //   init_ctxt_setup_resp->crit_diagnostics_present = true;
  //   init_ctxt_setup_resp->crit_diagnostics.value   = response.crit_diagnostics.value();
  // }

  logger.info("Sending Initial Context Setup Response to AMF");
  amf_notifier.on_new_message(ngc_msg);
}
