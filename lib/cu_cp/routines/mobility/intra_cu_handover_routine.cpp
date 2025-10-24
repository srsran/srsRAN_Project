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

#include "intra_cu_handover_routine.h"
#include "../pdu_session_routine_helpers.h"
#include "handover_reconfiguration_routine.h"
#include "mobility_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

static bool verify_ho_request(const cu_cp_intra_cu_handover_request& request,
                              ue_manager&                            ue_mng,
                              const srslog::basic_logger&            logger)
{
  if (request.target_pci == INVALID_PCI) {
    logger.warning("Target PCI must not be invalid");
    return false;
  }

  if (request.target_du_index == du_index_t::invalid) {
    logger.warning("Target DU index must not be invalid");
    return false;
  }

  if (request.source_ue_index == ue_index_t::invalid) {
    logger.warning("Source UE index must not be invalid");
    return false;
  }

  if (!ue_mng.find_du_ue(request.source_ue_index)) {
    logger.warning("Can't find source ue={}");
    return false;
  }

  return true;
}

intra_cu_handover_routine::intra_cu_handover_routine(const cu_cp_intra_cu_handover_request& request_,
                                                     const byte_buffer&                     target_cell_sib1_,
                                                     f1ap_ue_context_manager&               source_du_f1ap_ue_ctxt_mng_,
                                                     f1ap_ue_context_manager&               target_du_f1ap_ue_ctxt_mng_,
                                                     cu_cp_impl_interface&                  cu_cp_handler_,
                                                     ue_manager&                            ue_mng_,
                                                     mobility_manager&                      mobility_mng_,
                                                     srslog::basic_logger&                  logger_) :
  request(request_),
  target_cell_sib1(target_cell_sib1_),
  source_du_f1ap_ue_ctxt_mng(source_du_f1ap_ue_ctxt_mng_),
  target_du_f1ap_ue_ctxt_mng(target_du_f1ap_ue_ctxt_mng_),
  cu_cp_handler(cu_cp_handler_),
  ue_mng(ue_mng_),
  mobility_mng(mobility_mng_),
  logger(logger_)
{
}

