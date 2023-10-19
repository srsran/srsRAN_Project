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

#include "inter_cu_handover_target_routine.h"
#include "../pdu_session_routine_helpers.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/ran/cause.h"

using namespace srsran;
using namespace srs_cu_cp;

// This free function takes the E1AP Bearer Setup Response and pre-fills the subsequent F1AP UE Context Setup Request to
// be send to the DU.
bool handle_procedure_response(f1ap_ue_context_setup_request& ue_context_setup_req,
                               const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_list,
                               const e1ap_bearer_context_setup_response& bearer_context_setup_resp,
                               up_config_update&                         next_config,
                               up_resource_manager&                      up_resource_mng,
                               srslog::basic_logger&                     logger);

// This free function takes the F1AP UE Context Setup Response and pre-fills the subsequent E1AP Bearer Context
// Modification to be send to the CU-UP.
bool handle_procedure_response(e1ap_bearer_context_modification_request& bearer_ctxt_mod_request,
                               const f1ap_ue_context_setup_response&     ue_context_setup_response,
                               const up_config_update&                   next_config,
                               const srslog::basic_logger&               logger);

inter_cu_handover_target_routine::inter_cu_handover_target_routine(
    const ngap_handover_request&           request_,
    du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notif_,
    du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
    du_processor_cu_cp_notifier&           cu_cp_notifier_,
    du_processor_ue_manager&               ue_manager_,
    const security_indication_t&           default_security_indication_,
    srslog::basic_logger&                  logger_) :
  request(request_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  cu_cp_notifier(cu_cp_notifier_),
  ue_manager(ue_manager_),
  logger(logger_),
  default_security_indication(default_security_indication_)
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
    CORO_EARLY_RETURN(generate_handover_resource_allocation_response(false));
  }

  {
    // Calculate next user-plane configuration based on incoming setup message.
    next_config = ue->get_up_resource_manager().calculate_update(request.pdu_session_res_setup_list_ho_req);
  }

  // Prepare E1AP Bearer Context Setup Request and call E1AP notifier
  {
    // Generate security keys for Bearer Context Setup Request (RRC UE is not created yet)
    if (!generate_security_keys()) {
      logger.error("ue={}: \"{}\" failed to generate security keys.", request.ue_index, name());
      CORO_EARLY_RETURN(generate_handover_resource_allocation_response(false));
    }

    fill_e1ap_bearer_context_setup_request();

    // Call E1AP procedure
    CORO_AWAIT_VALUE(bearer_context_setup_response,
                     e1ap_ctrl_notifier.on_bearer_context_setup_request(bearer_context_setup_request));

    // Handle Bearer Context Setup Response
    if (!handle_procedure_response(ue_context_setup_request,
                                   request.pdu_session_res_setup_list_ho_req,
                                   bearer_context_setup_response,
                                   next_config,
                                   ue->get_up_resource_manager(),
                                   logger)) {
      logger.error("ue={}: \"{}\" failed to setup bearer at CU-UP.", request.ue_index, name());
      CORO_EARLY_RETURN(generate_handover_resource_allocation_response(false));
    }
  }

  // Prepare F1AP UE Context Setup Request and call F1AP notifier
  {
    // Add remaining fields to UE Context Setup Request
    ue_context_setup_request.ue_index      = request.ue_index;
    ue_context_setup_request.sp_cell_id    = request.source_to_target_transparent_container.target_cell_id;
    ue_context_setup_request.serv_cell_idx = 0; // TODO: Remove hardcoded value
    ue_context_setup_request.cu_to_du_rrc_info.ie_exts.emplace();
    ue_context_setup_request.cu_to_du_rrc_info.ie_exts.value().ho_prep_info =
        request.source_to_target_transparent_container.rrc_container.copy();

    // Call F1AP procedure
    CORO_AWAIT_VALUE(ue_context_setup_response,
                     f1ap_ue_ctxt_notifier.on_ue_context_setup_request(ue_context_setup_request, true));
    // Handle UE Context Setup Response
    if (!handle_procedure_response(
            bearer_context_modification_request, ue_context_setup_response, next_config, logger)) {
      logger.error("ue={}: \"{}\" failed to setup UE context at DU.", request.ue_index, name());
      cu_cp_notifier.on_ue_removal_required(ue_context_setup_response.ue_index);
      CORO_EARLY_RETURN(generate_handover_resource_allocation_response(false));
    }
  }

  // Target UE object exists from this point on.

  // Setup SRB1 and initialize security context in RRC
  {
    create_srb1();

    if (!ue->get_rrc_ue_notifier().on_new_security_context(request.security_context)) {
      logger.error("ue={}: \"{}\" failed to setup security context at UE.", request.ue_index, name());
      CORO_EARLY_RETURN(generate_handover_resource_allocation_response(false));
    }
  }

  // Prepare Bearer Context Modification Request and call E1AP notifier
  {
    // Add remaining fields to Bearer Context Modification Request
    bearer_context_modification_request.ue_index = request.ue_index;

    // Call E1AP procedure
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (!bearer_context_modification_response.success) {
      logger.error("ue={}: \"{}\" failed to modify bearer at CU-UP.", request.ue_index, name());
      CORO_EARLY_RETURN(generate_handover_resource_allocation_response(false));
    }
  }

  // Prepare RRC Reconfiguration and call RRC UE notifier
  // If default DRB is being setup, SRB2 needs to be setup as well
  {
    fill_rrc_reconfig_args(rrc_reconfig_args,
                           ue_context_setup_request.srbs_to_be_setup_list,
                           next_config.pdu_sessions_to_setup_list,
                           ue_context_setup_response.du_to_cu_rrc_info,
                           {} /* No NAS PDUs required */,
                           ue->get_rrc_ue_notifier().get_rrc_ue_meas_config());

    // Define transaction ID
    // We set this to zero, as only in the inter CU Handover case, the first RRC transaction of the target UE is an RRC
    // Reconfiguration. When the RRC Reconfig Complete with a transaction ID = 0 is received, we will notify the NGAP to
    // trigger a HandoverNotify message.
    unsigned transaction_id = 0;

    // Get RRC Handover Command container
    rrc_reconfiguration = ue->get_rrc_ue_notifier().on_rrc_handover_command_required(rrc_reconfig_args, transaction_id);
  }

  CORO_RETURN(generate_handover_resource_allocation_response(true));
}

