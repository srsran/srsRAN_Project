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

#include "inter_du_handover_routine.h"
#include "../pdu_session_routine_helpers.h"
#include "handover_reconfiguration_routine.h"
#include "mobility_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

bool verify_ho_command(const cu_cp_inter_du_handover_request& command,
                       du_processor_ue_manager&               ue_manager,
                       const srslog::basic_logger&            logger)
{
  if (command.target_pci == INVALID_PCI) {
    logger.error("Target PCI must not be invalid");
    return false;
  }

  if (command.target_du_index == du_index_t::invalid) {
    logger.error("Target DU index must not be invalid");
    return false;
  }

  if (command.source_ue_index == ue_index_t::invalid) {
    logger.error("Source UE index must not be invalid");
    return false;
  }

  if (!ue_manager.find_du_ue(command.source_ue_index)) {
    logger.error("Can't find source UE index {}");
    return false;
  }

  return true;
}

inter_du_handover_routine::inter_du_handover_routine(
    const cu_cp_inter_du_handover_request& command_,
    du_processor_cu_cp_notifier&           cu_cp_notifier_,
    du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notif_,
    du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
    du_processor_ue_context_notifier&      source_du_processor_notifier_,
    du_processor_ue_context_notifier&      target_du_processor_notifier_,
    du_processor_ue_manager&               ue_manager_,
    srslog::basic_logger&                  logger_) :
  command(command_),
  cu_cp_notifier(cu_cp_notifier_),
  target_du_f1ap_ue_ctxt_notifier(target_du_f1ap_ue_ctxt_notif_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  source_du_processor_notifier(source_du_processor_notifier_),
  target_du_processor_notifier(target_du_processor_notifier_),
  ue_manager(ue_manager_),
  logger(logger_)
{
}

void inter_du_handover_routine::operator()(coro_context<async_task<cu_cp_inter_du_handover_response>>& ctx)
{
  CORO_BEGIN(ctx);

  {
    // Verify input.
    if (!verify_ho_command(command, ue_manager, logger)) {
      logger.error("ue={}: \"{}\" - invalid input parameters", command.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    // Retrieve source UE context.
    source_ue          = ue_manager.find_du_ue(command.source_ue_index);
    source_rrc_context = source_ue->get_rrc_ue_notifier().get_transfer_context();
    next_config        = to_config_update(source_rrc_context.up_ctx);
  }

  logger.debug("ue={}: \"{}\" initialized", command.source_ue_index, name());

  {
    // Allocate UE index at target DU
    target_ue_context_setup_request.ue_index = ue_manager.allocate_new_ue_index(command.target_du_index);
    if (target_ue_context_setup_request.ue_index == ue_index_t::invalid) {
      logger.error("ue={}: \"{}\" failed to allocate UE index at target DU", command.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    // prepare F1AP UE Context Setup Command and call F1AP notifier of target DU
    if (!generate_ue_context_setup_request(
            target_ue_context_setup_request, source_ue->get_rrc_ue_srb_notifier().get_srbs(), source_rrc_context)) {
      logger.error("ue={}: \"{}\" failed to generate UE context setup request", command.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    CORO_AWAIT_VALUE(target_ue_context_setup_response,
                     target_du_f1ap_ue_ctxt_notifier.on_ue_context_setup_request(target_ue_context_setup_request,
                                                                                 source_rrc_context));

    // Handle UE Context Setup Response
    if (!handle_context_setup_response(response_msg,
                                       bearer_context_modification_request,
                                       target_ue_context_setup_response,
                                       next_config,
                                       logger,
                                       false)) {
      logger.error("ue={}: \"{}\" failed to create UE context at target DU", command.source_ue_index, name());
      cu_cp_notifier.on_ue_removal_required(target_ue_context_setup_request.ue_index);
      CORO_EARLY_RETURN(response_msg);
    }
  }

  // Target UE object exists from this point on.
  target_ue = ue_manager.find_du_ue(target_ue_context_setup_response.ue_index);
  srsran_assert(
      target_ue != nullptr, "Couldn't find UE with index {} in target DU", target_ue_context_setup_response.ue_index);

  // Setup SRB1 and initialize security context in RRC
  {
    for (const auto& srb_id : source_rrc_context.srbs) {
      create_srb(target_ue, srb_id);
    }
  }

  // Inform CU-UP about new DL tunnels.
  {
    //  prepare Bearer Context Release Command and call E1AP notifier
    bearer_context_modification_request.ue_index = command.source_ue_index;

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle Bearer Context Modification Response
    if (!handle_bearer_context_modification_response(
            response_msg, source_ue_context_mod_request, bearer_context_modification_response, next_config, logger)) {
      logger.error("ue={}: \"{}\" failed to modify bearer context at target CU-UP", command.source_ue_index, name());

      {
        // Remove target UE context if Bearer Context Modification failed.
        {
          ue_context_release_command.ue_index = target_ue_context_setup_response.ue_index;
          ue_context_release_command.cause    = cause_radio_network_t::unspecified;
          CORO_AWAIT(target_du_processor_notifier.handle_ue_context_release_command(ue_context_release_command));
        }

        logger.debug("ue={}: \"{}\" removed target UE context", command.source_ue_index, name());
      }

      CORO_EARLY_RETURN(response_msg);
    }
  }

  {
    // prepare RRC Reconfiguration and call RRC UE notifier
    // if default DRB is being setup, SRB2 needs to be setup as well
    {
      if (!fill_rrc_reconfig_args(rrc_reconfig_args,
                                  target_ue_context_setup_request.srbs_to_be_setup_list,
                                  next_config.pdu_sessions_to_setup_list,
                                  target_ue_context_setup_response.du_to_cu_rrc_info,
                                  {} /* No NAS PDUs required */,
                                  target_ue->get_rrc_ue_notifier().generate_meas_config(source_rrc_context.meas_cfg),
                                  true, /* Reestablish SRBs */
                                  false /* do not reestablish DRBs */,
                                  true, /* Update keys */
                                  logger)) {
        logger.error("ue={}: \"{}\" Failed to fill RRC Reconfiguration", command.source_ue_index, name());
        CORO_EARLY_RETURN(response_msg);
      }
    }

    target_ue = ue_manager.find_du_ue(target_ue_context_setup_response.ue_index);
    // Trigger RRC Reconfiguration
    CORO_AWAIT_VALUE(bool reconf_result,
                     launch_async<handover_reconfiguration_routine>(rrc_reconfig_args, *source_ue, *target_ue, logger));

    if (!reconf_result) {
      logger.error("ue={}: \"{}\" RRC Reconfiguration failed", command.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }
  }

  {
    // Prepare update for UP resource manager.
    up_config_update_result result;
    for (const auto& pdu_session_to_add : next_config.pdu_sessions_to_setup_list) {
      result.pdu_sessions_added_list.push_back(pdu_session_to_add.second);
    }
    target_ue->get_up_resource_manager().apply_config_update(result);
  }

  // Transfer old UE context (NGAP and E1AP) to new UE context and remove old UE context.
  CORO_AWAIT_VALUE(context_transfer_success,
                   cu_cp_notifier.on_ue_transfer_required(target_ue->get_ue_index(), command.source_ue_index));
  if (not context_transfer_success) {
    logger.error("ue={}: \"{}\" failed to transfer UE context", command.source_ue_index, name());
    CORO_EARLY_RETURN(response_msg);
  }

  // Remove source UE context.
  {
    ue_context_release_command.ue_index = source_ue->get_ue_index();
    ue_context_release_command.cause    = cause_radio_network_t::unspecified;
    CORO_AWAIT(source_du_processor_notifier.handle_ue_context_release_command(ue_context_release_command));
    logger.debug("ue={}: \"{}\" removed source UE context", ue_context_release_command.ue_index, name());
  }

  logger.debug("ue={}: \"{}\" finalized", command.source_ue_index, name());
  response_msg.success = true;

  CORO_RETURN(response_msg);
}

bool inter_du_handover_routine::generate_ue_context_setup_request(f1ap_ue_context_setup_request& setup_request,
                                                                  const static_vector<srb_id_t, MAX_NOF_SRBS>& srbs,
                                                                  const rrc_ue_transfer_context& transfer_context)
{
  setup_request.serv_cell_idx = 0; // TODO: Remove hardcoded value
  setup_request.sp_cell_id    = command.cgi;

  if (transfer_context.handover_preparation_info.empty()) {
    return false;
  }
  setup_request.cu_to_du_rrc_info.ie_exts.emplace();
  setup_request.cu_to_du_rrc_info.ie_exts.value().ho_prep_info = transfer_context.handover_preparation_info;

  for (const auto& srb_id : srbs) {
    f1ap_srbs_to_be_setup_mod_item srb_item;
    srb_item.srb_id = srb_id;
    setup_request.srbs_to_be_setup_list.emplace(srb_item.srb_id, srb_item);
  }

  for (const auto& pdu_session : next_config.pdu_sessions_to_setup_list) {
    for (const auto& drb : pdu_session.second.drb_to_add) {
      const up_drb_context& drb_context = drb.second;

      f1ap_drbs_to_be_setup_mod_item drb_item;
      drb_item.drb_id           = drb_context.drb_id;
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

      setup_request.drbs_to_be_setup_list.emplace(drb_item.drb_id, drb_item);
    }
  }

  return true;
}

void inter_du_handover_routine::create_srb(du_ue* ue, srb_id_t srb_id)
{
  srb_creation_message srb_msg{};
  srb_msg.ue_index        = ue->get_ue_index();
  srb_msg.srb_id          = srb_id;
  srb_msg.enable_security = true;
  // TODO: add support for non-default PDCP config.
  ue->get_rrc_ue_srb_notifier().create_srb(srb_msg);
}