void intra_cu_handover_routine::operator()(coro_context<async_task<cu_cp_intra_cu_handover_response>>& ctx)
{
  CORO_BEGIN(ctx);

  {
    // Verify input.
    if (!verify_ho_request(request, ue_mng, logger)) {
      logger.warning("ue={}: \"{}\" - invalid input parameters", request.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    // Retrieve source UE context.
    source_ue          = ue_mng.find_du_ue(request.source_ue_index);
    source_rrc_context = source_ue->get_rrc_ue()->get_transfer_context();
    next_config        = to_config_update(source_rrc_context.up_ctx);
  }

  logger.debug("ue={}: \"{}\" started...", request.source_ue_index, name());

  {
    // Allocate UE index at target DU.
    target_ue_context_setup_request.ue_index = ue_mng.add_ue(request.target_du_index);
    if (target_ue_context_setup_request.ue_index == ue_index_t::invalid) {
      logger.warning("ue={}: \"{}\" failed to allocate UE index at target DU", request.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }
    if (!cu_cp_handler.handle_ue_plmn_selected(target_ue_context_setup_request.ue_index,
                                               source_ue->get_ue_context().plmn)) {
      logger.warning("ue={}: \"{}\" failed to set PLMN for target UE", request.source_ue_index, name());
      ue_mng.remove_ue(target_ue_context_setup_request.ue_index);
      CORO_EARLY_RETURN(response_msg);
    }
    // Connect the target UE to the same CU-UP as the source UE.
    ue_mng.find_du_ue(target_ue_context_setup_request.ue_index)->set_cu_up_index(source_ue->get_cu_up_index());

    // Prepare F1AP UE Context Setup Command and call F1AP notifier of target DU.
    if (!generate_ue_context_setup_request(
            target_ue_context_setup_request, source_ue->get_rrc_ue()->get_srbs(), source_rrc_context)) {
      logger.warning("ue={}: \"{}\" failed to generate UeContextSetupRequest", request.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }
    target_ue_context_setup_request.cu_to_du_rrc_info.meas_cfg = source_ue->get_rrc_ue()->get_packed_meas_config();
    target_ue_context_setup_request.serving_cell_mo            = source_ue->get_rrc_ue()->get_serving_cell_mo();

    CORO_AWAIT_VALUE(target_ue_context_setup_response,
                     target_du_f1ap_ue_ctxt_mng.handle_ue_context_setup_request(target_ue_context_setup_request,
                                                                                source_rrc_context));

    // Handle UE Context Setup Response.
    if (!handle_context_setup_response(response_msg,
                                       bearer_context_modification_request,
                                       target_ue_context_setup_response,
                                       next_config,
                                       logger,
                                       true)) {
      logger.warning("ue={}: \"{}\" failed to create UE context at target DU", request.source_ue_index, name());
      CORO_AWAIT(cu_cp_handler.handle_ue_removal_request(target_ue_context_setup_request.ue_index));
      // Note: From this point the UE is removed and only the stored context can be accessed.
      CORO_EARLY_RETURN(response_msg);
    }
  }

  // Target UE object exists from this point on.
  target_ue = ue_mng.find_du_ue(target_ue_context_setup_response.ue_index);
  srsran_assert(target_ue != nullptr, "Couldn't find ue={} in target DU", target_ue_context_setup_response.ue_index);

  // Setup SRB1 and initialize security context in RRC.
  {
    for (const auto& srb_id : source_rrc_context.srbs) {
      create_srb(target_ue, srb_id);
    }
  }

  {
    // Prepare RRC Reconfiguration and call RRC UE notifier.
    // If default DRB is being setup, SRB2 needs to be setup as well.
    // This will also stop the UE data traffic on the source DU.
    {
      if (!fill_rrc_reconfig_args(rrc_reconfig_args,
                                  target_ue_context_setup_request.srbs_to_be_setup_list,
                                  next_config.pdu_sessions_to_setup_list,
                                  {} /* No DRB to be removed */,
                                  target_ue_context_setup_response.du_to_cu_rrc_info,
                                  {} /* No NAS PDUs required */,
                                  target_ue->get_rrc_ue()->generate_meas_config(source_rrc_context.meas_cfg),
                                  true, /* Reestablish SRBs */
                                  true /* Reestablish DRBs */,
                                  target_ue->get_security_manager().get_ncc(), /* Update keys */
                                  target_cell_sib1.copy(),
                                  std::nullopt,
                                  logger)) {
        logger.warning("ue={}: \"{}\" Failed to fill RrcReconfiguration", request.source_ue_index, name());
        CORO_EARLY_RETURN(response_msg);
      }
    }

    // Trigger RRC Reconfiguration.
    CORO_AWAIT_VALUE(rrc_reconfig_sent,
                     launch_async<handover_reconfiguration_routine>(rrc_reconfig_args,
                                                                    bearer_context_modification_request,
                                                                    target_ue_context_setup_response.ue_index,
                                                                    *source_ue,
                                                                    source_du_f1ap_ue_ctxt_mng,
                                                                    cu_cp_handler,
                                                                    logger));
    if (!rrc_reconfig_sent) {
      logger.warning(
          "ue={}: \"{}\" failed to send RRC reconfiguration. Releasing target UE", request.source_ue_index, name());

      ue_context_release_command.ue_index             = target_ue->get_ue_index();
      ue_context_release_command.cause                = ngap_cause_radio_network_t::unspecified;
      ue_context_release_command.requires_rrc_release = false;
      CORO_AWAIT(cu_cp_handler.handle_ue_context_release_command(ue_context_release_command));
      logger.debug("ue={}: \"{}\" removed target UE context", ue_context_release_command.ue_index, name());

      logger.debug("ue={}: \"{}\" failed", request.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    // Notify mobility manager about requested handover execution.
    mobility_mng.get_metrics_handler().aggregate_requested_handover_execution();
  }

  {
    // Prepare update for UP resource manager.
    up_config_update_result result;
    for (const auto& pdu_session_to_add : next_config.pdu_sessions_to_setup_list) {
      result.pdu_sessions_added_list.push_back(pdu_session_to_add.second);
    }
    target_ue->get_up_resource_manager().apply_config_update(result);
  }

  logger.debug("ue={}: \"{}\" finished successfully", request.source_ue_index, name());
  response_msg.success = true;

  CORO_RETURN(response_msg);
}

bool intra_cu_handover_routine::generate_ue_context_setup_request(f1ap_ue_context_setup_request& setup_request,
                                                                  const static_vector<srb_id_t, MAX_NOF_SRBS>& srbs,
                                                                  const rrc_ue_transfer_context& transfer_context)
{
  setup_request.serv_cell_idx = 0; // TODO: Remove hardcoded value
  setup_request.sp_cell_id    = request.cgi;

  if (transfer_context.handover_preparation_info.empty()) {
    return false;
  }

  setup_request.cu_to_du_rrc_info.ie_exts.emplace();
  auto buffer_copy = transfer_context.handover_preparation_info.deep_copy();
  if (!buffer_copy) {
    return false;
  }
  setup_request.cu_to_du_rrc_info.ie_exts.value().ho_prep_info = std::move(buffer_copy.value());
  setup_request.cu_to_du_rrc_info.ue_cap_rat_container_list    = transfer_context.ue_cap_rat_container_list.copy();

  for (const auto& srb_id : srbs) {
    f1ap_srb_to_setup srb_item;
    srb_item.srb_id = srb_id;
    setup_request.srbs_to_be_setup_list.push_back(srb_item);
  }

  for (const auto& pdu_session : next_config.pdu_sessions_to_setup_list) {
    for (const auto& drb : pdu_session.second.drb_to_add) {
      const up_drb_context& drb_context = drb.second;

      f1ap_drb_to_setup drb_item;
      drb_item.drb_id           = drb_context.drb_id;
      drb_item.qos_info.drb_qos = drb_context.qos_params;

      // Add each QoS flow including QoS.
      for (const auto& flow : drb_context.qos_flows) {
        flow_mapped_to_drb flow_item;
        flow_item.qos_flow_id               = flow.first;
        flow_item.qos_flow_level_qos_params = flow.second.qos_params;
        drb_item.qos_info.flows_mapped_to_drb_list.push_back(flow_item);
      }
      drb_item.uluptnl_info_list = drb_context.ul_up_tnl_info_to_be_setup_list;
      drb_item.mode              = drb_context.rlc_mod;
      drb_item.pdcp_sn_len       = drb_context.pdcp_cfg.tx.sn_size;

      setup_request.drbs_to_be_setup_list.push_back(drb_item);
    }
  }

  return true;
}

void intra_cu_handover_routine::create_srb(cu_cp_ue* ue, srb_id_t srb_id)
{
  srb_creation_message srb_msg{};
  srb_msg.ue_index        = ue->get_ue_index();
  srb_msg.srb_id          = srb_id;
  srb_msg.enable_security = true;
  // TODO: add support for non-default PDCP config.
  ue->get_rrc_ue()->create_srb(srb_msg);
}