// Same as above but taking the result from E1AP Bearer Context Setup message
bool handle_procedure_response(f1ap_ue_context_setup_request& ue_context_setup_req,
                               const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_list,
                               const e1ap_bearer_context_setup_response& bearer_context_setup_resp,
                               up_config_update&                         next_config,
                               up_resource_manager&                      up_resource_mng,
                               srslog::basic_logger&                     logger)
{
  // Traverse setup list
  if (!update_setup_list(ue_context_setup_req.srbs_to_be_setup_list,
                         ue_context_setup_req.drbs_to_be_setup_list,
                         setup_list,
                         bearer_context_setup_resp.pdu_session_resource_setup_list,
                         next_config,
                         up_resource_mng,
                         logger)) {
    return false;
  }

  return bearer_context_setup_resp.success;
}

bool handle_procedure_response(e1ap_bearer_context_modification_request& bearer_ctxt_mod_request,
                               const f1ap_ue_context_setup_response&     ue_context_setup_resp,
                               const up_config_update&                   next_config,
                               const srslog::basic_logger&               logger)
{
  // Fail procedure if (single) DRB couldn't be setup
  if (!ue_context_setup_resp.drbs_failed_to_be_setup_list.empty()) {
    logger.error("Couldn't setup {} DRBs at DU.", ue_context_setup_resp.drbs_failed_to_be_setup_list.size());
    return false;
  }

  if (!update_setup_list(bearer_ctxt_mod_request, ue_context_setup_resp.drbs_setup_list, next_config, logger)) {
    return false;
  }

  // TODO: traverse other fields

  return ue_context_setup_resp.success;
}

bool inter_cu_handover_target_routine::generate_security_keys()
{
  // Copy security context to RRC UE context
  sec_context = request.security_context;

  // Select preferred integrity algorithm.
  security::preferred_integrity_algorithms inc_algo_pref_list  = {security::integrity_algorithm::nia2,
                                                                  security::integrity_algorithm::nia1,
                                                                  security::integrity_algorithm::nia3,
                                                                  security::integrity_algorithm::nia0};
  security::preferred_ciphering_algorithms ciph_algo_pref_list = {security::ciphering_algorithm::nea0,
                                                                  security::ciphering_algorithm::nea2,
                                                                  security::ciphering_algorithm::nea1,
                                                                  security::ciphering_algorithm::nea3};
  if (not sec_context.select_algorithms(inc_algo_pref_list, ciph_algo_pref_list)) {
    logger.error("ue={} could not select security algorithm.", request.ue_index);
    return false;
  }
  logger.debug("ue={} selected security algorithms NIA=NIA{} NEA=NEA{}.",
               request.ue_index,
               sec_context.sel_algos.integ_algo,
               sec_context.sel_algos.cipher_algo);

  // Generate K_rrc_enc and K_rrc_int
  sec_context.generate_as_keys();

  security_cfg = sec_context.get_as_config(security::sec_domain::rrc);

  return true;
}

void inter_cu_handover_target_routine::fill_e1ap_bearer_context_setup_request()
{
  bearer_context_setup_request.ue_index = request.ue_index;

  // security info
  bearer_context_setup_request.security_info.security_algorithm.ciphering_algo = security_cfg.cipher_algo;
  bearer_context_setup_request.security_info.security_algorithm.integrity_protection_algorithm =
      security_cfg.integ_algo;
  bearer_context_setup_request.security_info.up_security_key.encryption_key = security_cfg.k_enc;
  if (security_cfg.k_int.has_value()) {
    bearer_context_setup_request.security_info.up_security_key.integrity_protection_key = security_cfg.k_int.value();
  }

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
                                          ue_manager.get_ue_config(),
                                          default_security_indication);
}

void inter_cu_handover_target_routine::create_srb1()
{
  // create SRB1
  srb_creation_message srb1_msg{};
  srb1_msg.ue_index = request.ue_index;
  srb1_msg.srb_id   = srb_id_t::srb1;
  srb1_msg.pdcp_cfg = asn1::rrc_nr::pdcp_cfg_s{};
  ue_manager.find_du_ue(request.ue_index)->get_rrc_ue_srb_notifier().create_srb(srb1_msg);
}

ngap_handover_resource_allocation_response
inter_cu_handover_target_routine::generate_handover_resource_allocation_response(bool success)
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
      failed_item.unsuccessful_transfer.cause = cause_protocol_t::unspecified;

      response.pdu_session_res_failed_to_setup_list_ho_ack.emplace(failed_item.pdu_session_id, failed_item);
    }
    // target to source transparent container
    response.target_to_source_transparent_container.rrc_container = rrc_reconfiguration.copy();
  } else {
    response.success  = false;
    response.ue_index = request.ue_index;
    response.cause    = cause_protocol_t::unspecified;
  }

  return response;
}
