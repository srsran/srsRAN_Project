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
    ngc_ue&                                            ue_,
    const asn1::ngap::pdu_session_res_setup_request_s& request_,
    ngc_rrc_ue_control_notifier&                       rrc_ue_ctrl_notif_,
    ngc_message_notifier&                              amf_notif_,
    srslog::basic_logger&                              logger_) :
  ue(ue_), request(request_), rrc_ue_ctrl_notifier(rrc_ue_ctrl_notif_), amf_notifier(amf_notif_), logger(logger_)
{
}

void ng_pdu_session_resource_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("PDU Session Resource Setup started");

  // Handle mandatory IEs
  CORO_AWAIT_VALUE(
      response,
      rrc_ue_ctrl_notifier.on_new_pdu_session_resource_setup_request(request, ue.get_aggregate_maximum_bit_rate_dl()));

  // TODO: Handle optional IEs

  send_pdu_session_resource_setup_response();

  logger.debug("PDU Session Resource Setup finished");
  CORO_RETURN();
}

void ng_pdu_session_resource_setup_procedure::send_pdu_session_resource_setup_response()
{
  ngc_message ngc_msg = {};

  ngc_msg.pdu.set_successful_outcome();
  ngc_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_SETUP);
  ngc_msg.pdu.successful_outcome().value.pdu_session_res_setup_resp() = response;
  auto& pdu_session_res_setup_resp = ngc_msg.pdu.successful_outcome().value.pdu_session_res_setup_resp();
  pdu_session_res_setup_resp->amf_ue_ngap_id.value = amf_ue_id_to_uint(ue.get_amf_ue_id());
  pdu_session_res_setup_resp->ran_ue_ngap_id.value = ran_ue_id_to_uint(ue.get_ran_ue_id());

  logger.info("Sending Initial Context Setup Response to AMF");
  amf_notifier.on_new_message(ngc_msg);
}
