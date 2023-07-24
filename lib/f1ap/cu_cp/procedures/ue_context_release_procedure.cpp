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

#include "ue_context_release_procedure.h"
#include "../f1ap_asn1_converters.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ran/lcid.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::f1ap;

ue_context_release_procedure::ue_context_release_procedure(f1ap_ue_context_list&                  ue_ctx_list_,
                                                           const f1ap_ue_context_release_command& cmd_,
                                                           f1ap_message_notifier&                 f1ap_notif_,
                                                           srslog::basic_logger&                  logger_) :
  ue_ctxt_list(ue_ctx_list_), ue_ctxt(ue_ctxt_list[cmd_.ue_index]), f1ap_notifier(f1ap_notif_), logger(logger_)
{
  command->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.cu_ue_f1ap_id);
  command->gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.du_ue_f1ap_id);
  command->cause             = cause_to_f1ap_asn1(cmd_.cause);
  if (!cmd_.rrc_release_pdu.empty()) {
    command->rrc_container_present = true;
    command->rrc_container         = cmd_.rrc_release_pdu.copy();

    srsran_assert(cmd_.srb_id.has_value(), "SRB-ID for UE Context Release Command with RRC Container must be set.");

    command->srb_id_present = true;
    command->srb_id         = srb_id_to_uint(cmd_.srb_id.value());
  }
}

void ue_context_release_procedure::operator()(coro_context<async_task<ue_index_t>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", ue_ctxt.ue_index, name());

  transaction_sink.subscribe_to(ue_ctxt.ev_mng.context_release_complete);

  ue_ctxt.marked_for_release = true;

  // Send command to DU.
  send_ue_context_release_command();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  CORO_RETURN(create_ue_context_release_complete(
      transaction_sink.successful() ? transaction_sink.response() : asn1::f1ap::ue_context_release_complete_s{}));
}

void ue_context_release_procedure::send_ue_context_release_command()
{
  // Pack message into PDU
  f1ap_message f1ap_ue_ctxt_rel_msg;
  f1ap_ue_ctxt_rel_msg.pdu.set_init_msg();
  f1ap_ue_ctxt_rel_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  f1ap_ue_ctxt_rel_msg.pdu.init_msg().value.ue_context_release_cmd() = command;

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    f1ap_ue_ctxt_rel_msg.pdu.to_json(js);
    logger.debug("Containerized UeContextReleaseCommand: {}", js.to_string());
  }

  // send DL RRC message
  f1ap_notifier.on_new_message(f1ap_ue_ctxt_rel_msg);
}

ue_index_t
ue_context_release_procedure::create_ue_context_release_complete(const asn1::f1ap::ue_context_release_complete_s& msg)
{
  logger.debug("Received UeContextReleaseComplete");

  ue_index_t ret = ue_index_t::invalid;

  if (msg->gnb_du_ue_f1ap_id == gnb_du_ue_f1ap_id_to_uint(ue_ctxt.du_ue_f1ap_id)) {
    ret = ue_ctxt.ue_index;
    logger.debug("ue={}: \"{}\" finalized.", ret, name());
  } else {
    logger.error("ue={}: \"{}\" failed.", ue_ctxt.ue_index, name());
  }
  ue_ctxt_list.remove_ue(ue_ctxt.cu_ue_f1ap_id);

  return ret;
}
