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

#include "pdu_session_resource_setup_routine.h"
#include "pdu_session_routine_helpers.h"
#include "srsran/ran/cause/common.h"
#include "srsran/ran/cause/e1ap_cause_converters.h"
#include "srsran/ran/cause/ngap_cause.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

// Free function to amend to the final procedure response message. This will take the results from the various
// sub-procedures and update the succeeded/failed fields.
bool handle_bearer_context_modification_response(
    cu_cp_pdu_session_resource_setup_response&       response_msg,
    f1ap_ue_context_modification_request&            ue_context_mod_request,
    up_config_update&                                next_config,
    const cu_cp_pdu_session_resource_setup_request&  setup_msg,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response,
    up_resource_manager&                             up_resource_mng,
    const security_indication_t&                     default_security_indication,
    srslog::basic_logger&                            logger);

// Same as above but taking the result from E1AP Bearer Context Setup message.
bool handle_bearer_context_setup_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                                          f1ap_ue_context_modification_request&           ue_context_mod_request,
                                          const cu_cp_pdu_session_resource_setup_request& setup_msg,
                                          const e1ap_bearer_context_setup_response&       bearer_context_setup_response,
                                          up_config_update&                               next_config,
                                          up_resource_manager&                            up_resource_mng_,
                                          const security_indication_t&                    default_security_indication,
                                          srslog::basic_logger&                           logger);

// This method takes the F1AP UE Context Modification Response message and pre-fills the subsequent
// bearer context modification message to be send to the CU-UP.
// In case of a negative outcome it also prefills the final PDU session resource setup respone message.
bool handle_ue_context_modification_response(
    cu_cp_pdu_session_resource_setup_response&      response_msg,
    e1ap_bearer_context_modification_request&       bearer_ctxt_mod_request,
    const cu_cp_pdu_session_resource_setup_request& setup_msg,
    const f1ap_ue_context_modification_response&    ue_context_modification_response,
    const up_config_update&                         next_config,
    const srslog::basic_logger&                     logger);

bool handle_rrc_reconfiguration_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                                         const cu_cp_pdu_session_resource_setup_request& setup_msg,
                                         bool                                            rrc_reconfig_result,
                                         const srslog::basic_logger&                     logger);

