/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_pdu_session_resource_setup_procedure.h"
#include "../ngap/ngap_asn1_helpers.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::ngap;

ngap_pdu_session_resource_setup_procedure::ngap_pdu_session_resource_setup_procedure(
    ngc_ue&                                   ue_,
    cu_cp_pdu_session_resource_setup_request& request_,
    ngc_du_processor_control_notifier&        du_processor_ctrl_notif_,
    ngc_message_notifier&                     amf_notif_,
    srslog::basic_logger&                     logger_) :
  ue(ue_),
  request(request_),
  du_processor_ctrl_notifier(du_processor_ctrl_notif_),
  amf_notifier(amf_notif_),
  logger(logger_)
{
}

void ngap_pdu_session_resource_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("PDU Session Resource Setup started");

  // Handle mandatory IEs
  CORO_AWAIT_VALUE(response, du_processor_ctrl_notifier.on_new_pdu_session_resource_setup_request(request));

  // TODO: Handle optional IEs

  send_pdu_session_resource_setup_response();

  logger.debug("PDU Session Resource Setup finished");
  CORO_RETURN();
}

void ngap_pdu_session_resource_setup_procedure::send_pdu_session_resource_setup_response()
{
  ngc_message ngc_msg = {};

  ngc_msg.pdu.set_successful_outcome();
  ngc_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_SETUP);

  fill_pdu_session_res_setup_resp_s(ngc_msg.pdu.successful_outcome().value.pdu_session_res_setup_resp(), response);

  auto& pdu_session_res_setup_resp = ngc_msg.pdu.successful_outcome().value.pdu_session_res_setup_resp();
  pdu_session_res_setup_resp->amf_ue_ngap_id.value = amf_ue_id_to_uint(ue.get_amf_ue_id());
  pdu_session_res_setup_resp->ran_ue_ngap_id.value = ran_ue_id_to_uint(ue.get_ran_ue_id());

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    ngc_msg.pdu.to_json(js);
    logger.debug("Containerized PDU Session Resource Setup Response Message: {}", js.to_string());
  }

  logger.info("Sending PDU Session Resource Setup Response to AMF");
  amf_notifier.on_new_message(ngc_msg);
}
