/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/ngap/ngap_message.h"
#include "srsran/ran/bcd_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_handover_preparation_procedure::ngap_handover_preparation_procedure(
    const ngap_handover_preparation_request& request_,
    const ngap_context_t&                    context_,
    const ngap_ue_ids&                       ue_ids_,
    ngap_message_notifier&                   amf_notif_,
    ngap_rrc_ue_control_notifier&            rrc_ue_notif_,
    up_resource_manager&                     up_manager_,
    ngap_transaction_manager&                ev_mng_,
    timer_factory                            timers,
    ngap_ue_logger&                          logger_) :
  request(request_),
  context(context_),
  ue_ids(ue_ids_),
  amf_notifier(amf_notif_),
  rrc_ue_notifier(rrc_ue_notif_),
  up_manager(up_manager_),
  ev_mng(ev_mng_),
  logger(logger_),
  tng_reloc_prep_timer(timers.create_timer())
{
}

void ngap_handover_preparation_procedure::operator()(coro_context<async_task<ngap_handover_preparation_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());

  if (ue_ids.amf_ue_id == amf_ue_id_t::invalid || ue_ids.ran_ue_id == ran_ue_id_t::invalid) {
    logger.log_error("Invalid NGAP id pair");
    CORO_EARLY_RETURN(ngap_handover_preparation_response{false});
  }

  // Subscribe to respective publisher to receive HANDOVER COMMAND/HANDOVER PREPARATION FAILURE message.
  transaction_sink.subscribe_to(ev_mng.handover_preparation_outcome, tng_reloc_prep_ms);

  // Get required context from RRC UE
  get_required_handover_context();

  // Send Handover Required to AMF
  send_handover_required();

  CORO_AWAIT(transaction_sink);

  if (transaction_sink.timeout_expired()) {
    logger.log_warning("\"{}\" timed out after {}ms", name(), tng_reloc_prep_ms.count());
    // TODO: Initialize Handover Cancellation procedure
  }

  if (transaction_sink.successful()) {
    // Forward RRC Container
    if (!forward_rrc_handover_command()) {
      CORO_EARLY_RETURN(ngap_handover_preparation_response{false});
    }
    logger.log_debug("\"{}\" finalized", name());
  }

  // Forward procedure result to DU manager.
  CORO_RETURN(ngap_handover_preparation_response{true});
}

void ngap_handover_preparation_procedure::get_required_handover_context()
{
  ngap_ue_source_handover_context                           src_ctx;
  const std::map<pdu_session_id_t, up_pdu_session_context>& pdu_sessions = up_manager.get_pdu_sessions_map();
  // create a map of all PDU sessions and their associated QoS flows
  for (const auto& pdu_session : pdu_sessions) {
    std::vector<qos_flow_id_t> qos_flows;
    for (const auto& drb : pdu_session.second.drbs) {
      for (const auto& qos_flow : drb.second.qos_flows) {
        qos_flows.push_back(qos_flow.first);
      }
    }
    ho_ue_context.pdu_sessions.insert({pdu_session.first, qos_flows});
  }
  ho_ue_context.rrc_container = rrc_ue_notifier.on_handover_preparation_message_required();
}

void ngap_handover_preparation_procedure::send_handover_required()
{
  ngap_message msg = {};
  // set NGAP PDU contents
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_HO_PREP);
  ho_required_s& ho_required = msg.pdu.init_msg().value.ho_required();

  ho_required->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ids.amf_ue_id);
  ho_required->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ids.ran_ue_id);

  // only intra5gs supported.
  ho_required->handov_type = handov_type_opts::intra5gs;

  ho_required->cause.set_radio_network();
  ho_required->cause.radio_network() = cause_radio_network_opts::ho_desirable_for_radio_reason;

  fill_asn1_target_ran_node_id(ho_required->target_id);
  fill_asn1_pdu_session_res_list(ho_required->pdu_session_res_list_ho_rqd);
  ho_required->source_to_target_transparent_container = fill_asn1_source_to_target_transparent_container();

  amf_notifier.on_new_message(msg);
}

