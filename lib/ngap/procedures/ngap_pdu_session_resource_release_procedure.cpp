/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_pdu_session_resource_release_procedure.h"
#include "../ngap/ngap_asn1_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_pdu_session_resource_release_procedure::ngap_pdu_session_resource_release_procedure(
    ngap_ue&                                          ue_,
    const cu_cp_pdu_session_resource_release_command& command_,
    ngap_du_processor_control_notifier&               du_processor_ctrl_notif_,
    ngap_message_notifier&                            amf_notif_,
    srslog::basic_logger&                             logger_) :
  ue(ue_),
  command(command_),
  du_processor_ctrl_notifier(du_processor_ctrl_notif_),
  amf_notifier(amf_notif_),
  logger(logger_)
{
}

void ngap_pdu_session_resource_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("PDU Session Resource Release Procedure started");

  // Handle mandatory IEs
  CORO_AWAIT_VALUE(response, du_processor_ctrl_notifier.on_new_pdu_session_resource_release_command(command));

  // TODO: Handle optional IEs

  send_pdu_session_resource_release_response();

  logger.debug("PDU Session Resource Release Procedure finished");
  CORO_RETURN();
}

void ngap_pdu_session_resource_release_procedure::send_pdu_session_resource_release_response()
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_RELEASE);

  fill_asn1_pdu_session_resource_release_response(
      ngap_msg.pdu.successful_outcome().value.pdu_session_res_release_resp(), response);

  auto& pdu_session_res_release_resp = ngap_msg.pdu.successful_outcome().value.pdu_session_res_release_resp();
  pdu_session_res_release_resp->amf_ue_ngap_id.value = amf_ue_id_to_uint(ue.get_amf_ue_id());
  pdu_session_res_release_resp->ran_ue_ngap_id.value = ran_ue_id_to_uint(ue.get_ran_ue_id());

  logger.info("Sending PduSessionResourceReleaseResponse");
  amf_notifier.on_new_message(ngap_msg);
}
