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

#include "ngap_handover_preparation_procedure.h"
#include "../ngap_asn1_converters.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_handover_preparation_procedure::ngap_handover_preparation_procedure(
    const ngap_handover_preparation_request& request_,
    ngap_context_t&                          context_,
    ngap_message_notifier&                   amf_notif_,
    ngap_transaction_manager&                ev_mng_,
    timer_factory                            timers,
    srslog::basic_logger&                    logger_) :
  request(request_),
  context(context_),
  amf_notifier(amf_notif_),
  ev_mng(ev_mng_),
  logger(logger_),
  tng_reloc_prep_timer(timers.create_timer())
{
  (void)context;
  (void)ev_mng;
  (void)logger;
}

void ngap_handover_preparation_procedure::operator()(coro_context<async_task<ngap_handover_preparation_response>>& ctx)
{
  CORO_BEGIN(ctx);

  send_handover_required();

  // Forward procedure result to DU manager.
  CORO_RETURN(ngap_handover_preparation_response{});
}

void ngap_handover_preparation_procedure::send_handover_required()
{
  ngap_message msg = {};
  // set NGAP PDU contents
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_HO_PREP);
  ho_required_s& ho_required = msg.pdu.init_msg().value.ho_required();

  ho_required->amf_ue_ngap_id = 1;
  ho_required->ran_ue_ngap_id = 1;

  // only intra5gs supported.
  ho_required->handov_type = handov_type_opts::intra5gs;

  ho_required->cause.set_radio_network();
  ho_required->cause.radio_network() = cause_radio_network_opts::ho_desirable_for_radio_reason;

  ho_required->target_id.set_target_ran_node_id();
  ho_required->target_id.target_ran_node_id();
  ho_required->target_id.target_ran_node_id().global_ran_node_id.set(global_ran_node_id_c::types::global_gnb_id);
  ho_required->target_id.target_ran_node_id().global_ran_node_id.global_gnb_id().plmn_id.from_string("001001");
  ho_required->target_id.target_ran_node_id().global_ran_node_id.global_gnb_id().gnb_id.set_gnb_id();
  ho_required->target_id.target_ran_node_id().global_ran_node_id.global_gnb_id().gnb_id.gnb_id().from_string(
      "0000000000000000000001");

  //
  pdu_session_res_item_ho_rqd_s pdu_session_item = {};
  ho_required->pdu_session_res_list_ho_rqd.push_back(pdu_session_item);

  amf_notifier.on_new_message(msg);
}