void ngap_handover_preparation_procedure::fill_asn1_target_ran_node_id(target_id_c& target_id)
{
  target_id.set_target_ran_node_id();
  target_id.target_ran_node_id();
  target_id.target_ran_node_id().global_ran_node_id.set(global_ran_node_id_c::types::global_gnb_id);
  target_id.target_ran_node_id().global_ran_node_id.global_gnb_id().plmn_id.from_number(
      plmn_string_to_bcd(context.plmn)); // cross-PLMN handover not supported
  target_id.target_ran_node_id().global_ran_node_id.global_gnb_id().gnb_id.set_gnb_id();
  target_id.target_ran_node_id().global_ran_node_id.global_gnb_id().gnb_id.gnb_id().from_number(request.gnb_id);
}

void ngap_handover_preparation_procedure::fill_asn1_pdu_session_res_list(
    pdu_session_res_list_ho_rqd_l& pdu_session_res_list)
{
  for (const auto& pdu_session : ho_ue_context.pdu_sessions) {
    pdu_session_res_item_ho_rqd_s pdu_session_item;
    pdu_session_item.pdu_session_id = pdu_session_id_to_uint(pdu_session.first);

    // pack PDU into temporary buffer
    ho_required_transfer_s ho_required_transfer = {};
    byte_buffer            ho_required_transfer_packed;
    asn1::bit_ref          bref(ho_required_transfer_packed);
    if (ho_required_transfer.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.log_error("Failed to pack PDU");
      return;
    }
    pdu_session_item.ho_required_transfer = std::move(ho_required_transfer_packed);
    pdu_session_res_list.push_back(pdu_session_item);
  }
}

byte_buffer ngap_handover_preparation_procedure::fill_asn1_source_to_target_transparent_container()
{
  struct source_ngran_node_to_target_ngran_node_transparent_container_s transparent_container;
  transparent_container.rrc_container = std::move(ho_ue_context.rrc_container);
  for (const auto& pdu_session : ho_ue_context.pdu_sessions) {
    pdu_session_res_info_item_s pdu_session_res_info_item;
    pdu_session_res_info_item.pdu_session_id = pdu_session_id_to_uint(pdu_session.first);
    for (const auto& qos_flow : pdu_session.second) {
      qos_flow_info_item_s qos_flow_info_item = {};
      // set qfi
      qos_flow_info_item.qos_flow_id = qos_flow_id_to_uint(qos_flow);
      pdu_session_res_info_item.qos_flow_info_list.push_back(qos_flow_info_item);
    }
    transparent_container.pdu_session_res_info_list.push_back(pdu_session_res_info_item);
  }
  nr_cgi_s& target_nr_cgi = transparent_container.target_cell_id.set_nr_cgi();

  target_nr_cgi.plmn_id.from_number(plmn_string_to_bcd(context.plmn)); // cross-PLMN handover not supported
  target_nr_cgi.nr_cell_id.from_number(request.nci);

  last_visited_cell_item_s        last_visited_cell_item;
  last_visited_ngran_cell_info_s& ngran_cell = last_visited_cell_item.last_visited_cell_info.set_ngran_cell();
  ngran_cell.global_cell_id.set_nr_cgi();
  ngran_cell.cell_type.cell_size = cell_size_opts::small;

  transparent_container.ue_history_info.push_back(last_visited_cell_item);

  byte_buffer   buf{};
  asn1::bit_ref bref{buf};
  if (transparent_container.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    logger.log_error("Failed to pack transparent container.");
    return {};
  }
  return buf;
}

bool ngap_handover_preparation_procedure::forward_rrc_handover_command()
{
  auto& target_to_source_container_packed = transaction_sink.response()->target_to_source_transparent_container;

  asn1::ngap::target_ngran_node_to_source_ngran_node_transparent_container_s target_to_source_container;
  asn1::cbit_ref bref({target_to_source_container_packed.begin(), target_to_source_container_packed.end()});

  if (target_to_source_container.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.log_error("Couldn't unpack target to source transparent container.");
    return false;
  }

  // Send RRC Container to RRC
  return rrc_ue_notifier.on_new_rrc_handover_command(target_to_source_container.rrc_container.copy());
}
