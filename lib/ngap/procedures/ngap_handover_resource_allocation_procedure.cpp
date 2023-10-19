/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_handover_resource_allocation_procedure.h"
#include "../ngap_asn1_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_handover_resource_allocation_procedure::ngap_handover_resource_allocation_procedure(
    const ngap_handover_request&       request_,
    const amf_ue_id_t                  amf_ue_id_,
    ngap_ue_context_list&              ue_ctxt_list_,
    ngap_cu_cp_du_repository_notifier& du_repository_notif_,
    ngap_message_notifier&             amf_notif_,
    srslog::basic_logger&              logger_) :
  request(request_),
  amf_ue_id(amf_ue_id_),
  ue_ctxt_list(ue_ctxt_list_),
  du_repository_notifier(du_repository_notif_),
  amf_notifier(amf_notif_),
  logger(logger_)
{
}

void ngap_handover_resource_allocation_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized", request.ue_index, name());

  // Notify DU repository about handover request and await requst ack
  CORO_AWAIT_VALUE(response, du_repository_notifier.on_ngap_handover_request(request));

  if (response.success) {
    // Update UE with AMF UE ID
    ngap_ue_context& ue_ctxt = ue_ctxt_list[response.ue_index];
    ue_ctxt_list.add_amf_ue_id(ue_ctxt.ue_ids.ran_ue_id, amf_ue_id);

    send_handover_request_ack(ue_ctxt.ue_ids.ue_index, ue_ctxt.ue_ids.ran_ue_id);
    logger.debug("ue={}: \"{}\" finalized", response.ue_index, name());
  } else {
    send_handover_failure();
    logger.debug("ue={}: \"{}\" failed", response.ue_index, name());
    CORO_EARLY_RETURN();
  }

  CORO_RETURN();
}

void ngap_handover_resource_allocation_procedure::send_handover_request_ack(ue_index_t ue_index, ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_HO_RES_ALLOC);

  fill_asn1_handover_resource_allocation_response(ngap_msg.pdu.successful_outcome().value.ho_request_ack(), response);

  auto& ho_request_ack           = ngap_msg.pdu.successful_outcome().value.ho_request_ack();
  ho_request_ack->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  ho_request_ack->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Sending HoRequestAck", ue_index, ran_ue_id, amf_ue_id);
  amf_notifier.on_new_message(ngap_msg);
}

void ngap_handover_resource_allocation_procedure::send_handover_failure()
{
  ngap_message ngap_msg;
  ngap_msg.pdu.set_unsuccessful_outcome();
  ngap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_HO_RES_ALLOC);

  fill_asn1_handover_resource_allocation_response(ngap_msg.pdu.unsuccessful_outcome().value.ho_fail(), response);

  auto& ho_fail           = ngap_msg.pdu.unsuccessful_outcome().value.ho_fail();
  ho_fail->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);

  logger.info("ue={} amf_ue_id={}: Sending HoFailure", request.ue_index, amf_ue_id);
  amf_notifier.on_new_message(ngap_msg);
}
