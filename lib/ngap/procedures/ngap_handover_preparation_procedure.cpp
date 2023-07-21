/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_handover_preparation_procedure.h"
#include "srsran/ran/bcd_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_handover_preparation_procedure::ngap_handover_preparation_procedure(
    const ngap_handover_preparation_request& request_,
    ngap_context_t&                          context_,
    ngap_ue_manager&                         ue_manager_,
    ngap_message_notifier&                   amf_notif_,
    ngap_transaction_manager&                ev_mng_,
    timer_factory                            timers,
    srslog::basic_logger&                    logger_) :
  request(request_),
  context(context_),
  ue_manager(ue_manager_),
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

  // Lookup UE in UE manager
  ue = ue_manager.find_ngap_ue(request.ue_index);
  if (ue == nullptr) {
    logger.error("ue={}: could not find UE context", request.ue_index);
    CORO_EARLY_RETURN(ngap_handover_preparation_response{false});
  }
  if (ue->get_amf_ue_id() == amf_ue_id_t::invalid || ue->get_ran_ue_id() == ran_ue_id_t::invalid) {
    logger.error(
        "ue={} ran_id={} amf_id={}: invalid NGAP id pair", request.ue_index, ue->get_ran_ue_id(), ue->get_amf_ue_id());
    CORO_EARLY_RETURN(ngap_handover_preparation_response{false});
  }

  // Get required context from UE RRC
  ho_ue_context = ue->get_rrc_ue_control_notifier().on_ue_source_handover_context_required();
  send_handover_required();

  // Forward procedure result to DU manager.
  CORO_RETURN(ngap_handover_preparation_response{true});
}

void ngap_handover_preparation_procedure::send_handover_required()
{
  ngap_message msg = {};
  // set NGAP PDU contents
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_HO_PREP);
  ho_required_s& ho_required = msg.pdu.init_msg().value.ho_required();

  ho_required->amf_ue_ngap_id = amf_ue_id_to_uint(ue->get_amf_ue_id());
  ho_required->ran_ue_ngap_id = ran_ue_id_to_uint(ue->get_ran_ue_id());

  // only intra5gs supported.
  ho_required->handov_type = handov_type_opts::intra5gs;

  ho_required->cause.set_radio_network();
  ho_required->cause.radio_network() = cause_radio_network_opts::ho_desirable_for_radio_reason;

  fill_asn1_target_ran_node_id(ho_required->target_id);
  fill_asn1_pdu_session_res_list(ho_required->pdu_session_res_list_ho_rqd);
  ho_required->source_to_target_transparent_container = fill_ans1_source_to_tartget_transparent_container();

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
  for (auto pdu_session : ho_ue_context.pdu_sessions) {
    pdu_session_res_item_ho_rqd_s pdu_session_item;
    pdu_session_item.pdu_session_id = pdu_session_id_to_uint(pdu_session);

    // pack PDU into temporary buffer
    ho_required_transfer_s ho_required_transfer = {};
    byte_buffer            ho_required_transfer_packed;
    asn1::bit_ref          bref(ho_required_transfer_packed);
    if (ho_required_transfer.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to pack PDU");
      return;
    }
    pdu_session_item.ho_required_transfer = std::move(ho_required_transfer_packed);
    pdu_session_res_list.push_back(pdu_session_item);
  }
}

byte_buffer ngap_handover_preparation_procedure::fill_ans1_source_to_tartget_transparent_container()
{
  struct source_ngran_node_to_target_ngran_node_transparent_container_s transparent_container;
  transparent_container.rrc_container = std::move(ho_ue_context.transparent_container);
  for (auto pdu_session : ho_ue_context.pdu_sessions) {
    pdu_session_res_info_item_s pdu_session_res_info_item;
    pdu_session_res_info_item.pdu_session_id = pdu_session_id_to_uint(pdu_session);
    qos_flow_info_item_s qos_flow_info_item  = {};
    pdu_session_res_info_item.qos_flow_info_list.push_back(qos_flow_info_item);
    transparent_container.pdu_session_res_info_list.push_back(pdu_session_res_info_item);
  }
  transparent_container.target_cell_id.set_nr_cgi();

  last_visited_cell_item_s        last_visited_cell_item;
  last_visited_ngran_cell_info_s& ngran_cell = last_visited_cell_item.last_visited_cell_info.set_ngran_cell();
  ngran_cell.global_cell_id.set_nr_cgi();
  ngran_cell.cell_type.cell_size = cell_size_opts::small;

  transparent_container.ue_history_info.push_back(last_visited_cell_item);

  byte_buffer   buf{};
  asn1::bit_ref bref{buf};
  if (transparent_container.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    logger.error("Failed to pack transparent container.");
    return {};
  }
  return buf;
}
