/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/cu_cp/ue_task_scheduler.h"
#include "srsran/ran/cause/e1ap_cause_converters.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

// Free functions to amend to the final procedure response message. This will take the results from the various
// sub-procedures and update the succeeded/failed fields.

/// \brief Handle first Bearer Context Modification response and prepare subsequent UE context modification request.
bool handle_bearer_context_modification_response(
    cu_cp_pdu_session_resource_modify_response&      response_msg,
    f1ap_ue_context_modification_request&            ue_context_mod_request,
    const cu_cp_pdu_session_resource_modify_request& modify_request,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response,
    up_config_update&                                next_config,
    srslog::basic_logger&                            logger);

/// \brief Handle UE context modification response and prepare second Bearer Context Modification.
bool handle_ue_context_modification_response(
    cu_cp_pdu_session_resource_modify_response&      response_msg,
    e1ap_bearer_context_modification_request&        bearer_ctxt_mod_request,
    const cu_cp_pdu_session_resource_modify_request& modify_request,
    const f1ap_ue_context_modification_response&     ue_context_modification_response,
    const up_config_update&                          next_config,
    const srslog::basic_logger&                      logger);

/// \brief Handle RRC reconfiguration result.
bool handle_rrc_reconfiguration_response(cu_cp_pdu_session_resource_modify_response&      response_msg,
                                         const cu_cp_pdu_session_resource_modify_request& modify_request,
                                         bool                                             rrc_reconfig_result,
                                         const srslog::basic_logger&                      logger);

