/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "inter_du_handover_routine.h"
#include "handover_reconfiguration_routine.h"
#include "mobility_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

inter_du_handover_routine::inter_du_handover_routine(
    const cu_cp_inter_du_handover_request& command_,
    du_processor_f1ap_ue_context_notifier& source_du_f1ap_ue_ctxt_notif_,
    du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notif_,
    du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
    du_processor_ue_manager&               ue_manager_,
    up_resource_manager&                   ue_up_resource_manager_,
    srslog::basic_logger&                  logger_) :
  command(command_),
  source_du_f1ap_ue_ctxt_notifier(source_du_f1ap_ue_ctxt_notif_),
  target_du_f1ap_ue_ctxt_notifier(target_du_f1ap_ue_ctxt_notif_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  ue_manager(ue_manager_),
  ue_up_resource_manager(ue_up_resource_manager_),
  logger(logger_)
{
  source_ue     = ue_manager.find_du_ue(command.source_ue_index);
  ue_up_context = ue_up_resource_manager.get_up_context();
  srsran_assert(source_ue != nullptr, "Can't find source UE index {}.", command.source_ue_index);
}

void inter_du_handover_routine::operator()(coro_context<async_task<cu_cp_inter_du_handover_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", command.source_ue_index, name());

  {
    // prepare F1AP UE Context Setup Command and call F1AP notifier of target DU
    if (!generate_ue_context_setup_request(target_ue_context_setup_request, source_ue->get_srbs())) {
      logger.error("ue={}: \"{}\" failed to generate UE context setup request at DU.", command.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    CORO_AWAIT_VALUE(target_ue_context_setup_response,
                     target_du_f1ap_ue_ctxt_notifier.on_ue_context_setup_request(target_ue_context_setup_request));

    // Handle UE Context Setup Response
    if (!handle_context_setup_response(response_msg,
                                       bearer_context_modification_request,
                                       target_ue_context_setup_response,
                                       ue_up_context,
                                       logger)) {
      logger.error("ue={}: \"{}\" failed to create UE context at target DU.", command.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
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
            response_msg, source_ue_context_mod_request, bearer_context_modification_response, ue_up_context, logger)) {
      logger.error("ue={}: \"{}\" failed to modify bearer context at target CU-UP.", command.source_ue_index, name());

      {
        target_ue_context_release_cmd.ue_index = target_ue_context_setup_response.ue_index;
        target_ue_context_release_cmd.cause    = cause_t::radio_network;
        CORO_AWAIT_VALUE(target_ue_context_release_result,
                         target_du_f1ap_ue_ctxt_notifier.on_ue_context_release_command(target_ue_context_release_cmd));

        if (target_ue_context_release_result == ue_index_t::invalid) {
          logger.error("ue={}: \"{}\" failed to remove UE context at target UE.", command.source_ue_index, name());
        } else {
          logger.debug("ue={}: \"{}\" removed UE context for {} at target UE.",
                       command.source_ue_index,
                       name(),
                       target_ue_context_release_result);
        }
      }

      CORO_EARLY_RETURN(response_msg);
    }
  }

  {
    target_ue = ue_manager.find_du_ue(target_ue_context_setup_response.ue_index);
    // Trigger RRC Reconfiguration
    CORO_AWAIT_VALUE(bool reconf_result,
                     launch_async<handover_reconfiguration_routine>(rrc_reconfig_args, *source_ue, *target_ue, logger));

    if (!reconf_result) {
      logger.error("ue={}: \"{}\" RRC Reconfiguration failed.", command.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }
    // TODO: remove source UE
    // rrc_du_notifier.on_ue_context_release_command(command.source_ue_index);
  }

  {
    CORO_AWAIT_VALUE(source_ue_context_modification_response,
                     source_du_f1ap_ue_ctxt_notifier.on_ue_context_modification_request(source_ue_context_mod_request));
  }

  CORO_RETURN(response_msg);
}

bool inter_du_handover_routine::generate_ue_context_setup_request(f1ap_ue_context_setup_request& setup_request,
                                                                  const std::map<srb_id_t, cu_srb_context>& srbs)
{
  setup_request.serv_cell_idx = 0;
  setup_request.sp_cell_id    = command.cgi;

  for (const auto& srb : srbs) {
    if (srb.second.pdcp_context.has_value()) {
      f1ap_srbs_to_be_setup_mod_item srb_item;
      srb_item.srb_id = srb.first;
      setup_request.srbs_to_be_setup_list.emplace(srb_item.srb_id, srb_item);
    }
  }

  for (const auto& pdu_session : ue_up_context.pdu_sessions) {
    for (const auto& drb : pdu_session.second.drbs) {
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