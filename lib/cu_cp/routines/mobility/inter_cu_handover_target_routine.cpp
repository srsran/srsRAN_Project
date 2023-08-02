/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "inter_cu_handover_target_routine.h"
#include "../pdu_session_routine_helpers.h"
#include "srsran/ngap/ngap_handover.h"

using namespace srsran;
using namespace srs_cu_cp;

inter_cu_handover_target_routine::inter_cu_handover_target_routine(
    const ngap_handover_request&           request_,
    du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notif_,
    du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
    du_processor_ue_manager&               ue_manager_,
    srslog::basic_logger&                  logger_) :
  request(request_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  ue_manager(ue_manager_),
  logger(logger_)
{
}

void inter_cu_handover_target_routine::operator()(
    coro_context<async_task<ngap_handover_resource_allocation_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", request.ue_index, name());

  ue = ue_manager.find_ue(request.ue_index);

  // Perform initial sanity checks on incoming message.
  if (!ue->get_up_resource_manager().validate_request(request.pdu_session_res_setup_list_ho_req)) {
    logger.error("ue={}: \"{}\" Invalid PDU Session Resource Setup", request.ue_index, name());
    CORO_EARLY_RETURN(generate_handover_request_response(false));
  }

  {
    // Calculate next user-plane configuration based on incoming setup message.
    next_config = ue->get_up_resource_manager().calculate_update(request.pdu_session_res_setup_list_ho_req);
  }

  // prepare Bearer Context Setup Request and call E1AP notifier
  {
    fill_e1ap_bearer_context_setup_request();

    // call E1AP procedure
    CORO_AWAIT_VALUE(bearer_context_setup_response,
                     e1ap_ctrl_notifier.on_bearer_context_setup_request(bearer_context_setup_request));

    // Handle BearerContextSetupResponse
    if (!bearer_context_setup_response.success) {
      logger.error("ue={}: \"{}\" failed to setup bearer at CU-UP.", request.ue_index, name());
      CORO_EARLY_RETURN(generate_handover_request_response(false));
    }
  }

  // prepare F1AP UE Context Setup Command and call F1AP notifier
  {
    fill_f1ap_ue_context_setup_request();

    // call F1AP procedure
    CORO_AWAIT_VALUE(ue_context_setup_response,
                     f1ap_ue_ctxt_notifier.on_ue_context_setup_request(ue_context_setup_request));
    // Handle UeContextSetupResponse
    if (ue_context_setup_response.ue_index == ue_index_t::invalid) {
      logger.error("ue={}: \"{}\" Failed to create UE at the target DU.", request.ue_index, name());
      CORO_EARLY_RETURN(generate_handover_request_response(false));
    }
  }

  // prepare Bearer Context Modification Request and call E1AP notifier
  {
    fill_e1ap_bearer_context_modification_request();

    // call E1AP procedure
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (!bearer_context_modification_response.success) {
      logger.error("ue={}: \"{}\" failed to modify bearer at CU-UP.", request.ue_index, name());
      CORO_EARLY_RETURN(generate_handover_request_response(false));
    }
  }

  // Get RRC Reconfiguration container
  rrc_reconfiguration = ue->get_rrc_ue_notifier().on_rrc_reconfiguration_pdu_required(rrc_reconfig_args);

  CORO_RETURN(generate_handover_request_response(true));
}

void inter_cu_handover_target_routine::fill_e1ap_bearer_context_setup_request()
{
  bearer_context_setup_request.ue_index = request.ue_index;

  // security info
  bearer_context_setup_request.security_info.security_algorithm.ciphering_algo =
      request.security_context.sel_algos.cipher_algo;
  bearer_context_setup_request.security_info.security_algorithm.integrity_protection_algorithm =
      request.security_context.sel_algos.integ_algo;
  bearer_context_setup_request.security_info.up_security_key.encryption_key = request.security_context.as_keys.k_up_enc;
  bearer_context_setup_request.security_info.up_security_key.integrity_protection_key =
      request.security_context.as_keys.k_up_int;

  bearer_context_setup_request.ue_dl_aggregate_maximum_bit_rate = request.ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl;
  bearer_context_setup_request.serving_plmn = request.source_to_target_transparent_container.target_cell_id.plmn;
  bearer_context_setup_request.activity_notif_level = "ue"; // TODO: Remove hardcoded value
  if (bearer_context_setup_request.activity_notif_level == "ue") {
    bearer_context_setup_request.ue_inactivity_timer = ue_manager.get_ue_config().inactivity_timer;
  }

  // Add new PDU sessions.
  fill_e1ap_pdu_session_res_to_setup_list(bearer_context_setup_request.pdu_session_res_to_setup_list,
                                          logger,
                                          next_config,
                                          request.pdu_session_res_setup_list_ho_req,
                                          ue_manager.get_ue_config());
}

void inter_cu_handover_target_routine::fill_f1ap_ue_context_setup_request()
{
  ue_context_setup_request.serv_cell_idx = 0;
  ue_context_setup_request.sp_cell_id    = request.source_to_target_transparent_container.target_cell_id;

  // Request setup of SRB0, SRB1 and SRB2
  for (unsigned srb_id = 0; srb_id < 3; ++srb_id) {
    f1ap_srbs_to_be_setup_mod_item srb_item;
    srb_item.srb_id = int_to_srb_id(srb_id);
    ue_context_setup_request.srbs_to_be_setup_list.emplace(srb_item.srb_id, srb_item);
  }

  for (const auto& pdu_session : next_config.pdu_sessions_to_setup_list) {
    for (const auto& drb : pdu_session.second.drb_to_add) {
      const up_drb_context& drb_context = drb.second;

      f1ap_drbs_to_be_setup_mod_item drb_item;
      drb_item.drb_id           = drb.first;
      drb_item.qos_info.drb_qos = drb_context.qos_params;

      // Add each QoS flow including QoS.
      for (auto& flow : drb_context.qos_flows) {
        f1ap_flows_mapped_to_drb_item flow_item;
        flow_item.qos_flow_id               = flow.first;
        flow_item.qos_flow_level_qos_params = flow.second.qos_params;
        drb_item.qos_info.flows_mapped_to_drb_list.emplace(flow_item.qos_flow_id, flow_item);
      }
      drb_item.ul_up_tnl_info_to_be_setup_list = drb_context.ul_up_tnl_info_to_be_setup_list;
      drb_item.rlc_mod                         = drb_context.rlc_mod;

      ue_context_setup_request.drbs_to_be_setup_list.emplace(drb_item.drb_id, drb_item);
    }
  }
}

void inter_cu_handover_target_routine::fill_e1ap_bearer_context_modification_request()
{
  bearer_context_modification_request.ue_index = request.ue_index;

  // Start with empty message.
  e1ap_ng_ran_bearer_context_mod_request& e1ap_bearer_context_mod =
      bearer_context_modification_request.ng_ran_bearer_context_mod_request.emplace();

  fill_e1ap_bearer_context_list(e1ap_bearer_context_mod.pdu_session_res_to_modify_list,
                                ue_context_setup_response.drbs_setup_list,
                                next_config.pdu_sessions_to_setup_list);
}

ngap_handover_resource_allocation_response
inter_cu_handover_target_routine::generate_handover_request_response(bool success)
{
  if (success) {
    response.success  = true;
    response.ue_index = ue->get_ue_index();

    // fill handover request ack
    // pdu session res admitted list
    for (const auto& pdu_session : bearer_context_setup_response.pdu_session_resource_setup_list) {
      ngap_pdu_session_res_admitted_item admitted_item;

      // pdu session id
      admitted_item.pdu_session_id = pdu_session.pdu_session_id;

      // ho request ack transfer
      // dl ngu up tnl info
      admitted_item.ho_request_ack_transfer.dl_ngu_up_tnl_info = pdu_session.ng_dl_up_tnl_info;

      for (const auto& drb_setup_item : pdu_session.drb_setup_list_ng_ran) {
        // qos flow setup resp list
        for (const auto& flow_setup_item : drb_setup_item.flow_setup_list) {
          ngap_qos_flow_item_with_data_forwarding qos_flow_item;
          // qos flow id
          qos_flow_item.qos_flow_id = flow_setup_item.qos_flow_id;

          admitted_item.ho_request_ack_transfer.qos_flow_setup_resp_list.push_back(qos_flow_item);
        }

        // qos flow failed to setup list
        for (const auto& flow_failed_item : drb_setup_item.flow_failed_list) {
          cu_cp_qos_flow_with_cause_item qos_flow_item;
          // qos flow id
          qos_flow_item.qos_flow_id = flow_failed_item.qos_flow_id;
          // cause
          qos_flow_item.cause = flow_failed_item.cause;

          admitted_item.ho_request_ack_transfer.qos_flow_failed_to_setup_list.push_back(qos_flow_item);
        }

        // data forwarding resp drb list
        ngap_data_forwarding_resp_drb_item drb_item;
        drb_item.drb_id = drb_setup_item.drb_id;
        admitted_item.ho_request_ack_transfer.data_forwarding_resp_drb_list.push_back(drb_item);
      }

      response.pdu_session_res_admitted_list.emplace(admitted_item.pdu_session_id, admitted_item);
    }

    // pdu session res failed to setup list ho ack
    for (const auto& pdu_session : bearer_context_setup_response.pdu_session_resource_failed_list) {
      cu_cp_pdu_session_res_setup_failed_item failed_item;

      // pdu session id
      failed_item.pdu_session_id = pdu_session.pdu_session_id;

      // ngap_ho_res_alloc_unsuccessful_transfer
      // cause
      failed_item.unsuccessful_transfer.cause = cause_t::protocol;

      response.pdu_session_res_failed_to_setup_list_ho_ack.emplace(failed_item.pdu_session_id, failed_item);
    }
    // target to source transparent container
    response.target_to_source_transparent_container.rrc_container = rrc_reconfiguration.copy();
  } else {
    response.success  = false;
    response.ue_index = request.ue_index;
    response.cause    = cause_t::protocol;
  }

  return response;
}