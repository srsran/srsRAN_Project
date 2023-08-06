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

#include "pdu_session_resource_setup_routine.h"
#include "pdu_session_routine_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

// Free function to amend to the final procedure response message. This will take the results from the various
// sub-procedures and update the succeeded/failed fields.
bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&       response_msg,
                               f1ap_ue_context_modification_request&            ue_context_mod_request,
                               const cu_cp_pdu_session_resource_setup_request   setup_msg,
                               const e1ap_bearer_context_modification_response& bearer_context_modification_response,
                               up_config_update&                                next_config,
                               up_resource_manager&                             rrc_ue_up_resource_manager_,
                               srslog::basic_logger&                            logger);

// Same as above but taking the result from E1AP Bearer Context Setup message
bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                               f1ap_ue_context_modification_request&           ue_context_mod_request,
                               const cu_cp_pdu_session_resource_setup_request& setup_msg,
                               const e1ap_bearer_context_setup_response&       bearer_context_setup_response,
                               up_config_update&                               next_config,
                               up_resource_manager&                            rrc_ue_up_resource_manager_,
                               srslog::basic_logger&                           logger);

// This method takes the F1AP UE Context Modification Response message and pre-fills the subsequent
// bearer context modification message to be send to the CU-UP.
// In case of a negative outcome it also prefills the final PDU session resource setup respone message.
bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                               e1ap_bearer_context_modification_request&       bearer_ctxt_mod_request,
                               const cu_cp_pdu_session_resource_setup_request& setup_msg,
                               const f1ap_ue_context_modification_response&    ue_context_modification_response,
                               const up_config_update&                         next_config,
                               const srslog::basic_logger&                     logger);

bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                               const cu_cp_pdu_session_resource_setup_request& setup_msg,
                               bool                                            rrc_reconfig_result,
                               const srslog::basic_logger&                     logger);

