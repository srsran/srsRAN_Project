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

bool verify_ho_request(const cu_cp_inter_du_handover_request& request,
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

inter_du_handover_routine::inter_du_handover_routine(const cu_cp_inter_du_handover_request& request_,
                                                     const byte_buffer&                     target_cell_sib1_,
                                                     e1ap_bearer_context_manager&           e1ap_bearer_ctxt_mng_,
                                                     f1ap_ue_context_manager&               source_du_f1ap_ue_ctxt_mng_,
                                                     f1ap_ue_context_manager&               target_du_f1ap_ue_ctxt_mng_,
                                                     cu_cp_ue_context_release_handler&      ue_context_release_handler_,
                                                     cu_cp_ue_removal_handler&              ue_removal_handler_,
                                                     cu_cp_ue_context_manipulation_handler& cu_cp_handler_,
                                                     ue_manager&                            ue_mng_,
                                                     srslog::basic_logger&                  logger_) :
  request(request_),
  target_cell_sib1(target_cell_sib1_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  source_du_f1ap_ue_ctxt_mng(source_du_f1ap_ue_ctxt_mng_),
  target_du_f1ap_ue_ctxt_mng(target_du_f1ap_ue_ctxt_mng_),
  ue_context_release_handler(ue_context_release_handler_),
  ue_removal_handler(ue_removal_handler_),
  cu_cp_handler(cu_cp_handler_),
  ue_mng(ue_mng_),
  logger(logger_)
{
}

void inter_du_handover_routine::operator()(coro_context<async_task<cu_cp_inter_du_handover_response>>& ctx)
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
    source_rrc_context = source_ue->get_rrc_ue_notifier().get_transfer_context();
    next_config        = to_config_update(source_rrc_context.up_ctx);
  }

  logger.debug("ue={}: \"{}\" initialized", request.source_ue_index, name());

  {
    // Allocate UE index at target DU
    target_ue_context_setup_request.ue_index = ue_mng.add_ue(request.target_du_index);
    if (target_ue_context_setup_request.ue_index == ue_index_t::invalid) {
      logger.warning("ue={}: \"{}\" failed to allocate UE index at target DU", request.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    // prepare F1AP UE Context Setup Command and call F1AP notifier of target DU
    if (!generate_ue_context_setup_request(
            target_ue_context_setup_request, source_ue->get_rrc_ue_srb_notifier().get_srbs(), source_rrc_context)) {
      logger.warning("ue={}: \"{}\" failed to generate UeContextSetupRequest", request.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    CORO_AWAIT_VALUE(target_ue_context_setup_response,
                     target_du_f1ap_ue_ctxt_mng.handle_ue_context_setup_request(target_ue_context_setup_request,
                                                                                source_rrc_context));

    // Handle UE Context Setup Response
    if (!handle_context_setup_response(response_msg,
                                       bearer_context_modification_request,
                                       target_ue_context_setup_response,
                                       next_config,
                                       logger,
                                       true)) {
      logger.warning("ue={}: \"{}\" failed to create UE context at target DU", request.source_ue_index, name());
      CORO_AWAIT(ue_removal_handler.handle_ue_removal_request(target_ue_context_setup_request.ue_index));
      // Note: From this point the UE is removed and only the stored context can be accessed.
      CORO_EARLY_RETURN(response_msg);
    }
  }

  // Target UE object exists from this point on.
  target_ue = ue_mng.find_du_ue(target_ue_context_setup_response.ue_index);
  srsran_assert(target_ue != nullptr, "Couldn't find ue={} in target DU", target_ue_context_setup_response.ue_index);

  // Setup SRB1 and initialize security context in RRC
  {
    for (const auto& srb_id : source_rrc_context.srbs) {
      create_srb(target_ue, srb_id);
    }
  }

  // Inform CU-UP about new DL tunnels.
  {
    // get security context of target UE
    if (!target_ue->get_security_manager().is_security_context_initialized()) {
      logger.warning(
          "ue={}: \"{}\" failed. Cause: Security context not initialized", target_ue->get_ue_index(), name());
      CORO_EARLY_RETURN(response_msg);
    }

    if (!add_security_context_to_bearer_context_modification(target_ue->get_security_manager().get_up_as_config())) {
      logger.warning("ue={}: \"{}\" failed to create UE context at target DU", request.source_ue_index, name());
      CORO_AWAIT(ue_removal_handler.handle_ue_removal_request(target_ue_context_setup_request.ue_index));
      // Note: From this point the UE is removed and only the stored context can be accessed.
      CORO_EARLY_RETURN(response_msg);
    }

    // prepare Bearer Context Modification Request and call E1AP notifier
    bearer_context_modification_request.ue_index = request.source_ue_index;

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(
        bearer_context_modification_response,
        e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_modification_request));

    // Handle Bearer Context Modification Response
    if (!handle_bearer_context_modification_response(
            response_msg, source_ue_context_mod_request, bearer_context_modification_response, next_config, logger)) {
      logger.warning("ue={}: \"{}\" failed to modify bearer context at target CU-UP", request.source_ue_index, name());

      {
        // Remove target UE context if Bearer Context Modification failed.
        {
          ue_context_release_command.ue_index = target_ue_context_setup_response.ue_index;
          ue_context_release_command.cause    = ngap_cause_radio_network_t::unspecified;
          CORO_AWAIT(ue_context_release_handler.handle_ue_context_release_command(ue_context_release_command));
        }

        logger.debug("ue={}: \"{}\" removed target UE context", request.source_ue_index, name());
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
                                  {} /* No DRB to be removed */,
                                  target_ue_context_setup_response.du_to_cu_rrc_info,
                                  {} /* No NAS PDUs required */,
                                  target_ue->get_rrc_ue_notifier().generate_meas_config(source_rrc_context.meas_cfg),
                                  true, /* Reestablish SRBs */
                                  true /* Reestablish DRBs */,
                                  true, /* Update keys */
                                  target_cell_sib1.copy(),
                                  logger)) {
        logger.warning("ue={}: \"{}\" Failed to fill RrcReconfiguration", request.source_ue_index, name());
        CORO_EARLY_RETURN(response_msg);
      }
    }

    // Trigger RRC Reconfiguration
    CORO_AWAIT_VALUE(reconf_result,
                     launch_async<handover_reconfiguration_routine>(rrc_reconfig_args,
                                                                    target_ue_context_setup_response.ue_index,
                                                                    *source_ue,
                                                                    source_du_f1ap_ue_ctxt_mng,
                                                                    cu_cp_handler,
                                                                    logger));

    if (!reconf_result) {
      logger.warning("ue={}: \"{}\" RRC reconfiguration failed", request.source_ue_index, name());
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

  {
    // Transfer old UE context (NGAP and E1AP) to new UE context and remove old UE context.
    cu_cp_handler.handle_handover_ue_context_push(request.source_ue_index, target_ue->get_ue_index());
  }

  // Remove source UE context.
  {
    ue_context_release_command.ue_index             = source_ue->get_ue_index();
    ue_context_release_command.cause                = ngap_cause_radio_network_t::unspecified;
    ue_context_release_command.requires_rrc_release = false;
    CORO_AWAIT(ue_context_release_handler.handle_ue_context_release_command(ue_context_release_command));
    logger.debug("ue={}: \"{}\" removed source UE context", ue_context_release_command.ue_index, name());
  }

  logger.debug("ue={}: \"{}\" finalized", request.source_ue_index, name());
  response_msg.success = true;

  CORO_RETURN(response_msg);
}

bool inter_du_handover_routine::generate_ue_context_setup_request(f1ap_ue_context_setup_request& setup_request,
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
      for (const auto& flow : drb_context.qos_flows) {
        f1ap_flows_mapped_to_drb_item flow_item;
        flow_item.qos_flow_id               = flow.first;
        flow_item.qos_flow_level_qos_params = flow.second.qos_params;
        drb_item.qos_info.flows_mapped_to_drb_list.emplace(flow_item.qos_flow_id, flow_item);
      }
      drb_item.ul_up_tnl_info_to_be_setup_list = drb_context.ul_up_tnl_info_to_be_setup_list;
      drb_item.rlc_mod                         = drb_context.rlc_mod;
      drb_item.pdcp_sn_len                     = drb_context.pdcp_cfg.tx.sn_size;

      setup_request.drbs_to_be_setup_list.emplace(drb_item.drb_id, drb_item);
    }
  }

  return true;
}

void inter_du_handover_routine::create_srb(cu_cp_ue* ue, srb_id_t srb_id)
{
  srb_creation_message srb_msg{};
  srb_msg.ue_index        = ue->get_ue_index();
  srb_msg.srb_id          = srb_id;
  srb_msg.enable_security = true;
  // TODO: add support for non-default PDCP config.
  ue->get_rrc_ue_srb_notifier().create_srb(srb_msg);
}

bool inter_du_handover_routine::add_security_context_to_bearer_context_modification(
    const srsran::security::sec_as_config& security_cfg)
{
  // Fill security info
  bearer_context_modification_request.security_info.emplace();
  bearer_context_modification_request.security_info->security_algorithm.ciphering_algo = security_cfg.cipher_algo;
  bearer_context_modification_request.security_info->security_algorithm.integrity_protection_algorithm =
      security_cfg.integ_algo;
  auto k_enc_buffer = byte_buffer::create(security_cfg.k_enc);
  if (not k_enc_buffer.has_value()) {
    logger.warning("Unable to allocate byte_buffer");
    return false;
  }
  bearer_context_modification_request.security_info->up_security_key.encryption_key = std::move(k_enc_buffer.value());
  if (security_cfg.k_int.has_value()) {
    auto k_int_buffer = byte_buffer::create(security_cfg.k_int.value());
    if (not k_int_buffer.has_value()) {
      logger.warning("Unable to allocate byte_buffer");
      return false;
    }
    bearer_context_modification_request.security_info->up_security_key.integrity_protection_key =
        std::move(k_int_buffer.value());
  }

  return true;
}