pdu_session_resource_setup_routine::pdu_session_resource_setup_routine(
    const cu_cp_pdu_session_resource_setup_request& setup_msg_,
    const ue_configuration&                         ue_cfg_,
    const srsran::security::sec_as_config&          security_cfg_,
    const security_indication_t&                    default_security_indication_,
    e1ap_bearer_context_manager&                    e1ap_bearer_ctxt_mng_,
    f1ap_ue_context_manager&                        f1ap_ue_ctxt_mng_,
    rrc_ue_interface*                               rrc_ue_,
    cu_cp_rrc_ue_interface&                         cu_cp_notifier_,
    ue_task_scheduler&                              ue_task_sched_,
    up_resource_manager&                            up_resource_mng_,
    srslog::basic_logger&                           logger_) :
  setup_msg(setup_msg_),
  ue_cfg(ue_cfg_),
  security_cfg(security_cfg_),
  default_security_indication(default_security_indication_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  f1ap_ue_ctxt_mng(f1ap_ue_ctxt_mng_),
  rrc_ue(rrc_ue_),
  cu_cp_notifier(cu_cp_notifier_),
  ue_task_sched(ue_task_sched_),
  up_resource_mng(up_resource_mng_),
  logger(logger_)
{
}

void pdu_session_resource_setup_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" started...", setup_msg.ue_index, name());

  // Perform initial sanity checks on incoming message.
  if (!up_resource_mng.validate_request(setup_msg.pdu_session_res_setup_items)) {
    logger.info("ue={}: \"{}\" Invalid PduSessionResourceSetup", setup_msg.ue_index, name());
    CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(
        false, cause_protocol_t::abstract_syntax_error_falsely_constructed_msg));
  }

  {
    // Calculate next user-plane configuration based on incoming setup message.
    next_config = up_resource_mng.calculate_update(setup_msg.pdu_session_res_setup_items);
  }

  // Check next configuration.
  if (next_config.pdu_sessions_to_setup_list.empty()) {
    logger.info("ue={}: \"{}\" No PDU sessions to setup", setup_msg.ue_index, name());
    CORO_EARLY_RETURN(
        handle_pdu_session_resource_setup_result(false, ngap_cause_radio_network_t::radio_res_not_available));
  }

  // Sanity check passed, decide whether we have to create a Bearer Context at the CU-UP or modify an existing one.
  if (next_config.initial_context_creation) {
    // Prepare BearerContextSetupRequest.
    if (!fill_e1ap_bearer_context_setup_request(bearer_context_setup_request)) {
      logger.warning("ue={}: \"{}\" failed to fill bearer context at CU-UP", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(
          false, cause_protocol_t::abstract_syntax_error_falsely_constructed_msg));
    }

    // Call E1AP procedure.
    CORO_AWAIT_VALUE(bearer_context_setup_response,
                     e1ap_bearer_ctxt_mng.handle_bearer_context_setup_request(bearer_context_setup_request));

    // Handle BearerContextSetupResponse.
    if (!handle_bearer_context_setup_response(response_msg,
                                              ue_context_mod_request,
                                              setup_msg,
                                              bearer_context_setup_response,
                                              next_config,
                                              up_resource_mng,
                                              default_security_indication,
                                              logger)) {
      logger.warning("ue={}: \"{}\" failed to setup bearer at CU-UP", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(
          handle_pdu_session_resource_setup_result(false, cause_protocol_t::msg_not_compatible_with_receiver_state));
    }
  } else {
    // Prepare BearerContextModificationRequest and modify existing bearer.
    bearer_context_modification_request.ng_ran_bearer_context_mod_request.emplace(); // initialize fresh message
    fill_initial_e1ap_bearer_context_modification_request(bearer_context_modification_request);

    // Call E1AP procedure and wait for BearerContextModificationResponse.
    CORO_AWAIT_VALUE(
        bearer_context_modification_response,
        e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse.
    if (!handle_bearer_context_modification_response(response_msg,
                                                     ue_context_mod_request,
                                                     next_config,
                                                     setup_msg,
                                                     bearer_context_modification_response,
                                                     up_resource_mng,
                                                     default_security_indication,
                                                     logger)) {
      logger.warning("ue={}: \"{}\" failed to modify bearer at CU-UP", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(
          handle_pdu_session_resource_setup_result(false, cause_protocol_t::msg_not_compatible_with_receiver_state));
    }
  }

  // Register required SRB and DRB resources at DU.
  {
    // Prepare UE Context Modification Request and call F1.
    ue_context_mod_request.ue_index = setup_msg.ue_index;
    ue_context_mod_request.cu_to_du_rrc_info.emplace();
    ue_context_mod_request.cu_to_du_rrc_info.value().ue_cap_rat_container_list =
        rrc_ue->get_packed_ue_capability_rat_container_list();
    ue_context_mod_request.cu_to_du_rrc_info.value().meas_cfg = rrc_ue->get_packed_meas_config();
    ue_context_mod_request.gnb_du_ue_ambr_ul                  = setup_msg.ue_aggregate_maximum_bit_rate_ul;

    // DRB setup have already added above.
    CORO_AWAIT_VALUE(ue_context_modification_response,
                     f1ap_ue_ctxt_mng.handle_ue_context_modification_request(ue_context_mod_request));

    // Handle UE Context Modification Response.
    if (!handle_ue_context_modification_response(response_msg,
                                                 bearer_context_modification_request,
                                                 setup_msg,
                                                 ue_context_modification_response,
                                                 next_config,
                                                 logger)) {
      logger.warning("ue={}: \"{}\" failed to modify UE context at DU", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(
          handle_pdu_session_resource_setup_result(false, cause_protocol_t::msg_not_compatible_with_receiver_state));
    }
  }

  // Inform CU-UP about the new TEID for UL F1u traffic.
  {
    // Add remaining fields to BearerContextModificationRequest.
    bearer_context_modification_request.ue_index = setup_msg.ue_index;

    // Call E1AP procedure and wait for BearerContextModificationResponse.
    CORO_AWAIT_VALUE(
        bearer_context_modification_response,
        e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse.
    if (!handle_bearer_context_modification_response(response_msg,
                                                     ue_context_mod_request,
                                                     next_config,
                                                     setup_msg,
                                                     bearer_context_modification_response,
                                                     up_resource_mng,
                                                     default_security_indication,
                                                     logger)) {
      logger.warning("ue={}: \"{}\" failed to modify bearer at CU-UP", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(
          handle_pdu_session_resource_setup_result(false, cause_protocol_t::msg_not_compatible_with_receiver_state));
    }
  }

  {
    // Prepare RRC Reconfiguration and call RRC UE notifier.
    // If default DRB is being setup, SRB2 needs to be setup as well.
    {
      // Get NAS PDUs as received by AMF.
      std::vector<byte_buffer> nas_pdus;
      if (!setup_msg.nas_pdu.empty()) {
        nas_pdus.push_back(setup_msg.nas_pdu);
      }

      for (const auto& pdu_session : setup_msg.pdu_session_res_setup_items) {
        if (!pdu_session.pdu_session_nas_pdu.empty()) {
          nas_pdus.push_back(pdu_session.pdu_session_nas_pdu);
        }
      }

      if (!fill_rrc_reconfig_args(rrc_reconfig_args,
                                  ue_context_mod_request.srbs_to_be_setup_mod_list,
                                  next_config.pdu_sessions_to_setup_list,
                                  {} /* No extra DRB to be removed */,
                                  ue_context_modification_response.du_to_cu_rrc_info,
                                  nas_pdus,
                                  next_config.initial_context_creation ? rrc_ue->generate_meas_config(std::nullopt)
                                                                       : std::optional<rrc_meas_cfg>{},
                                  false,
                                  false,
                                  std::nullopt,
                                  {},
                                  std::nullopt,
                                  logger)) {
        logger.warning("ue={}: \"{}\" Failed to fill RrcReconfiguration", setup_msg.ue_index, name());
        CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(
            false, cause_protocol_t::abstract_syntax_error_falsely_constructed_msg));
      }
    }

    CORO_AWAIT_VALUE(rrc_reconfig_result, rrc_ue->handle_rrc_reconfiguration_request(rrc_reconfig_args));

    // Handle RRCReconfiguration result.
    if (!handle_rrc_reconfiguration_response(response_msg, setup_msg, rrc_reconfig_result, logger)) {
      logger.warning("ue={}: \"{}\" RRC reconfiguration failed", setup_msg.ue_index, name());
      // Notify NGAP to request UE context release from AMF.
      ue_task_sched.schedule_async_task(cu_cp_notifier.handle_ue_context_release(
          {setup_msg.ue_index, {}, ngap_cause_radio_network_t::release_due_to_ngran_generated_reason}));
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(
          false, ngap_cause_radio_network_t::release_due_to_ngran_generated_reason));
    }
  }

  // We are done, all good.
  CORO_RETURN(handle_pdu_session_resource_setup_result(true));
}

/// \brief Processes the response of a Bearer Context Setup/Modification Request.
/// \param[out] ngap_response_list Reference to the final NGAP response.
/// \param[out] srb_setup_mod_list Reference to the successful SRB setup list.
/// \param[out] drb_setup_mod_list Reference to the successful DRB setup list.
/// \param[out] next_config Const reference to the calculated config update.
/// \param[out] ngap_setup_list Const reference to the original NGAP request.
/// \param[in] pdu_session_resource_setup_list Const reference to the PDU sessions of the Bearer Context
/// Setup/Modification Response.
/// \param[in] up_resource_mng Reference to the UP resource manager.
/// \param[in] default_security_indication Const reference to the default security indication.
/// \param[in] logger Reference to the logger.
/// \return True on success, false otherwise.
static bool update_setup_list_with_bearer_ctxt_setup_mod_response(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item>& ngap_response_list,
    std::vector<f1ap_srb_to_setup>&                                                 srb_setup_mod_list,
    std::vector<f1ap_drb_to_setup>&                                                 drb_setup_mod_list,
    up_config_update&                                                               next_config,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>&    ngap_setup_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                                 pdu_session_resource_setup_list,
    up_resource_manager&         up_resource_mng,
    const security_indication_t& default_security_indication,
    const srslog::basic_logger&  logger)
{
  // Set up SRB2 if this is the first DRB to be setup.
  if (up_resource_mng.get_nof_drbs() == 0) {
    f1ap_srb_to_setup srb2;
    srb2.srb_id = srb_id_t::srb2;
    srb_setup_mod_list.push_back(srb2);
  }

  for (const auto& e1ap_item : pdu_session_resource_setup_list) {
    const auto& psi = e1ap_item.pdu_session_id;

    // Sanity check - make sure this session ID is present in the original setup message.
    if (!ngap_setup_list.contains(psi)) {
      logger.warning("PduSessionResourceSetupRequest doesn't include setup for {}", psi);
      return false;
    }
    // Also check if PDU session is included in expected next configuration.
    if (next_config.pdu_sessions_to_setup_list.find(psi) == next_config.pdu_sessions_to_setup_list.end()) {
      logger.warning("Didn't expect setup for {}", psi);
      return false;
    }

    cu_cp_pdu_session_res_setup_response_item item;
    item.pdu_session_id = psi;

    auto& transfer                                    = item.pdu_session_resource_setup_response_transfer;
    transfer.dlqos_flow_per_tnl_info.up_tp_layer_info = e1ap_item.ng_dl_up_tnl_info;

    // Determine security settings for this PDU session and decide whether we have to send the security_result via NGAP.
    bool integrity_enabled = false;
    bool ciphering_enabled = false;

    if (ngap_setup_list[psi].security_ind.has_value()) {
      // TS 38.413 Sec. 8.2.1.2:
      // For each PDU session for which the Security Indication IE is included in the PDU Session Resource Setup Request
      // Transfer IE of the PDU SESSION RESOURCE SETUP REQUEST message, and the Integrity Protection Indication IE
      // or Confidentiality Protection Indication IE is set to "preferred", then the NG-RAN node should, if supported,
      // perform user plane integrity protection or ciphering, respectively, for the concerned PDU session and shall
      // notify whether it performed the user plane integrity protection or ciphering by including the Integrity
      // Protection Result IE or Confidentiality Protection Result IE, respectively, in the PDU Session Resource Setup
      // Response Transfer IE of the PDU SESSION RESOURCE SETUP RESPONSE message.
      const auto& ngap_sec_ind = ngap_setup_list[psi].security_ind.value();
      if (security_result_required(ngap_sec_ind)) {
        // Apply security settings according to the decision in the CU-UP.
        if (!e1ap_item.security_result.has_value()) {
          logger.warning("Missing security result in E1AP response for {}", psi);
          return false;
        }
        const auto& sec_res = e1ap_item.security_result.value();
        integrity_enabled   = sec_res.integrity_protection_result == integrity_protection_result_t::performed;
        ciphering_enabled = sec_res.confidentiality_protection_result == confidentiality_protection_result_t::performed;
        // Add result to NGAP response
        transfer.security_result = sec_res;
      } else {
        // Apply security settings that were requested via NGAP and do not require an explicit reponse.
        integrity_enabled = ngap_sec_ind.integrity_protection_ind == integrity_protection_indication_t::required;
        ciphering_enabled =
            ngap_sec_ind.confidentiality_protection_ind == confidentiality_protection_indication_t::required;
      }
    } else {
      // Security settings were not signaled via NGAP, we have used the defaults of CU-CP.
      const auto sec_ind = default_security_indication;
      if (security_result_required(sec_ind)) {
        // Apply security settings according to the decision in the CU-UP.
        if (!e1ap_item.security_result.has_value()) {
          logger.warning("Missing security result in E1AP response for {}", psi);
          return false;
        }
        const auto& sec_res = e1ap_item.security_result.value();
        integrity_enabled   = sec_res.integrity_protection_result == integrity_protection_result_t::performed;
        ciphering_enabled = sec_res.confidentiality_protection_result == confidentiality_protection_result_t::performed;
        // No result in NGAP response needed here.
      } else {
        // Apply default security settings that do not require an explicit response.
        integrity_enabled = sec_ind.integrity_protection_ind == integrity_protection_indication_t::required;
        ciphering_enabled = sec_ind.confidentiality_protection_ind == confidentiality_protection_indication_t::required;
      }
    }

    auto& next_cfg_pdu_session = next_config.pdu_sessions_to_setup_list.at(psi);

    for (const auto& e1ap_drb_item : e1ap_item.drb_setup_list_ng_ran) {
      const auto& drb_id = e1ap_drb_item.drb_id;
      if (next_config.pdu_sessions_to_setup_list.at(psi).drb_to_add.find(drb_id) ==
          next_config.pdu_sessions_to_setup_list.at(psi).drb_to_add.end()) {
        logger.warning("DRB id {} not part of next configuration", drb_id);
        return false;
      }

      // Update security settings of each DRB.
      next_cfg_pdu_session.drb_to_add.find(drb_id)->second.pdcp_cfg.integrity_protection_required = integrity_enabled;
      next_cfg_pdu_session.drb_to_add.find(drb_id)->second.pdcp_cfg.ciphering_required            = ciphering_enabled;

      // Prepare DRB item for DU.
      f1ap_drb_to_setup drb_setup_mod_item;
      if (!fill_f1ap_drb_setup_mod_item(drb_setup_mod_item,
                                        &transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list,
                                        item.pdu_session_id,
                                        drb_id,
                                        next_config.pdu_sessions_to_setup_list.at(psi).drb_to_add.at(drb_id),
                                        e1ap_drb_item,
                                        ngap_setup_list[item.pdu_session_id].qos_flow_setup_request_items,
                                        logger)) {
        logger.warning("Couldn't populate DRB setup/mod item {}", e1ap_drb_item.drb_id);
        return false;
      }
      drb_setup_mod_list.push_back(drb_setup_mod_item);
    }

    // Fail on any DRB that fails to be setup.
    if (!e1ap_item.drb_failed_list_ng_ran.empty()) {
      logger.warning("Non-empty DRB failed list not supported");
      return false;
    }

    ngap_response_list.emplace(item.pdu_session_id, item);
  }

  return true;
}

/// \brief Processes the response of a Bearer Context Setup/Modification Request.
/// \param[out] response_msg Reference to the final NGAP response.
/// \param[out] next_config Const reference to the calculated config update.
/// \param[in] pdu_session_resource_failed_list Const reference to the failed PDU sessions of the Bearer Context
/// Setup/Modification Response.
static void update_failed_list(
    cu_cp_pdu_session_resource_setup_response&                                        response_msg,
    up_config_update&                                                                 next_config,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>& pdu_session_resource_failed_list)
{
  for (const auto& e1ap_item : pdu_session_resource_failed_list) {
    // Remove from next config.
    next_config.pdu_sessions_to_setup_list.erase(e1ap_item.pdu_session_id);
    response_msg.pdu_session_res_setup_response_items.erase(e1ap_item.pdu_session_id);

    // Add to list taking cause received from CU-UP.
    cu_cp_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id              = e1ap_item.pdu_session_id;
    failed_item.unsuccessful_transfer.cause = e1ap_to_ngap_cause(e1ap_item.cause);
    response_msg.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
  }
}

/// \brief Processes the response of a UE Context Modification Request.
/// \param[in] drb_setup_mod_list Reference to the DRBs originally requested to be setup.
/// \param[in] ngap_setup_list Const reference to the original NGAP request.
/// \param[in] bearer_contxt_mod_response Const reference to the response of the Bearer Context Modification Response.
/// \param[in] next_config Const reference to the calculated config update.
/// \param[in] logger Reference to the logger.
/// \return True on success, false otherwise.
static bool validate_next_config_with_bearer_ctxt_mod_response(
    const std::vector<f1ap_drb_to_setup>&                                        drb_setup_mod_list,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& ngap_setup_list,
    const e1ap_bearer_context_modification_response&                             bearer_contxt_mod_response,
    const up_config_update&                                                      next_config,
    const srslog::basic_logger&                                                  logger)
{
  for (const auto& e1ap_item : bearer_contxt_mod_response.pdu_session_resource_modified_list) {
    const auto& psi = e1ap_item.pdu_session_id;

    // Sanity check - make sure this session ID is present in the original setup message.
    if (!ngap_setup_list.contains(psi)) {
      logger.warning("PduSessionResourceSetupRequest doesn't include setup for {}", psi);
      return false;
    }
    // Also check if PDU session is included in expected next configuration.
    if (next_config.pdu_sessions_to_setup_list.find(psi) == next_config.pdu_sessions_to_setup_list.end()) {
      logger.warning("Didn't expect setup for {}", psi);
      return false;
    }

    const auto& next_cfg_pdu_session = next_config.pdu_sessions_to_setup_list.at(psi);

    for (const auto& e1ap_drb_item : e1ap_item.drb_setup_list_ng_ran) {
      const auto& drb_id = e1ap_drb_item.drb_id;
      if (next_cfg_pdu_session.drb_to_add.find(drb_id) == next_cfg_pdu_session.drb_to_add.end()) {
        logger.warning("DRB id {} not part of next configuration", drb_id);
        return false;
      }

      // Check that all modified DRBs are already part of the next configuration.
      for (const auto& drb_mod_item : e1ap_item.drb_modified_list_ng_ran) {
        auto it =
            std::find_if(drb_setup_mod_list.begin(),
                         drb_setup_mod_list.end(),
                         [&](const f1ap_drb_to_setup& setup_item) { return setup_item.drb_id == drb_mod_item.drb_id; });
        if (it == drb_setup_mod_list.end()) {
          return false;
        }
      }
    }

    // Fail on any DRB that failed modification.
    if (!e1ap_item.drb_failed_to_modify_list_ng_ran.empty()) {
      logger.warning("Non-empty DRB failed to modify list not supported");
      return false;
    }
  }

  return true;
}

/// \brief Processes the response of a Bearer Context Modification Request.
/// \param[out] response_msg Reference to the final NGAP response.
/// \param[out] next_config Const reference to the calculated config update.
/// \param[in] bearer_context_modification_response Const reference to the response of the Bearer Context Modification
/// Request.
static void update_failed_to_modify_list_with_bearer_ctxt_mod_resp(
    cu_cp_pdu_session_resource_setup_response&       response_msg,
    up_config_update&                                next_config,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response)
{
  for (const auto& e1ap_item : bearer_context_modification_response.pdu_session_resource_failed_to_modify_list) {
    // Remove from next config.
    next_config.pdu_sessions_to_setup_list.erase(e1ap_item.pdu_session_id);
    response_msg.pdu_session_res_setup_response_items.erase(e1ap_item.pdu_session_id);

    // Add to list taking cause received from CU-UP.
    cu_cp_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id              = e1ap_item.pdu_session_id;
    failed_item.unsuccessful_transfer.cause = e1ap_to_ngap_cause(e1ap_item.cause);
    response_msg.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
  }
}

// Free function to amend to the final procedure response message. This will take the results from the various
// sub-procedures and update the succeeded/failed fields.
bool handle_bearer_context_modification_response(
    cu_cp_pdu_session_resource_setup_response&       response_msg,
    f1ap_ue_context_modification_request&            ue_context_mod_request,
    up_config_update&                                next_config,
    const cu_cp_pdu_session_resource_setup_request&  setup_msg,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response,
    up_resource_manager&                             up_resource_mng,
    const security_indication_t&                     default_security_indication,
    srslog::basic_logger&                            logger)
{
  // Traverse setup list.
  if (!update_setup_list_with_bearer_ctxt_setup_mod_response(
          response_msg.pdu_session_res_setup_response_items,
          ue_context_mod_request.srbs_to_be_setup_mod_list,
          ue_context_mod_request.drbs_to_be_setup_mod_list,
          next_config,
          setup_msg.pdu_session_res_setup_items,
          bearer_context_modification_response.pdu_session_resource_setup_list,
          up_resource_mng,
          default_security_indication,
          logger)) {
    return false;
  }

  // Traverse failed list.
  update_failed_list(response_msg, next_config, bearer_context_modification_response.pdu_session_resource_failed_list);

  // Traverse modified list.
  if (!validate_next_config_with_bearer_ctxt_mod_response(ue_context_mod_request.drbs_to_be_setup_mod_list,
                                                          setup_msg.pdu_session_res_setup_items,
                                                          bearer_context_modification_response,
                                                          next_config,
                                                          logger)) {
    return false;
  }

  // Traverse failed to modify list.
  update_failed_to_modify_list_with_bearer_ctxt_mod_resp(
      response_msg, next_config, bearer_context_modification_response);

  return bearer_context_modification_response.success;
}

// Same as above but taking the result from E1AP Bearer Context Setup message.
bool handle_bearer_context_setup_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                                          f1ap_ue_context_modification_request&           ue_context_mod_request,
                                          const cu_cp_pdu_session_resource_setup_request& setup_msg,
                                          const e1ap_bearer_context_setup_response&       bearer_context_setup_response,
                                          up_config_update&                               next_config,
                                          up_resource_manager&                            up_resource_mng_,
                                          const security_indication_t&                    default_security_indication,
                                          srslog::basic_logger&                           logger)
{
  // Traverse setup list.
  if (!update_setup_list_with_bearer_ctxt_setup_mod_response(
          response_msg.pdu_session_res_setup_response_items,
          ue_context_mod_request.srbs_to_be_setup_mod_list,
          ue_context_mod_request.drbs_to_be_setup_mod_list,
          next_config,
          setup_msg.pdu_session_res_setup_items,
          bearer_context_setup_response.pdu_session_resource_setup_list,
          up_resource_mng_,
          default_security_indication,
          logger)) {
    return false;
  }

  // Traverse failed list.
  update_failed_list(response_msg, next_config, bearer_context_setup_response.pdu_session_resource_failed_list);

  return bearer_context_setup_response.success;
}

// Helper function to fail all requested PDU session.
static void fill_setup_failed_list(cu_cp_pdu_session_resource_setup_response&      response_msg,
                                   const cu_cp_pdu_session_resource_setup_request& setup_msg)
{
  for (const auto& item : setup_msg.pdu_session_res_setup_items) {
    cu_cp_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id              = item.pdu_session_id;
    failed_item.unsuccessful_transfer.cause = ngap_cause_misc_t::unspecified;
    response_msg.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
  }
}

// This method takes the F1AP UE Context Modification Response message and pre-fills the subsequent
// bearer context modification message to be send to the CU-UP.
// In case of a negative outcome it also prefills the final PDU session resource setup respone message.
bool handle_ue_context_modification_response(
    cu_cp_pdu_session_resource_setup_response&      response_msg,
    e1ap_bearer_context_modification_request&       bearer_ctxt_mod_request,
    const cu_cp_pdu_session_resource_setup_request& setup_msg,
    const f1ap_ue_context_modification_response&    ue_context_modification_response,
    const up_config_update&                         next_config,
    const srslog::basic_logger&                     logger)
{
  // Fail procedure if (single) DRB couldn't be setup.
  if (!ue_context_modification_response.drbs_failed_to_be_setup_list.empty()) {
    logger.warning("Couldn't setup {} DRBs at DU",
                   ue_context_modification_response.drbs_failed_to_be_setup_list.size());
    return false;
  }

  if (!update_setup_list_with_ue_ctxt_setup_response(
          bearer_ctxt_mod_request, ue_context_modification_response.drbs_setup_list, next_config, logger)) {
    return false;
  }

  // Let all PDU sessions fail if response is negative.
  if (!ue_context_modification_response.success) {
    fill_setup_failed_list(response_msg, setup_msg);
  }

  // TODO: traverse other fields

  return ue_context_modification_response.success;
}

bool handle_rrc_reconfiguration_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
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
static void mark_all_sessions_as_failed(cu_cp_pdu_session_resource_setup_response&      response_msg,
                                        const cu_cp_pdu_session_resource_setup_request& setup_msg,
                                        up_config_update&                               next_config,
                                        ngap_cause_t                                    cause)
{
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item> failed_list;
  for (const auto& setup_item : setup_msg.pdu_session_res_setup_items) {
    // Remove from next config.
    next_config.pdu_sessions_to_setup_list.erase(setup_item.pdu_session_id);
    response_msg.pdu_session_res_setup_response_items.erase(setup_item.pdu_session_id);

    // Add to list taking cause received from CU-UP.
    cu_cp_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id              = setup_item.pdu_session_id;
    failed_item.unsuccessful_transfer.cause = cause;
    response_msg.pdu_session_res_failed_to_setup_items.emplace(setup_item.pdu_session_id, failed_item);
  }

  // No PDU session setup can be successful at the same time.
  response_msg.pdu_session_res_setup_response_items.clear();
}

cu_cp_pdu_session_resource_setup_response
pdu_session_resource_setup_routine::handle_pdu_session_resource_setup_result(bool success, ngap_cause_t cause)
{
  if (success) {
    logger.debug("ue={}: \"{}\" finished successfully", setup_msg.ue_index, name());

    // Prepare update for UP resource manager.
    up_config_update_result result;
    for (const auto& pdu_session_to_add : next_config.pdu_sessions_to_setup_list) {
      result.pdu_sessions_added_list.push_back(pdu_session_to_add.second);
    }
    up_resource_mng.apply_config_update(result);
  } else {
    logger.info("ue={}: \"{}\" failed", setup_msg.ue_index, name());

    mark_all_sessions_as_failed(response_msg, setup_msg, next_config, cause);
  }

  return response_msg;
}

bool pdu_session_resource_setup_routine::fill_e1ap_bearer_context_setup_request(
    e1ap_bearer_context_setup_request& e1ap_request)
{
  e1ap_request.ue_index = setup_msg.ue_index;

  // Security info.
  e1ap_request.security_info.security_algorithm.ciphering_algo                 = security_cfg.cipher_algo;
  e1ap_request.security_info.security_algorithm.integrity_protection_algorithm = security_cfg.integ_algo;
  auto k_enc_buffer = byte_buffer::create(security_cfg.k_enc);
  if (not k_enc_buffer.has_value()) {
    logger.warning("Unable to allocate byte_buffer");
    return false;
  }
  e1ap_request.security_info.up_security_key.encryption_key = std::move(k_enc_buffer.value());
  if (security_cfg.k_int.has_value()) {
    auto k_int_buffer = byte_buffer::create(security_cfg.k_int.value());
    if (not k_int_buffer.has_value()) {
      logger.warning("Unable to allocate byte_buffer");
      return false;
    }
    e1ap_request.security_info.up_security_key.integrity_protection_key = std::move(k_int_buffer.value());
  }

  e1ap_request.ue_dl_aggregate_maximum_bit_rate = setup_msg.ue_aggregate_maximum_bit_rate_dl;
  e1ap_request.serving_plmn                     = setup_msg.serving_plmn;
  e1ap_request.activity_notif_level             = "ue"; // TODO: Remove hardcoded value
  if (e1ap_request.activity_notif_level == "ue") {
    e1ap_request.ue_inactivity_timer = ue_cfg.inactivity_timer;
  }

  // Add new PDU sessions.
  fill_e1ap_pdu_session_res_to_setup_list(e1ap_request.pdu_session_res_to_setup_list,
                                          logger,
                                          next_config,
                                          setup_msg.pdu_session_res_setup_items,
                                          ue_cfg,
                                          default_security_indication);

  return true;
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
      setup_msg.pdu_session_res_setup_items,
      ue_cfg,
      default_security_indication);

  // Remove PDU sessions.
  for (const auto& pdu_session_id : next_config.pdu_sessions_to_remove_list) {
    e1ap_bearer_context_mod.pdu_session_res_to_rem_list.push_back(pdu_session_id);
  }
}
