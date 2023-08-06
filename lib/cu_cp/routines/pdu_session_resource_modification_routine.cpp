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

#include "pdu_session_resource_modification_routine.h"
#include "pdu_session_routine_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

pdu_session_resource_modification_routine::pdu_session_resource_modification_routine(
    const cu_cp_pdu_session_resource_modify_request& modify_request_,
    du_processor_e1ap_control_notifier&              e1ap_ctrl_notif_,
    du_processor_f1ap_ue_context_notifier&           f1ap_ue_ctxt_notif_,
    du_processor_rrc_ue_control_message_notifier&    rrc_ue_notifier_,
    up_resource_manager&                             rrc_ue_up_resource_manager_,
    srslog::basic_logger&                            logger_) :
  modify_request(modify_request_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  rrc_ue_notifier(rrc_ue_notifier_),
  rrc_ue_up_resource_manager(rrc_ue_up_resource_manager_),
  logger(logger_)
{
}

void fill_e1ap_pdu_session_res_to_modify_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_modify_item>& pdu_session_res_to_modify_list,
    srslog::basic_logger&                                                     logger,
    const up_config_update&                                                   next_config,
    const cu_cp_pdu_session_resource_modify_request&                          modify_request)
{
  for (const auto& modify_item : next_config.pdu_sessions_to_modify_list) {
    const auto& session = modify_item.second;
    srsran_assert(modify_request.pdu_session_res_modify_items.contains(session.id),
                  "Modify request doesn't contain config for PDU session id={}",
                  session.id);

    // Obtain PDU session config from original resource modify request.
    const auto&                         pdu_session_cfg = modify_request.pdu_session_res_modify_items[session.id];
    e1ap_pdu_session_res_to_modify_item e1ap_pdu_session_item;
    e1ap_pdu_session_item.pdu_session_id = session.id;
    fill_drb_to_setup_list(e1ap_pdu_session_item.drb_to_setup_list_ng_ran,
                           pdu_session_cfg.transfer.qos_flow_add_or_modify_request_list,
                           session.drb_to_add,
                           logger);

    fill_drb_to_remove_list(e1ap_pdu_session_item.drb_to_rem_list_ng_ran, session.drb_to_remove);

    pdu_session_res_to_modify_list.emplace(pdu_session_cfg.pdu_session_id, e1ap_pdu_session_item);
  }
}

// Helper to fill a Bearer Context Modification request if it is the initial E1AP message
// for this procedure.
void pdu_session_resource_modification_routine::fill_initial_e1ap_bearer_context_modification_request(
    e1ap_bearer_context_modification_request& e1ap_request)
{
  e1ap_request.ue_index = modify_request.ue_index;

  // Start with a fresh message.
  e1ap_ng_ran_bearer_context_mod_request& e1ap_bearer_context_mod =
      e1ap_request.ng_ran_bearer_context_mod_request.emplace();

  // Add PDU sessions to be modified.
  fill_e1ap_pdu_session_res_to_modify_list(
      e1ap_request.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list,
      logger,
      next_config,
      modify_request);

  // Remove PDU sessions.
  for (const auto& pdu_session_id : next_config.pdu_sessions_to_remove_list) {
    e1ap_bearer_context_mod.pdu_session_res_to_rem_list.push_back(pdu_session_id);
  }
}

// \brief Handle first Bearer Context Modifcation response and prepare subsequent UE context modification request.
bool handle_procedure_response(cu_cp_pdu_session_resource_modify_response&      response_msg,
                               f1ap_ue_context_modification_request&            ue_context_mod_request,
                               const cu_cp_pdu_session_resource_modify_request  modify_request,
                               const e1ap_bearer_context_modification_response& bearer_context_modification_response,
                               up_config_update&                                next_config,
                               srslog::basic_logger&                            logger)
{
  // Traverse modify list
  if (update_modify_list(response_msg.pdu_session_res_modify_list,
                         ue_context_mod_request,
                         modify_request.pdu_session_res_modify_items,
                         bearer_context_modification_response.pdu_session_resource_modified_list,
                         next_config,
                         logger) == false) {
    return false;
  }

  // Traverse failed list
  update_failed_list(response_msg.pdu_session_res_failed_to_modify_list,
                     bearer_context_modification_response.pdu_session_resource_failed_list);

  return bearer_context_modification_response.success;
}

// Handle UE context modifcation response and prepare second Bearer Context Modification.
bool handle_procedure_response(cu_cp_pdu_session_resource_modify_response&     response_msg,
                               e1ap_bearer_context_modification_request&       bearer_ctxt_mod_request,
                               const cu_cp_pdu_session_resource_modify_request modify_request,
                               const f1ap_ue_context_modification_response&    ue_context_modification_response,
                               const up_config_update&                         next_config,
                               const srslog::basic_logger&                     logger)
{
  // Traverse modify list
  if (update_modify_list(response_msg.pdu_session_res_modify_list,
                         bearer_ctxt_mod_request,
                         modify_request.pdu_session_res_modify_items,
                         ue_context_modification_response,
                         next_config,
                         logger) == false) {
    return false;
  }

  return ue_context_modification_response.success;
}

// Helper function to fail all requested PDU session.
void fill_modify_failed_list(cu_cp_pdu_session_resource_modify_response&      response_msg,
                             const cu_cp_pdu_session_resource_modify_request& modify_request)
{
  for (const auto& item : modify_request.pdu_session_res_modify_items) {
    cu_cp_pdu_session_resource_failed_to_modify_item failed_item;
    failed_item.pdu_session_id                                         = item.pdu_session_id;
    failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause = cause_t::misc;
    response_msg.pdu_session_res_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
  }
}