pdu_session_resource_modification_routine::pdu_session_resource_modification_routine(
    const cu_cp_pdu_session_resource_modify_request& modify_request_,
    e1ap_bearer_context_manager&                     e1ap_bearer_ctxt_mng_,
    f1ap_ue_context_manager&                         f1ap_ue_ctxt_mng_,
    rrc_ue_interface*                                rrc_ue_,
    cu_cp_rrc_ue_interface&                          cu_cp_notifier_,
    ue_task_scheduler&                               ue_task_sched_,
    up_resource_manager&                             up_resource_mng_,
    srslog::basic_logger&                            logger_) :
  modify_request(modify_request_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  f1ap_ue_ctxt_mng(f1ap_ue_ctxt_mng_),
  rrc_ue(rrc_ue_),
  cu_cp_notifier(cu_cp_notifier_),
  ue_task_sched(ue_task_sched_),
  up_resource_mng(up_resource_mng_),
  logger(logger_)
{
}

void pdu_session_resource_modification_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_modify_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized", modify_request.ue_index, name());

  // Perform initial sanity checks on incoming message.
  if (!up_resource_mng.validate_request(modify_request)) {
    logger.warning("ue={}: \"{}\" Invalid PduSessionResourceModification", modify_request.ue_index, name());
    CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
  }

  {
    // Calculate next user-plane configuration based on incoming modify message.
    next_config = up_resource_mng.calculate_update(modify_request);
  }

  {
    // Prepare first BearerContextModificationRequest.
    bearer_context_modification_request.ng_ran_bearer_context_mod_request.emplace(); // initialize fresh message
    fill_initial_e1ap_bearer_context_modification_request(bearer_context_modification_request);

    // Call E1AP procedure and wait for BearerContextModificationResponse.
    CORO_AWAIT_VALUE(
        bearer_context_modification_response,
        e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse and fill subsequent UEContextModificationRequest.
    if (!handle_bearer_context_modification_response(response_msg,
                                                     ue_context_mod_request,
                                                     modify_request,
                                                     bearer_context_modification_response,
                                                     next_config,
                                                     logger)) {
      logger.warning("ue={}: \"{}\" failed to modify bearer at CU-UP", modify_request.ue_index, name());
      CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
    }
  }

  {
    // Prepare UEContextModificationRequest and call F1 notifier.
    ue_context_mod_request.ue_index = modify_request.ue_index;
    ue_context_mod_request.cu_to_du_rrc_info.emplace();
    ue_context_mod_request.cu_to_du_rrc_info->meas_cfg = rrc_ue->get_packed_meas_config();

    CORO_AWAIT_VALUE(ue_context_modification_response,
                     f1ap_ue_ctxt_mng.handle_ue_context_modification_request(ue_context_mod_request));

    // Handle UEContextModificationResponse.
    if (!handle_ue_context_modification_response(response_msg,
                                                 bearer_context_modification_request,
                                                 modify_request,
                                                 ue_context_modification_response,
                                                 next_config,
                                                 logger)) {
      logger.warning("ue={}: \"{}\" failed to modify UE context at DU", modify_request.ue_index, name());
      CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
    }
  }

  // If needed, inform CU-UP about the new TEID for UL F1u traffic.
  if (bearer_context_modification_request.ng_ran_bearer_context_mod_request.has_value()) {
    // Add remaining fields to BearerContextModificationRequest.
    bearer_context_modification_request.ue_index = modify_request.ue_index;

    // Call E1AP procedure and wait for BearerContextModificationResponse.
    CORO_AWAIT_VALUE(
        bearer_context_modification_response,
        e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse.
    if (!handle_bearer_context_modification_response(response_msg,
                                                     ue_context_mod_request,
                                                     modify_request,
                                                     bearer_context_modification_response,
                                                     next_config,
                                                     logger)) {
      logger.warning("ue={}: \"{}\" failed to modify bearer at CU-UP", modify_request.ue_index, name());
      CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
    }
  }

  {
    // Prepare RRCReconfiguration and call RRC UE notifier.
    {
      // Get NAS PDUs as received by AMF.
      std::vector<byte_buffer> nas_pdus;
      for (const auto& pdu_session : modify_request.pdu_session_res_modify_items) {
        if (!pdu_session.nas_pdu.empty()) {
          nas_pdus.push_back(pdu_session.nas_pdu);
        }
      }

      if (!fill_rrc_reconfig_args(rrc_reconfig_args,
                                  {},
                                  next_config.pdu_sessions_to_modify_list,
                                  {} /* No extra DRB to be removed */,
                                  ue_context_modification_response.du_to_cu_rrc_info,
                                  nas_pdus,
                                  rrc_ue->generate_meas_config(),
                                  false,
                                  false,
                                  std::nullopt,
                                  {},
                                  std::nullopt,
                                  logger)) {
        logger.warning("ue={}: \"{}\" Failed to fill RrcReconfiguration", modify_request.ue_index, name());
        CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
      }
    }

    CORO_AWAIT_VALUE(rrc_reconfig_result, rrc_ue->handle_rrc_reconfiguration_request(rrc_reconfig_args));

    // Handle RRCReconfiguration result.
    if (!handle_rrc_reconfiguration_response(response_msg, modify_request, rrc_reconfig_result, logger)) {
      logger.warning("ue={}: \"{}\" RRC reconfiguration failed", modify_request.ue_index, name());
      // Notify NGAP to request UE context release from AMF.
      ue_task_sched.schedule_async_task(cu_cp_notifier.handle_ue_context_release(
          {modify_request.ue_index, {}, ngap_cause_radio_network_t::release_due_to_ngran_generated_reason}));
      CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
    }
  }

  // We are done.
  CORO_RETURN(generate_pdu_session_resource_modify_response(true));
}

static void fill_e1ap_pdu_session_res_to_modify_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_modify_item>& pdu_session_res_to_modify_list,
    srslog::basic_logger&                                                     logger,
    const up_config_update&                                                   next_config,
    const cu_cp_pdu_session_resource_modify_request&                          modify_request)
{
  for (const auto& modify_item : next_config.pdu_sessions_to_modify_list) {
    const auto& session = modify_item.second;
    srsran_assert(modify_request.pdu_session_res_modify_items.contains(session.id),
                  "Modify request doesn't contain config for {}",
                  session.id);

    // Obtain PDU session config from original resource modify request.
    const auto&                         pdu_session_cfg = modify_request.pdu_session_res_modify_items[session.id];
    e1ap_pdu_session_res_to_modify_item e1ap_pdu_session_item;
    e1ap_pdu_session_item.pdu_session_id = session.id;
    fill_drb_to_setup_list(e1ap_pdu_session_item.drb_to_setup_list_ng_ran,
                           pdu_session_cfg.transfer.qos_flow_add_or_modify_request_list,
                           session.drb_to_add,
                           logger);

    fill_drb_to_modify_list(e1ap_pdu_session_item.drb_to_modify_list_ng_ran,
                            pdu_session_cfg.transfer.qos_flow_add_or_modify_request_list,
                            session.drb_to_modify,
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

/// \brief Processes the result of a Bearer Context Modification Result's PDU session modify list.
/// \param[out] ngap_response_list Reference to the final NGAP response.
/// \param[out] ue_context_mod_request Reference to the next request message - a UE context modification.
/// \param[in] ngap_modify_list Const reference to the original NGAP request.
/// \param[in] bearer_context_modification_response Const reference to the response of the previous subprocedure.
/// \param[in] next_config Const reference to the calculated config update.
/// \param[in] logger Reference to the logger.
/// \return True on success, false otherwise.
static bool update_modify_list_with_bearer_ctxt_mod_response(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_modify_response_item>& ngap_response_list,
    f1ap_ue_context_modification_request&                                                 ue_context_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req>& ngap_modify_list,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response,
    up_config_update&                                next_config,
    const srslog::basic_logger&                      logger)
{
  for (const auto& e1ap_item : bearer_context_modification_response.pdu_session_resource_modified_list) {
    const auto& psi = e1ap_item.pdu_session_id;
    // Sanity check - make sure this session ID is present in the original modify message.
    if (!ngap_modify_list.contains(psi)) {
      logger.warning("PduSessionResourceSetupRequest doesn't include setup for {}", psi);
      return false;
    }
    // Also check if PDU session is included in expected next configuration.
    if (next_config.pdu_sessions_to_modify_list.find(psi) == next_config.pdu_sessions_to_modify_list.end()) {
      logger.warning("Didn't expect modification for {}", psi);
      return false;
    }

    if (ngap_response_list.contains(psi)) {
      // Load existing response item from previous call.
      logger.debug("Amend to existing NGAP response item for {}", psi);
    } else {
      // Add empty new item.
      cu_cp_pdu_session_resource_modify_response_item new_item;
      new_item.pdu_session_id = psi;
      ngap_response_list.emplace(new_item.pdu_session_id, new_item);
      logger.debug("Insert new NGAP response item for {}", psi);
    }

    // Start/continue filling response item.
    cu_cp_pdu_session_resource_modify_response_item& ngap_item = ngap_response_list[psi];
    for (const auto& e1ap_drb_item : e1ap_item.drb_setup_list_ng_ran) {
      const auto& drb_id = e1ap_drb_item.drb_id;
      if (next_config.pdu_sessions_to_modify_list.at(psi).drb_to_add.find(drb_id) ==
          next_config.pdu_sessions_to_modify_list.at(psi).drb_to_add.end()) {
        logger.warning("{} not part of next configuration", drb_id);
        return false;
      }

      const auto& request_transfer = ngap_modify_list[psi].transfer;

      //  Prepare DRB creation at DU.
      f1ap_drb_to_setup drb_setup_mod_item;
      if (!fill_f1ap_drb_setup_mod_item(drb_setup_mod_item,
                                        nullptr,
                                        psi,
                                        drb_id,
                                        next_config.pdu_sessions_to_modify_list.at(psi).drb_to_add.at(drb_id),
                                        e1ap_drb_item,
                                        request_transfer.qos_flow_add_or_modify_request_list,
                                        logger)) {
        logger.warning("Couldn't populate DRB setup/mod item {}", e1ap_drb_item.drb_id);
        return false;
      }

      // Note: this extra handling for the Modification could be optimized.
      for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
        // Fill added flows in NGAP response transfer.
        if (!ngap_item.transfer.qos_flow_add_or_modify_response_list.has_value()) {
          // Add list if it's not present yet.
          ngap_item.transfer.qos_flow_add_or_modify_response_list.emplace();
        }

        qos_flow_add_or_mod_response_item qos_flow;
        qos_flow.qos_flow_id = e1ap_flow.qos_flow_id;
        ngap_item.transfer.qos_flow_add_or_modify_response_list.value().emplace(qos_flow.qos_flow_id, qos_flow);
      }

      // Finally add DRB to setup to UE context modification.
      ue_context_mod_request.drbs_to_be_setup_mod_list.push_back(drb_setup_mod_item);
    }

    // Add DRB to be removed to UE context modification.
    for (const auto& drb_id : next_config.pdu_sessions_to_modify_list.at(psi).drb_to_remove) {
      ue_context_mod_request.drbs_to_be_released_list.push_back(drb_id);
    }

    // Fail on any DRB that fails to be setup.
    if (!e1ap_item.drb_failed_list_ng_ran.empty()) {
      logger.warning("Non-empty DRB failed list not supported");
      return false;
    }
  }

  return true;
}

/// \brief Processes the response of a Bearer Context Modification Request.
/// \param[out] response_msg Reference to the final NGAP response.
/// \param[out] next_config Const reference to the calculated config update.
/// \param[in] pdu_session_resource_failed_list Const reference to the failed PDU sessions of the Bearer Context
/// Modification Response.
static void update_failed_list_with_bearer_ctxt_mod_response(
    cu_cp_pdu_session_resource_modify_response&                                       response_msg,
    up_config_update&                                                                 next_config,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>& pdu_session_resource_failed_list)
{
  for (const auto& e1ap_item : pdu_session_resource_failed_list) {
    // Remove from next config.
    next_config.pdu_sessions_to_setup_list.erase(e1ap_item.pdu_session_id);
    response_msg.pdu_session_res_modify_list.erase(e1ap_item.pdu_session_id);

    // Add to list taking cause received from CU-UP.
    cu_cp_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id              = e1ap_item.pdu_session_id;
    failed_item.unsuccessful_transfer.cause = e1ap_to_ngap_cause(e1ap_item.cause);
    response_msg.pdu_session_res_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
  }
}

// \brief Handle first BearerContextModificationResponse and prepare subsequent UEContextModificationRequest.
bool handle_bearer_context_modification_response(
    cu_cp_pdu_session_resource_modify_response&      response_msg,
    f1ap_ue_context_modification_request&            ue_context_mod_request,
    const cu_cp_pdu_session_resource_modify_request& modify_request,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response,
    up_config_update&                                next_config,
    srslog::basic_logger&                            logger)
{
  // Traverse modify list.
  if (!update_modify_list_with_bearer_ctxt_mod_response(response_msg.pdu_session_res_modify_list,
                                                        ue_context_mod_request,
                                                        modify_request.pdu_session_res_modify_items,
                                                        bearer_context_modification_response,
                                                        next_config,
                                                        logger)) {
    return false;
  }

  // Traverse failed list.
  update_failed_list_with_bearer_ctxt_mod_response(
      response_msg, next_config, bearer_context_modification_response.pdu_session_resource_failed_list);

  return bearer_context_modification_response.success;
}

/// \brief Processes the response of a UE Context Modification Request.
/// \param[out] ngap_response_list Reference to the final NGAP response.
/// \param[out] ue_context_mod_request Reference to the next request message - a Bearer context modification request.
/// \param[in] ngap_modify_list Const reference to the original NGAP request.
/// \param[in] ue_context_modification_response Const reference to the response of the UE context modification request.
/// \param[in] next_config Const reference to the calculated config update.
/// \param[in] logger Reference to the logger.
/// \return True on success, false otherwise.
static bool update_modify_list_with_ue_ctxt_mod_response(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_modify_response_item>& ngap_response_list,
    e1ap_bearer_context_modification_request&                                             bearer_ctxt_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req>& ngap_modify_list,
    const f1ap_ue_context_modification_response& ue_context_modification_response,
    const up_config_update&                      next_config,
    const srslog::basic_logger&                  logger)
{
  // Fail procedure if (single) DRB couldn't be setup.
  if (!ue_context_modification_response.drbs_failed_to_be_setup_list.empty()) {
    logger.warning("Couldn't setup {} DRBs at DU",
                   ue_context_modification_response.drbs_failed_to_be_setup_list.size());
    return false;
  }

  // Only prepare bearer context modification request if needed.
  if (ue_context_modification_response.drbs_setup_list.empty() and
      ue_context_modification_response.drbs_modified_list.empty()) {
    // No DRB added or updated.
    logger.debug("Skipping preparation of bearer context modification request");
    bearer_ctxt_mod_request.ng_ran_bearer_context_mod_request.reset();
    return ue_context_modification_response.success;
  }

  // Start with empty message.
  e1ap_ng_ran_bearer_context_mod_request& e1ap_bearer_context_mod =
      bearer_ctxt_mod_request.ng_ran_bearer_context_mod_request.emplace();

  fill_e1ap_bearer_context_list(e1ap_bearer_context_mod.pdu_session_res_to_modify_list,
                                ue_context_modification_response.drbs_setup_list,
                                next_config.pdu_sessions_to_modify_list);

  // TODO: traverse other fields

  return ue_context_modification_response.success;
}

// Handle UEContextModificationResponse and prepare second BearerContextModificationRequest.
bool handle_ue_context_modification_response(
    cu_cp_pdu_session_resource_modify_response&      response_msg,
    e1ap_bearer_context_modification_request&        bearer_ctxt_mod_request,
    const cu_cp_pdu_session_resource_modify_request& modify_request,
    const f1ap_ue_context_modification_response&     ue_context_modification_response,
    const up_config_update&                          next_config,
    const srslog::basic_logger&                      logger)
{
  // Traverse modify list.
  if (!update_modify_list_with_ue_ctxt_mod_response(response_msg.pdu_session_res_modify_list,
                                                    bearer_ctxt_mod_request,
                                                    modify_request.pdu_session_res_modify_items,
                                                    ue_context_modification_response,
                                                    next_config,
                                                    logger)) {
    return false;
  }

  return ue_context_modification_response.success;
}

// Helper function to fail all requested PDU session.
static void fill_modify_failed_list(cu_cp_pdu_session_resource_modify_response&      response_msg,
                                    const cu_cp_pdu_session_resource_modify_request& modify_request)
{
  for (const auto& item : modify_request.pdu_session_res_modify_items) {
    cu_cp_pdu_session_resource_failed_to_modify_item failed_item;
    failed_item.pdu_session_id              = item.pdu_session_id;
    failed_item.unsuccessful_transfer.cause = ngap_cause_misc_t::unspecified;
    response_msg.pdu_session_res_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
  }
}

// Handle RRCReconfiguration result.
bool handle_rrc_reconfiguration_response(cu_cp_pdu_session_resource_modify_response&      response_msg,
                                         const cu_cp_pdu_session_resource_modify_request& modify_request,
                                         bool                                             rrc_reconfig_result,
                                         const srslog::basic_logger&                      logger)
{
  // Let all PDU sessions fail if response is negative.
  if (!rrc_reconfig_result) {
    fill_modify_failed_list(response_msg, modify_request);
  }

  return rrc_reconfig_result;
}

// Helper to mark all PDU sessions that were requested to be set up as failed.
static void mark_all_sessions_as_failed(cu_cp_pdu_session_resource_modify_response&      response_msg,
                                        const cu_cp_pdu_session_resource_modify_request& modify_request)
{
  for (const auto& modify_item : modify_request.pdu_session_res_modify_items) {
    cu_cp_pdu_session_resource_failed_to_modify_item failed_item;
    failed_item.pdu_session_id              = modify_item.pdu_session_id;
    failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::unspecified;
    response_msg.pdu_session_res_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
  }
  // No PDU session modified can be successful at the same time.
  response_msg.pdu_session_res_modify_list.clear();
}

cu_cp_pdu_session_resource_modify_response
pdu_session_resource_modification_routine::generate_pdu_session_resource_modify_response(bool success)
{
  if (success) {
    logger.debug("ue={}: \"{}\" finalized", modify_request.ue_index, name());

    // Prepare update for UP resource manager.
    up_config_update_result result;
    for (const auto& pdu_session_to_mod : next_config.pdu_sessions_to_modify_list) {
      result.pdu_sessions_modified_list.push_back(pdu_session_to_mod.second);
    }
    up_resource_mng.apply_config_update(result);

    for (const auto& psi : next_config.pdu_sessions_failed_to_modify_list) {
      cu_cp_pdu_session_resource_failed_to_modify_item failed_item;
      failed_item.pdu_session_id              = psi;
      failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::unspecified;
      response_msg.pdu_session_res_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
    }
  } else {
    logger.warning("ue={}: \"{}\" failed", modify_request.ue_index, name());
    mark_all_sessions_as_failed(response_msg, modify_request);
  }
  return response_msg;
}