pdu_session_resource_setup_routine::pdu_session_resource_setup_routine(
    const cu_cp_pdu_session_resource_setup_request& setup_msg_,
    const ue_configuration&                         ue_cfg_,
    const srsran::security::sec_as_config&          security_cfg_,
    du_processor_e1ap_control_notifier&             e1ap_ctrl_notif_,
    du_processor_f1ap_ue_context_notifier&          f1ap_ue_ctxt_notif_,
    du_processor_rrc_ue_control_message_notifier&   rrc_ue_notifier_,
    up_resource_manager&                            rrc_ue_up_resource_manager_,
    srslog::basic_logger&                           logger_) :
  setup_msg(setup_msg_),
  ue_cfg(ue_cfg_),
  security_cfg(security_cfg_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  rrc_ue_notifier(rrc_ue_notifier_),
  rrc_ue_up_resource_manager(rrc_ue_up_resource_manager_),
  logger(logger_)
{
}

void pdu_session_resource_setup_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", setup_msg.ue_index, name());

  // Perform initial sanity checks on incoming message.
  if (!rrc_ue_up_resource_manager.validate_request(setup_msg.pdu_session_res_setup_items)) {
    logger.error("ue={}: \"{}\" Invalid PDU Session Resource Setup", setup_msg.ue_index, name());
    CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
  }

  {
    CORO_AWAIT_VALUE(ue_capability_transfer_result,
                     rrc_ue_notifier.on_ue_capability_transfer_request(ue_capability_transfer_request));

    // Handle UE Capability Transfer result
    if (not ue_capability_transfer_result) {
      logger.error("ue={}: \"{}\" UE capability transfer failed", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  {
    // Calculate next user-plane configuration based on incoming setup message.
    next_config = rrc_ue_up_resource_manager.calculate_update(setup_msg.pdu_session_res_setup_items);
  }

  // sanity check passed, decide whether we have to create a Bearer Context at the CU-UP or modify an existing one.
  if (next_config.initial_context_creation) {
    // prepare BearerContextSetupRequest
    fill_e1ap_bearer_context_setup_request(bearer_context_setup_request);

    // call E1AP procedure
    CORO_AWAIT_VALUE(bearer_context_setup_response,
                     e1ap_ctrl_notifier.on_bearer_context_setup_request(bearer_context_setup_request));

    // Handle BearerContextSetupResponse
    if (!handle_procedure_response(response_msg,
                                   ue_context_mod_request,
                                   setup_msg,
                                   bearer_context_setup_response,
                                   next_config,
                                   rrc_ue_up_resource_manager,
                                   logger)) {
      logger.error("ue={}: \"{}\" failed to setup bearer at CU-UP.", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  } else {
    // prepare BearerContextModificationRequest and modify existing bearer
    bearer_context_modification_request.ng_ran_bearer_context_mod_request.emplace(); // initialize fresh message
    fill_initial_e1ap_bearer_context_modification_request(bearer_context_modification_request);

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (!handle_procedure_response(response_msg,
                                   ue_context_mod_request,
                                   setup_msg,
                                   bearer_context_modification_response,
                                   next_config,
                                   rrc_ue_up_resource_manager,
                                   logger)) {
      logger.error("ue={}: \"{}\" failed to modification bearer at CU-UP.", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // Register required SRB and DRB resources at DU
  {
    // prepare UE Context Modification Request and call F1 notifier
    ue_context_mod_request.ue_index = setup_msg.ue_index;

    // DRB setup have already added above.
    CORO_AWAIT_VALUE(ue_context_modification_response,
                     f1ap_ue_ctxt_notifier.on_ue_context_modification_request(ue_context_mod_request));

    // Handle UE Context Modification Response
    if (!handle_procedure_response(response_msg,
                                   bearer_context_modification_request,
                                   setup_msg,
                                   ue_context_modification_response,
                                   next_config,
                                   logger)) {
      logger.error("ue={}: \"{}\" failed to modify UE context at DU.", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // Inform CU-UP about the new TEID for UL F1u traffic
  {
    // add remaining fields to BearerContextModificationRequest
    bearer_context_modification_request.ue_index = setup_msg.ue_index;

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (!handle_procedure_response(response_msg,
                                   ue_context_mod_request,
                                   setup_msg,
                                   bearer_context_modification_response,
                                   next_config,
                                   rrc_ue_up_resource_manager,
                                   logger)) {
      logger.error("ue={}: \"{}\" failed to modification bearer at CU-UP.", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  {
    // prepare RRC Reconfiguration and call RRC UE notifier
    // if default DRB is being setup, SRB2 needs to be setup as well
    {
      // get NAS PDUs as received by AMF
      std::map<pdu_session_id_t, byte_buffer> nas_pdus;
      for (const auto& pdu_session : setup_msg.pdu_session_res_setup_items) {
        if (!pdu_session.pdu_session_nas_pdu.empty()) {
          nas_pdus.emplace(pdu_session.pdu_session_id, pdu_session.pdu_session_nas_pdu);
        }
      }

      fill_rrc_reconfig_args(rrc_reconfig_args,
                             ue_context_mod_request.srbs_to_be_setup_mod_list,
                             next_config.pdu_sessions_to_setup_list,
                             ue_context_modification_response.du_to_cu_rrc_info,
                             nas_pdus,
                             rrc_ue_notifier.get_rrc_ue_meas_config());
    }

    CORO_AWAIT_VALUE(rrc_reconfig_result, rrc_ue_notifier.on_rrc_reconfiguration_request(rrc_reconfig_args));

    // Handle UE Context Modification Response
    if (!handle_procedure_response(response_msg, setup_msg, rrc_reconfig_result, logger)) {
      logger.error("ue={}: \"{}\" RRC Reconfiguration failed.", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // we are done, all good
  CORO_RETURN(handle_pdu_session_resource_setup_result(true));
}

// Free function to amend to the final procedure response message. This will take the results from the various
// sub-procedures and update the succeeded/failed fields.
bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&       response_msg,
                               f1ap_ue_context_modification_request&            ue_context_mod_request,
                               const cu_cp_pdu_session_resource_setup_request   setup_msg,
                               const e1ap_bearer_context_modification_response& bearer_context_modification_response,
                               up_config_update&                                next_config,
                               up_resource_manager&                             rrc_ue_up_resource_manager_,
                               srslog::basic_logger&                            logger)
{
  // Traverse setup list
  if (!update_setup_list(response_msg.pdu_session_res_setup_response_items,
                         ue_context_mod_request,
                         setup_msg.pdu_session_res_setup_items,
                         bearer_context_modification_response.pdu_session_resource_setup_list,
                         next_config,
                         rrc_ue_up_resource_manager_,
                         logger)) {
    return false;
  }

  // Traverse failed list
  update_failed_list(response_msg.pdu_session_res_failed_to_setup_items,
                     bearer_context_modification_response.pdu_session_resource_failed_list);

  for (const auto& e1ap_item : bearer_context_modification_response.pdu_session_resource_modified_list) {
    // modified list
    logger.info("Implement handling of resource modified item {}.", e1ap_item.pdu_session_id);
  }

  for (const auto& e1ap_item : bearer_context_modification_response.pdu_session_resource_failed_to_modify_list) {
    // modified list
    logger.info("Implement handling of resource failed to modify item {}.", e1ap_item.pdu_session_id);
  }

  return bearer_context_modification_response.success;
}

// Same as above but taking the result from E1AP Bearer Context Setup message
bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                               f1ap_ue_context_modification_request&           ue_context_mod_request,
                               const cu_cp_pdu_session_resource_setup_request& setup_msg,
                               const e1ap_bearer_context_setup_response&       bearer_context_setup_response,
                               up_config_update&                               next_config,
                               up_resource_manager&                            rrc_ue_up_resource_manager_,
                               srslog::basic_logger&                           logger)
{
  // Traverse setup list
  if (!update_setup_list(response_msg.pdu_session_res_setup_response_items,
                         ue_context_mod_request,
                         setup_msg.pdu_session_res_setup_items,
                         bearer_context_setup_response.pdu_session_resource_setup_list,
                         next_config,
                         rrc_ue_up_resource_manager_,
                         logger)) {
    return false;
  }

  // Traverse failed list
  update_failed_list(response_msg.pdu_session_res_failed_to_setup_items,
                     bearer_context_setup_response.pdu_session_resource_failed_list);

  return bearer_context_setup_response.success;
}

// Helper function to fail all requested PDU session.
void fill_setup_failed_list(cu_cp_pdu_session_resource_setup_response&      response_msg,
                            const cu_cp_pdu_session_resource_setup_request& setup_msg)
{
  for (const auto& item : setup_msg.pdu_session_res_setup_items) {
    cu_cp_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id                                         = item.pdu_session_id;
    failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause = cause_t::misc;
    response_msg.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
  }
}

// This method takes the F1AP UE Context Modification Response message and pre-fills the subsequent
// bearer context modification message to be send to the CU-UP.
// In case of a negative outcome it also prefills the final PDU session resource setup respone message.
bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                               e1ap_bearer_context_modification_request&       bearer_ctxt_mod_request,
                               const cu_cp_pdu_session_resource_setup_request& setup_msg,
                               const f1ap_ue_context_modification_response&    ue_context_modification_response,
                               const up_config_update&                         next_config,
                               const srslog::basic_logger&                     logger)
{
  // Fail procedure if (single) DRB couldn't be setup
  if (!ue_context_modification_response.drbs_failed_to_be_setup_mod_list.empty()) {
    logger.error("Couldn't setup {} DRBs at DU.",
                 ue_context_modification_response.drbs_failed_to_be_setup_mod_list.size());
    return false;
  }

  if (!update_setup_list(response_msg.pdu_session_res_setup_response_items,
                         bearer_ctxt_mod_request,
                         setup_msg.pdu_session_res_setup_items,
                         ue_context_modification_response,
                         next_config,
                         logger)) {
    return false;
  }

  // Let all PDU sessions fail if response is negative.
  if (!ue_context_modification_response.success) {
    fill_setup_failed_list(response_msg, setup_msg);
  }

  // TODO: traverse other fields

  return ue_context_modification_response.success;
}

bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                               const cu_cp_pdu_session_resource_setup_request& setup_msg,
                               bool                                            rrc_reconfig_result,
                               const srslog::basic_logger&                     logger)
{
  // Let all PDU sessions fail if response is negative.
  if (!rrc_reconfig_result) {
    fill_setup_failed_list(response_msg, setup_msg);
  }

  return rrc_reconfig_result;
}

// Helper to mark all PDU sessions that were requested to be set up as failed.
void mark_all_sessions_as_failed(cu_cp_pdu_session_resource_setup_response&      response_msg,
                                 const cu_cp_pdu_session_resource_setup_request& setup_msg,
                                 cause_t                                         cause)
{
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item> failed_list;
  for (const auto& setup_item : setup_msg.pdu_session_res_setup_items) {
    e1ap_pdu_session_resource_failed_item fail_item;
    fail_item.pdu_session_id = setup_item.pdu_session_id;
    fail_item.cause          = cause;
    failed_list.emplace(setup_item.pdu_session_id, fail_item);
  }
  update_failed_list(response_msg.pdu_session_res_failed_to_setup_items, failed_list);
}

cu_cp_pdu_session_resource_setup_response
pdu_session_resource_setup_routine::handle_pdu_session_resource_setup_result(bool success)
{
  if (success) {
    logger.debug("ue={}: \"{}\" finalized.", setup_msg.ue_index, name());

    // Prepare update for UP resource manager.
    up_config_update_result result;
    for (const auto& pdu_session_to_add : next_config.pdu_sessions_to_setup_list) {
      result.pdu_sessions_added_list.push_back(pdu_session_to_add.second);
    }
    rrc_ue_up_resource_manager.apply_config_update(result);
  } else {
    logger.error("ue={}: \"{}\" failed.", setup_msg.ue_index, name());

    mark_all_sessions_as_failed(response_msg, setup_msg, cause_t::radio_network);
  }

  return response_msg;
}

void fill_e1ap_pdu_session_res_to_setup_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_setup_item>& pdu_session_res_to_setup_list,
    const srslog::basic_logger&                                              logger,
    const up_config_update&                                                  next_config,
    const cu_cp_pdu_session_resource_setup_request&                          setup_msg,
    const ue_configuration&                                                  ue_cfg)
{
  for (const auto& setup_item : next_config.pdu_sessions_to_setup_list) {
    const auto& session = setup_item.second;
    srsran_assert(setup_msg.pdu_session_res_setup_items.contains(session.id),
                  "Setup request doesn't contain config for PDU session id={}",
                  session.id);
    // Obtain PDU session config from original setup request.
    const auto&                        pdu_session_cfg = setup_msg.pdu_session_res_setup_items[session.id];
    e1ap_pdu_session_res_to_setup_item e1ap_pdu_session_item;

    e1ap_pdu_session_item.pdu_session_id    = pdu_session_cfg.pdu_session_id;
    e1ap_pdu_session_item.pdu_session_type  = pdu_session_cfg.pdu_session_type;
    e1ap_pdu_session_item.snssai            = pdu_session_cfg.s_nssai;
    e1ap_pdu_session_item.ng_ul_up_tnl_info = pdu_session_cfg.ul_ngu_up_tnl_info;

    if (pdu_session_cfg.security_ind.has_value()) {
      e1ap_pdu_session_item.security_ind = pdu_session_cfg.security_ind.value();
    } else {
      e1ap_pdu_session_item.security_ind = {};
    }

    // TODO: set `e1ap_pdu_session_item.pdu_session_inactivity_timer` if configured
    fill_drb_to_setup_list(e1ap_pdu_session_item.drb_to_setup_list_ng_ran,
                           pdu_session_cfg.qos_flow_setup_request_items,
                           session.drb_to_add,
                           logger);

    pdu_session_res_to_setup_list.emplace(e1ap_pdu_session_item.pdu_session_id, e1ap_pdu_session_item);
  }
}

void pdu_session_resource_setup_routine::fill_e1ap_bearer_context_setup_request(
    e1ap_bearer_context_setup_request& e1ap_request)
{
  e1ap_request.ue_index = setup_msg.ue_index;

  // security info
  e1ap_request.security_info.security_algorithm.ciphering_algo                 = security_cfg.cipher_algo;
  e1ap_request.security_info.security_algorithm.integrity_protection_algorithm = security_cfg.integ_algo;
  e1ap_request.security_info.up_security_key.encryption_key                    = security_cfg.k_enc;
  e1ap_request.security_info.up_security_key.integrity_protection_key          = security_cfg.k_int;

  e1ap_request.ue_dl_aggregate_maximum_bit_rate = setup_msg.ue_aggregate_maximum_bit_rate_dl;
  e1ap_request.serving_plmn                     = setup_msg.serving_plmn;
  e1ap_request.activity_notif_level             = "ue"; // TODO: Remove hardcoded value
  if (e1ap_request.activity_notif_level == "ue") {
    e1ap_request.ue_inactivity_timer = ue_cfg.inactivity_timer;
  }

  // Add new PDU sessions.
  fill_e1ap_pdu_session_res_to_setup_list(
      e1ap_request.pdu_session_res_to_setup_list, logger, next_config, setup_msg, ue_cfg);
}

// Helper to fill a Bearer Context Modification request if it is the initial E1AP message
// for this procedure.
void pdu_session_resource_setup_routine::fill_initial_e1ap_bearer_context_modification_request(
    e1ap_bearer_context_modification_request& e1ap_request)
{
  e1ap_request.ue_index = setup_msg.ue_index;

  // Start with a fresh message.
  e1ap_ng_ran_bearer_context_mod_request& e1ap_bearer_context_mod =
      e1ap_request.ng_ran_bearer_context_mod_request.emplace();

  // Add new PDU sessions.
  fill_e1ap_pdu_session_res_to_setup_list(
      e1ap_request.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_setup_mod_list,
      logger,
      next_config,
      setup_msg,
      ue_cfg);

  // Remove PDU sessions.
  for (const auto& pdu_session_id : next_config.pdu_sessions_to_remove_list) {
    e1ap_bearer_context_mod.pdu_session_res_to_rem_list.push_back(pdu_session_id);
  }
}
