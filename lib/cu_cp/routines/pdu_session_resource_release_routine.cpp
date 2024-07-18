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

#include "pdu_session_resource_release_routine.h"
#include "pdu_session_routine_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

pdu_session_resource_release_routine::pdu_session_resource_release_routine(
    const cu_cp_pdu_session_resource_release_command& release_cmd_,
    e1ap_bearer_context_manager&                      e1ap_bearer_ctxt_mng_,
    f1ap_ue_context_manager&                          f1ap_ue_ctxt_mng_,
    du_processor_rrc_ue_control_message_notifier&     rrc_ue_notifier_,
    cu_cp_rrc_ue_interface&                           cu_cp_notifier_,
    ue_task_scheduler&                                task_sched_,
    up_resource_manager&                              up_resource_mng_,
    srslog::basic_logger&                             logger_) :
  release_cmd(release_cmd_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  f1ap_ue_ctxt_mng(f1ap_ue_ctxt_mng_),
  rrc_ue_notifier(rrc_ue_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  task_sched(task_sched_),
  up_resource_mng(up_resource_mng_),
  logger(logger_)
{
}

// Handle RRC reconfiguration result.
bool handle_procedure_response(cu_cp_pdu_session_resource_release_response&      response_msg,
                               const cu_cp_pdu_session_resource_release_command& release_cmd,
                               bool                                              rrc_reconfig_result,
                               const srslog::basic_logger&                       logger)
{
  // Let all PDU sessions fail if response is negative.
  if (!rrc_reconfig_result) {
    // TODO: decide how to manage negative RRC reconfig result
  }

  return rrc_reconfig_result;
}

void pdu_session_resource_release_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_release_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized", release_cmd.ue_index, name());

  // Perform initial sanity checks on incoming message.
  if (!up_resource_mng.validate_request(release_cmd)) {
    logger.warning("ue={}: \"{}\" Invalid PduSessionResourceReleaseCommand", release_cmd.ue_index, name());
    CORO_EARLY_RETURN(handle_pdu_session_resource_release_response(false));
  }

  {
    // Calculate next user-plane configuration based on incoming release command.
    next_config = up_resource_mng.calculate_update(release_cmd);
  }

  // Inform the CU-UP about the release of the bearer context
  if (next_config.pdu_sessions_to_remove_list.size() == up_resource_mng.get_nof_pdu_sessions()) {
    bearer_context_release_command.ue_index = release_cmd.ue_index;
    bearer_context_release_command.cause    = e1ap_cause_radio_network_t::unspecified;

    /// NOTE: Only the Bearer Context at the CU-UP will be released. We don't want to release the UE.

    // Request BearerContextRelease
    CORO_AWAIT(e1ap_bearer_ctxt_mng.handle_bearer_context_release_command(bearer_context_release_command));

  } else { // Inform CU-UP about the release of a bearer

    // prepare BearerContextModificationRequest and call e1 notifier
    bearer_context_modification_request.ue_index = release_cmd.ue_index;

    for (const auto& pdu_session_res_to_release : next_config.pdu_sessions_to_remove_list) {
      e1ap_ng_ran_bearer_context_mod_request bearer_context_mod_request;
      bearer_context_mod_request.pdu_session_res_to_rem_list.push_back(pdu_session_res_to_release);
      bearer_context_modification_request.ng_ran_bearer_context_mod_request = bearer_context_mod_request;
    }

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(
        bearer_context_modification_response,
        e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (not bearer_context_modification_response.success) {
      logger.warning("ue={}: \"{}\" failed to release bearer(s) at CU-UP", release_cmd.ue_index, name());
    }
  }

  // Release DRB resources at DU
  {
    // prepare UeContextModificationRequest and call F1 notifier
    ue_context_mod_request.ue_index = release_cmd.ue_index;
    for (const auto& drb_id : next_config.drb_to_remove_list) {
      ue_context_mod_request.drbs_to_be_released_list.push_back(drb_id);
    }

    CORO_AWAIT_VALUE(ue_context_modification_response,
                     f1ap_ue_ctxt_mng.handle_ue_context_modification_request(ue_context_mod_request));

    // Handle UE Context Modification Response
    if (not ue_context_modification_response.success) {
      logger.warning("ue={}: \"{}\" failed to release bearer(s) at DU", release_cmd.ue_index, name());
    }
  }

  {
    // prepare RRC Reconfiguration and call RRC UE notifier
    {
      // get NAS PDUs as received by AMF
      std::vector<byte_buffer> nas_pdus;
      if (!release_cmd.nas_pdu.empty()) {
        nas_pdus.push_back(release_cmd.nas_pdu);
      }

      if (!fill_rrc_reconfig_args(rrc_reconfig_args,
                                  {},
                                  next_config.pdu_sessions_to_modify_list,
                                  next_config.drb_to_remove_list,
                                  ue_context_modification_response.du_to_cu_rrc_info,
                                  nas_pdus,
                                  rrc_ue_notifier.generate_meas_config(),
                                  false,
                                  false,
                                  false,
                                  {},
                                  logger)) {
        logger.warning("ue={}: \"{}\" Failed to fill RrcReconfiguration", release_cmd.ue_index, name());
        CORO_EARLY_RETURN(handle_pdu_session_resource_release_response(false));
      }
    }

    CORO_AWAIT_VALUE(rrc_reconfig_result, rrc_ue_notifier.on_rrc_reconfiguration_request(rrc_reconfig_args));

    // Handle RRC Reconfiguration result.
    if (not handle_procedure_response(response_msg, release_cmd, rrc_reconfig_result, logger)) {
      logger.warning("ue={}: \"{}\" RRC reconfiguration failed", release_cmd.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_release_response(false));
    }
  }

  CORO_RETURN(handle_pdu_session_resource_release_response(true));
}

cu_cp_pdu_session_resource_release_response
pdu_session_resource_release_routine::handle_pdu_session_resource_release_response(bool success)
{
  // Fill PDUSessionResponse with the released PDU sessions even in case of failure.
  for (const auto& setup_item : release_cmd.pdu_session_res_to_release_list_rel_cmd) {
    cu_cp_pdu_session_res_released_item_rel_res item;
    item.pdu_session_id = setup_item.pdu_session_id;

    response_msg.released_pdu_sessions.emplace(setup_item.pdu_session_id, item);
  }

  // Prepare update for UP resource manager.
  up_config_update_result result;
  for (const auto& pdu_session_to_remove : next_config.pdu_sessions_to_remove_list) {
    result.pdu_sessions_removed_list.push_back(pdu_session_to_remove);
  }
  up_resource_mng.apply_config_update(result);

  if (success) {
    logger.debug("ue={}: \"{}\" finalized", release_cmd.ue_index, name());
  } else {
    logger.info("ue={}: \"{}\" failed", release_cmd.ue_index, name());

    // Trigger UE context release request.
    task_sched.schedule_async_task(cu_cp_notifier.handle_ue_context_release(
        {release_cmd.ue_index, {}, ngap_cause_radio_network_t::radio_conn_with_ue_lost}));
  }

  return response_msg;
}
