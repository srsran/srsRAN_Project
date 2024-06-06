/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mobility_helpers.h"
#include "../pdu_session_routine_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

bool srsran::srs_cu_cp::handle_context_setup_response(
    cu_cp_inter_du_handover_response&         response_msg,
    e1ap_bearer_context_modification_request& bearer_context_modification_request,
    const f1ap_ue_context_setup_response&     target_ue_context_setup_response,
    const srsran::security::sec_as_config&    security_cfg,
    up_config_update&                         next_config,
    const srslog::basic_logger&               logger,
    bool                                      reestablish_pdcp)
{
  // Sanity checks.
  if (target_ue_context_setup_response.ue_index == ue_index_t::invalid) {
    logger.warning("Failed to create UE at the target DU");
    return false;
  }

  if (!target_ue_context_setup_response.srbs_failed_to_be_setup_list.empty()) {
    logger.warning("Couldn't setup {} SRBs at target DU",
                   target_ue_context_setup_response.srbs_failed_to_be_setup_list.size());
    return false;
  }

  if (!target_ue_context_setup_response.drbs_failed_to_be_setup_list.empty()) {
    logger.warning("Couldn't setup {} DRBs at target DU",
                   target_ue_context_setup_response.drbs_failed_to_be_setup_list.size());
    return false;
  }

  if (!target_ue_context_setup_response.c_rnti.has_value()) {
    logger.warning("No C-RNTI present in UE context setup");
    return false;
  }

  // Fill security info
  bearer_context_modification_request.security_info.emplace();
  bearer_context_modification_request.security_info->security_algorithm.ciphering_algo = security_cfg.cipher_algo;
  bearer_context_modification_request.security_info->security_algorithm.integrity_protection_algorithm =
      security_cfg.integ_algo;
  auto k_enc_buffer = byte_buffer::create(security_cfg.k_enc);
  if (k_enc_buffer.is_error()) {
    logger.warning("Unable to allocate byte_buffer");
    return false;
  }
  bearer_context_modification_request.security_info->up_security_key.encryption_key = std::move(k_enc_buffer.value());
  if (security_cfg.k_int.has_value()) {
    auto k_int_buffer = byte_buffer::create(security_cfg.k_int.value());
    if (k_int_buffer.is_error()) {
      logger.warning("Unable to allocate byte_buffer");
      return false;
    }
    bearer_context_modification_request.security_info->up_security_key.integrity_protection_key =
        std::move(k_int_buffer.value());
  }

  // Create bearer context mod request.
  if (!target_ue_context_setup_response.drbs_setup_list.empty()) {
    auto& context_mod_request = bearer_context_modification_request.ng_ran_bearer_context_mod_request.emplace();

    // Extract new DL tunnel information for CU-UP.
    for (const auto& pdu_session : next_config.pdu_sessions_to_setup_list) {
      // The modifications are only for this PDU session.
      e1ap_pdu_session_res_to_modify_item e1ap_mod_item;
      e1ap_mod_item.pdu_session_id = pdu_session.first;

      for (const auto& drb_item : pdu_session.second.drb_to_add) {
        srsran_assert(target_ue_context_setup_response.drbs_setup_list.contains(drb_item.first),
                      "Couldn't find {} in UE context setup response",
                      drb_item.first);

        const auto& context_setup_drb_item = target_ue_context_setup_response.drbs_setup_list[drb_item.first];

        e1ap_drb_to_modify_item_ng_ran e1ap_drb_item;
        e1ap_drb_item.drb_id = drb_item.first;

        for (const auto& dl_up_param : context_setup_drb_item.dl_up_tnl_info_to_be_setup_list) {
          e1ap_up_params_item e1ap_dl_up_param;
          e1ap_dl_up_param.up_tnl_info   = dl_up_param.dl_up_tnl_info;
          e1ap_dl_up_param.cell_group_id = 0; // TODO: Remove hardcoded value

          e1ap_drb_item.dl_up_params.push_back(e1ap_dl_up_param);
        }

        if (reestablish_pdcp) {
          // Reestablish PDCP.
          e1ap_drb_item.pdcp_cfg.emplace();
          fill_e1ap_drb_pdcp_config(e1ap_drb_item.pdcp_cfg.value(), drb_item.second.pdcp_cfg);
          e1ap_drb_item.pdcp_cfg->pdcp_reest = true;
        }

        e1ap_mod_item.drb_to_modify_list_ng_ran.emplace(e1ap_drb_item.drb_id, e1ap_drb_item);
      }

      context_mod_request.pdu_session_res_to_modify_list.emplace(e1ap_mod_item.pdu_session_id, e1ap_mod_item);
    }
  }

  return target_ue_context_setup_response.success;
}

bool srsran::srs_cu_cp::handle_bearer_context_modification_response(
    cu_cp_inter_du_handover_response&                response_msg,
    f1ap_ue_context_modification_request&            source_ue_context_mod_request,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response,
    up_config_update&                                next_config,
    const srslog::basic_logger&                      logger)

{
  // TOOD: Add proper handling.
  return bearer_context_modification_response.success;
}