// Handle RRC reconfiguration result
bool handle_procedure_response(cu_cp_pdu_session_resource_modify_response&      response_msg,
                               const cu_cp_pdu_session_resource_modify_request& modify_request,
                               bool                                             rrc_reconfig_result,
                               const srslog::basic_logger&                      logger)
{
  // Let all PDU sessions fail if response is negative.
  if (rrc_reconfig_result == false) {
    fill_modify_failed_list(response_msg, modify_request);
  }

  return rrc_reconfig_result;
}

void pdu_session_resource_modification_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_modify_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", modify_request.ue_index, name());

  // Perform initial sanity checks on incoming message.
  if (!rrc_ue_up_resource_manager.validate_request(modify_request)) {
    logger.error("ue={}: \"{}\" Invalid PDU Session Resource Modification", modify_request.ue_index, name());
    CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
  }

  {
    // Calculate next user-plane configuration based on incoming modify message.
    next_config = rrc_ue_up_resource_manager.calculate_update(modify_request);
  }

  {
    // prepare first BearerContextModificationRequest
    bearer_context_modification_request.ng_ran_bearer_context_mod_request.emplace(); // initialize fresh message
    fill_initial_e1ap_bearer_context_modification_request(bearer_context_modification_request);

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse and fill subsequent UE context modification
    if (handle_procedure_response(response_msg,
                                  ue_context_mod_request,
                                  modify_request,
                                  bearer_context_modification_response,
                                  next_config,
                                  logger) == false) {
      logger.error("ue={}: \"{}\" failed to modify bearer at CU-UP.", modify_request.ue_index, name());
      CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
    }
  }

  {
    // prepare UE Context Modification Request and call F1 notifier
    ue_context_mod_request.ue_index = modify_request.ue_index;

    CORO_AWAIT_VALUE(ue_context_modification_response,
                     f1ap_ue_ctxt_notifier.on_ue_context_modification_request(ue_context_mod_request));

    // Handle UE Context Modification Response
    if (handle_procedure_response(response_msg,
                                  bearer_context_modification_request,
                                  modify_request,
                                  ue_context_modification_response,
                                  next_config,
                                  logger) == false) {
      logger.error("ue={}: \"{}\" failed to modify UE context at DU.", modify_request.ue_index, name());
      CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
    }
  }

  // If needed, inform CU-UP about the new TEID for UL F1u traffic
  if (bearer_context_modification_request.ng_ran_bearer_context_mod_request.has_value()) {
    // add remaining fields to BearerContextModificationRequest
    bearer_context_modification_request.ue_index = modify_request.ue_index;

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (handle_procedure_response(response_msg,
                                  ue_context_mod_request,
                                  modify_request,
                                  bearer_context_modification_response,
                                  next_config,
                                  logger) == false) {
      logger.error("ue={}: \"{}\" failed to modify bearer at CU-UP.", modify_request.ue_index, name());
      CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
    }
  }

  {
    // prepare RRC Reconfiguration and call RRC UE notifier
    {
      // get NAS PDUs as received by AMF
      std::map<pdu_session_id_t, byte_buffer> nas_pdus;
      for (const auto& pdu_session : modify_request.pdu_session_res_modify_items) {
        if (!pdu_session.nas_pdu.empty()) {
          nas_pdus.emplace(pdu_session.pdu_session_id, pdu_session.nas_pdu);
        }
      }

      fill_rrc_reconfig_args(rrc_reconfig_args,
                             {},
                             next_config.pdu_sessions_to_modify_list,
                             ue_context_modification_response.du_to_cu_rrc_info,
                             nas_pdus,
                             rrc_ue_notifier.get_rrc_ue_meas_config());
    }

    CORO_AWAIT_VALUE(rrc_reconfig_result, rrc_ue_notifier.on_rrc_reconfiguration_request(rrc_reconfig_args));

    // Handle RRC Reconfiguration result.
    if (handle_procedure_response(response_msg, modify_request, rrc_reconfig_result, logger) == false) {
      logger.error("ue={}: \"{}\" RRC Reconfiguration failed.", modify_request.ue_index, name());
      CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
    }
  }

  // we are done
  CORO_RETURN(generate_pdu_session_resource_modify_response(true));
}

// Helper to mark all PDU sessions that were requested to be set up as failed.
void mark_all_sessions_as_failed(cu_cp_pdu_session_resource_modify_response&      response_msg,
                                 const cu_cp_pdu_session_resource_modify_request& modify_request)
{
  for (const auto& modify_item : modify_request.pdu_session_res_modify_items) {
    cu_cp_pdu_session_resource_failed_to_modify_item failed_item;
    failed_item.pdu_session_id                                         = modify_item.pdu_session_id;
    failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause = cause_t::radio_network;
    response_msg.pdu_session_res_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
  }
}

cu_cp_pdu_session_resource_modify_response
pdu_session_resource_modification_routine::generate_pdu_session_resource_modify_response(bool success)
{
  if (success) {
    logger.debug("ue={}: \"{}\" finalized.", modify_request.ue_index, name());

    // Prepare update for UP resource manager.
    up_config_update_result result;
    for (const auto& pdu_session_to_mod : next_config.pdu_sessions_to_modify_list) {
      result.pdu_sessions_modified_list.push_back(pdu_session_to_mod.second);
    }
    rrc_ue_up_resource_manager.apply_config_update(result);

    for (const auto& psi : next_config.pdu_sessions_failed_to_modify_list) {
      cu_cp_pdu_session_resource_failed_to_modify_item failed_item;
      failed_item.pdu_session_id                                         = psi;
      failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause = cause_t::radio_network;
      response_msg.pdu_session_res_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
    }
  } else {
    logger.error("ue={}: \"{}\" failed.", modify_request.ue_index, name());
    mark_all_sessions_as_failed(response_msg, modify_request);
  }
  return response_msg;
}
