/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    up_resource_manager&                             rrc_ue_up_resource_manager_,
    srslog::basic_logger&                            logger_) :
  modify_request(modify_request_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
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

    // Setup new DRBs if needed
    for (const auto& drb_to_setup : session.drb_to_add) {
      e1ap_drb_to_setup_item_ng_ran e1ap_drb_setup_item;
      e1ap_drb_setup_item.drb_id = drb_to_setup.first;
      // TODO: set `e1ap_drb_setup_item.drb_inactivity_timer` if configured
      e1ap_drb_setup_item.sdap_cfg = drb_to_setup.second.sdap_cfg;
      fill_e1ap_drb_pdcp_config(e1ap_drb_setup_item.pdcp_cfg, drb_to_setup.second.pdcp_cfg);

      e1ap_cell_group_info_item e1ap_cell_group_item;
      e1ap_cell_group_item.cell_group_id = 0; // TODO: Remove hardcoded value
      e1ap_drb_setup_item.cell_group_info.push_back(e1ap_cell_group_item);

      for (const auto& request_item : pdu_session_cfg.transfer.qos_flow_add_or_modify_request_list) {
        e1ap_qos_flow_qos_param_item e1ap_qos_item;
        fill_e1ap_qos_flow_param_item(e1ap_qos_item, logger, request_item);
        e1ap_drb_setup_item.qos_flow_info_to_be_setup.emplace(e1ap_qos_item.qos_flow_id, e1ap_qos_item);
      }

      e1ap_pdu_session_item.drb_to_setup_list_ng_ran.emplace(e1ap_drb_setup_item.drb_id, e1ap_drb_setup_item);
    }

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

// \brief Helper function to amend the final PDU session resource modify response.
bool handle_procedure_response(cu_cp_pdu_session_resource_modify_response&      response_msg,
                               cu_cp_ue_context_modification_request&           ue_context_mod_request,
                               const cu_cp_pdu_session_resource_modify_request  modify_request,
                               const e1ap_bearer_context_modification_response& bearer_context_modification_response,
                               const up_config_update&                          next_config,
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

bool handle_procedure_response(cu_cp_pdu_session_resource_modify_response&     response_msg,
                               e1ap_bearer_context_modification_request&       bearer_ctxt_mod_request,
                               const cu_cp_pdu_session_resource_modify_request modify_request,
                               const cu_cp_ue_context_modification_response&   ue_context_modification_response,
                               const up_config_update&                         next_config,
                               srslog::basic_logger&                           logger)
{
  // TODO: add implementation
  return ue_context_modification_response.success;
}

void pdu_session_resource_modification_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_modify_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", modify_request.ue_index, name());

  // Perform initial sanity checks.
  if (modify_request.pdu_session_res_modify_items.empty()) {
    logger.info("ue={}: \"{}\" Skipping empty PDU Session Resource Modification", modify_request.ue_index, name());
    CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
  }

  for (const auto& modify_item : modify_request.pdu_session_res_modify_items) {
    if (!rrc_ue_up_resource_manager.has_pdu_session(modify_item.pdu_session_id)) {
      logger.error("ue={}: \"{}\" PDU session ID {} doesn't exist.",
                   modify_request.ue_index,
                   name(),
                   modify_item.pdu_session_id);
      CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
    }
  }

  {
    // Calculate next user-plane configuration based on incoming modify message.
    next_config = rrc_ue_up_resource_manager.calculate_update(modify_request);
  }

  {
    // prepare BearerContextModificationRequest
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
      logger.error("ue={}: \"{}\" failed to modifify bearer at CU-UP.", modify_request.ue_index, name());
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

  // we are done
  CORO_RETURN(generate_pdu_session_resource_modify_response(true));
}

// Helper to mark all PDU sessions that were requested to be set up as failed.
void mark_all_sessions_as_failed(cu_cp_pdu_session_resource_modify_response&      response_msg,
                                 const cu_cp_pdu_session_resource_modify_request& modify_request)
{
  for (const auto& modify_item : modify_request.pdu_session_res_modify_items) {
    cu_cp_pdu_session_resource_failed_to_modify_item failed_item;
    failed_item.pdu_session_id = modify_item.pdu_session_id;
    response_msg.pdu_session_res_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
  }
}

cu_cp_pdu_session_resource_modify_response
pdu_session_resource_modification_routine::generate_pdu_session_resource_modify_response(bool success)
{
  if (success) {
    logger.debug("ue={}: \"{}\" finalized.", modify_request.ue_index, name());

    // TODO: Prepare update for UP resource manager.
    up_config_update_result result;
    rrc_ue_up_resource_manager.apply_config_update(result);
  } else {
    logger.error("ue={}: \"{}\" failed.", modify_request.ue_index, name());
    mark_all_sessions_as_failed(response_msg, modify_request);
  }
  return response_msg;
}
