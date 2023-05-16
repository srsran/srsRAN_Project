/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdu_session_resource_setup_routine.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

pdu_session_resource_setup_routine::pdu_session_resource_setup_routine(
    const cu_cp_pdu_session_resource_setup_request& setup_msg_,
    const ue_configuration&                         ue_cfg_,
    const srsran::security::sec_as_config&          security_cfg_,
    du_processor_e1ap_control_notifier&             e1ap_ctrl_notif_,
    du_processor_f1ap_ue_context_notifier&          f1ap_ue_ctxt_notif_,
    du_processor_rrc_ue_control_message_notifier&   rrc_ue_notifier_,
    up_resource_manager&                            rrc_ue_up_resource_manager_,
    srslog::basic_logger&                           logger_) :
  setup_msg(setup_msg_),
  ue_cfg(ue_cfg_),
  security_cfg(security_cfg_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  rrc_ue_notifier(rrc_ue_notifier_),
  rrc_ue_up_resource_manager(rrc_ue_up_resource_manager_),
  logger(logger_)
{
}

bool update_setup_list(cu_cp_pdu_session_resource_setup_response&     response_msg,
                       cu_cp_ue_context_modification_request&         ue_context_mod_request,
                       const cu_cp_pdu_session_resource_setup_request setup_msg,
                       const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                                               pdu_session_resource_setup_list,
                       const up_config_update& next_config,
                       srslog::basic_logger&   logger)
{
  for (const auto& e1ap_item : pdu_session_resource_setup_list) {
    // Sanity check - make sure this session ID is present in the original setup message.
    if (setup_msg.pdu_session_res_setup_items.contains(e1ap_item.pdu_session_id) == false) {
      logger.error("PDU Session Resource setup request doesn't include setup for PDU session {}",
                   e1ap_item.pdu_session_id);
      return false;
    }
    // Also check if PDU session is included in expected next configuration.
    if (next_config.pdu_sessions_to_setup_list.find(e1ap_item.pdu_session_id) ==
        next_config.pdu_sessions_to_setup_list.end()) {
      logger.error("Didn't expect setup for PDU session {}", e1ap_item.pdu_session_id);
      return false;
    }

    cu_cp_pdu_session_res_setup_response_item item;
    item.pdu_session_id = e1ap_item.pdu_session_id;

    auto& transfer                                    = item.pdu_session_resource_setup_response_transfer;
    transfer.dlqos_flow_per_tnl_info.up_tp_layer_info = e1ap_item.ng_dl_up_tnl_info;

    for (const auto& e1ap_drb_item : e1ap_item.drb_setup_list_ng_ran) {
      // TODO: add DRB verification

      for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
        // Verify the QoS flow ID is present in original setup message.
        if (setup_msg.pdu_session_res_setup_items[e1ap_item.pdu_session_id].qos_flow_setup_request_items.contains(
                e1ap_flow.qos_flow_id) == false) {
          logger.error("PDU Session Resource setup request doesn't include setup for QoS flow {} in PDU session {}",
                       e1ap_flow.qos_flow_id,
                       e1ap_item.pdu_session_id);
          return false;
        }

        cu_cp_associated_qos_flow qos_flow;
        qos_flow.qos_flow_id = e1ap_flow.qos_flow_id;
        transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list.emplace(e1ap_flow.qos_flow_id, qos_flow);
      }
      // Catch implementation limitations.
      if (!e1ap_drb_item.flow_failed_list.empty()) {
        logger.warning("Non-empty QoS flow failed list not supported");
        return false;
      }

      // verify only a single UL transport info item is present.
      if (e1ap_drb_item.ul_up_transport_params.size() != 1) {
        logger.error("Multiple UL UP transport items not supported");
        return false;
      }

      // Fill UE context modification for DU
      {
        cu_cp_drbs_to_be_setup_mod_item drb_setup_mod_item;
        drb_setup_mod_item.drb_id = e1ap_drb_item.drb_id;

        // Add qos info
        for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
          drb_setup_mod_item.qos_info.drb_qos.qos_characteristics =
              setup_msg.pdu_session_res_setup_items[item.pdu_session_id]
                  .qos_flow_setup_request_items[e1ap_flow.qos_flow_id]
                  .qos_flow_level_qos_params.qos_characteristics;

          non_dyn_5qi_descriptor_t non_dyn_5qi;
          non_dyn_5qi.five_qi = setup_msg.pdu_session_res_setup_items[item.pdu_session_id]
                                    .qos_flow_setup_request_items[e1ap_flow.qos_flow_id]
                                    .qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value()
                                    .five_qi;

          drb_setup_mod_item.qos_info.drb_qos.qos_characteristics.non_dyn_5qi = non_dyn_5qi;
        }

        // Add up tnl info
        for (const auto& ul_up_transport_param : e1ap_drb_item.ul_up_transport_params) {
          drb_setup_mod_item.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_transport_param.up_tnl_info);
        }

        // Add rlc mode
        drb_setup_mod_item.rlc_mod = rlc_mode::am; // TODO: is this coming from FiveQI mapping?

        ue_context_mod_request.drbs_to_be_setup_mod_list.emplace(e1ap_drb_item.drb_id, drb_setup_mod_item);
      }
    }

    // Fail on any DRB that fails to be setup
    if (!e1ap_item.drb_failed_list_ng_ran.empty()) {
      logger.error("Non-empty DRB failed list not supported");
      return false;
    }

    response_msg.pdu_session_res_setup_response_items.emplace(item.pdu_session_id, item);
  }

  return true;
}

void update_failed_list(
    cu_cp_pdu_session_resource_setup_response&                                        response_msg,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>& pdu_session_resource_failed_list)
{
  for (const auto& e1ap_item : pdu_session_resource_failed_list) {
    // Add to list taking cause received from CU-UP.
    cu_cp_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id                                         = e1ap_item.pdu_session_id;
    failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause = e1ap_item.cause;
    response_msg.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
  }
}

// Free function to amend to the final procedure response message. This will take the results from the various
// sub-procedures and update the succeeded/failed fields.
bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&       response_msg,
                               cu_cp_ue_context_modification_request&           ue_context_mod_request,
                               const cu_cp_pdu_session_resource_setup_request   setup_msg,
                               const e1ap_bearer_context_modification_response& bearer_context_modification_response,
                               const up_config_update&                          next_config,
                               srslog::basic_logger&                            logger)
{
  // Traverse setup list
  if (update_setup_list(response_msg,
                        ue_context_mod_request,
                        setup_msg,
                        bearer_context_modification_response.pdu_session_resource_setup_list,
                        next_config,
                        logger) == false) {
    return false;
  }

  // Traverse failed list
  update_failed_list(response_msg, bearer_context_modification_response.pdu_session_resource_failed_list);

  for (const auto& e1ap_item : bearer_context_modification_response.pdu_session_resource_modified_list) {
    // modified list
    logger.warning("Implement handling of resource modified item {}.", e1ap_item.pdu_session_id);
  }

  for (const auto& e1ap_item : bearer_context_modification_response.pdu_session_resource_failed_to_modify_list) {
    // modified list
    logger.warning("Implement handling of resource failed to modify item {}.", e1ap_item.pdu_session_id);
  }

  return bearer_context_modification_response.success;
}

// Same as above but taking the result from E1AP Bearer Context Setup message
bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                               cu_cp_ue_context_modification_request&          ue_context_mod_request,
                               const cu_cp_pdu_session_resource_setup_request& setup_msg,
                               const e1ap_bearer_context_setup_response&       bearer_context_setup_response,
                               const up_config_update&                         next_config,
                               srslog::basic_logger&                           logger)
{
  // Traverse setup list
  if (update_setup_list(response_msg,
                        ue_context_mod_request,
                        setup_msg,
                        bearer_context_setup_response.pdu_session_resource_setup_list,
                        next_config,
                        logger) == false) {
    return false;
  }

  // Traverse failed list
  update_failed_list(response_msg, bearer_context_setup_response.pdu_session_resource_failed_list);

  return bearer_context_setup_response.success;
}

// Helper function to fail all requested PDU session.
void fill_response_failed_list(cu_cp_pdu_session_resource_setup_response&      response_msg,
                               const cu_cp_pdu_session_resource_setup_request& setup_msg)
{
  for (const auto& item : setup_msg.pdu_session_res_setup_items) {
    cu_cp_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id                                         = item.pdu_session_id;
    failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause = cause_t::misc;
    response_msg.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
  }
}

// This method takes the F1AP UE Context Modification Response message and pre-fills the subsequent
// bearer context modification message to be send to the CU-UP.
// In case of a negative outcome it also prefills the final PDU session resource setup respone message.
bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                               e1ap_bearer_context_modification_request&       bearer_ctxt_mod_request,
                               const cu_cp_pdu_session_resource_setup_request& setup_msg,
                               const cu_cp_ue_context_modification_response&   ue_context_modification_response,
                               const up_config_update&                         next_config,
                               srslog::basic_logger&                           logger)
{
  // Fail procedure if (single) DRB couldn't be setup
  if (!ue_context_modification_response.drbs_failed_to_be_setup_mod_list.empty()) {
    logger.error("Couldn't setup {} DRBs at DU.",
                 ue_context_modification_response.drbs_failed_to_be_setup_mod_list.size());
    return false;
  }

  // Start with empty message.
  e1ap_ng_ran_bearer_context_mod_request& e1ap_bearer_context_mod =
      bearer_ctxt_mod_request.ng_ran_bearer_context_mod_request.emplace();

  /// Iterate over all PDU sessions to be setup and match the containing DRBs.
  for (const auto& pdu_session : next_config.pdu_sessions_to_setup_list) {
    // The modifications are only for this PDU session.
    e1ap_pdu_session_res_to_modify_item e1ap_mod_item;
    e1ap_mod_item.pdu_session_id = pdu_session.second.id;

    for (const auto& drb_item : ue_context_modification_response.drbs_setup_mod_list) {
      // Only include the DRB if it belongs to the this session.
      if (pdu_session.second.drbs.find(drb_item.drb_id) != pdu_session.second.drbs.end()) {
        // DRB belongs to this PDU session
        e1ap_drb_to_modify_item_ng_ran e1ap_drb_item;
        e1ap_drb_item.drb_id = drb_item.drb_id;

        for (const auto& dl_up_param : drb_item.dl_up_tnl_info_to_be_setup_list) {
          e1ap_up_params_item e1ap_dl_up_param;

          e1ap_dl_up_param.up_tnl_info   = dl_up_param.dl_up_tnl_info;
          e1ap_dl_up_param.cell_group_id = 0; // TODO: Remove hardcoded value

          e1ap_drb_item.dl_up_params.push_back(e1ap_dl_up_param);
        }
        e1ap_mod_item.drb_to_modify_list_ng_ran.emplace(drb_item.drb_id, e1ap_drb_item);
      }
    }
    e1ap_bearer_context_mod.pdu_session_res_to_modify_list.emplace(e1ap_mod_item.pdu_session_id, e1ap_mod_item);
  }

  // Let all PDU sessions fail if response is negative.
  if (ue_context_modification_response.success == false) {
    fill_response_failed_list(response_msg, setup_msg);
  }

  // TODO: traverse other fields

  return ue_context_modification_response.success;
}

bool handle_procedure_response(cu_cp_pdu_session_resource_setup_response&      response_msg,
                               const cu_cp_pdu_session_resource_setup_request& setup_msg,
                               bool                                            rrc_reconfig_result,
                               srslog::basic_logger&                           logger)
{
  // Let all PDU sessions fail if response is negative.
  if (rrc_reconfig_result == false) {
    fill_response_failed_list(response_msg, setup_msg);
  }

  return rrc_reconfig_result;
}

void pdu_session_resource_setup_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", setup_msg.ue_index, name());

  // Perform initial sanity checks.
  if (setup_msg.pdu_session_res_setup_items.empty()) {
    logger.info("ue={}: \"{}\" Skipping empty PDU Session Resource Setup", setup_msg.ue_index, name());
    CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
  }

  for (const auto& setup_item : setup_msg.pdu_session_res_setup_items) {
    // Make sure PDU session does not already exist.
    if (rrc_ue_up_resource_manager.has_pdu_session(setup_item.pdu_session_id)) {
      logger.error(
          "ue={}: \"{}\" PDU session ID {} already exists.", setup_msg.ue_index, name(), setup_item.pdu_session_id);
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }

    if (setup_item.qos_flow_setup_request_items.empty()) {
      logger.error("ue={}: \"{}\" Setup request for PDU session ID {} has empty QoS flow to setup list.",
                   setup_msg.ue_index,
                   name(),
                   setup_item.pdu_session_id);
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }

    // initial sanity check, making sure we only allow flows with a configured 5QI
    for (const qos_flow_setup_request_item& flow_item : setup_item.qos_flow_setup_request_items) {
      if (not valid_5qi(flow_item)) {
        five_qi_t five_qi;
        if (flow_item.qos_flow_level_qos_params.qos_characteristics.dyn_5qi.has_value()) {
          five_qi = flow_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value().five_qi;
        } else if (flow_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.has_value()) {
          five_qi = flow_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value().five_qi;
        } else {
          CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false))
        }

        logger.error("ue={}: \"{}\" QoS flow 5QI is not configured. id {} 5QI {}",
                     setup_msg.ue_index,
                     name(),
                     flow_item.qos_flow_id,
                     five_qi);
        CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
      }
    }
  }

  {
    CORO_AWAIT_VALUE(ue_capability_transfer_result,
                     rrc_ue_notifier.on_ue_capability_transfer_request(ue_capability_transfer_request));

    // Handle UE Capability Transfer result
    if (not ue_capability_transfer_result) {
      logger.error("ue={}: \"{}\" UE capability transfer failed", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  {
    // Calculate next user-plane configuration based on incoming setup message.
    next_config = rrc_ue_up_resource_manager.calculate_update(setup_msg);
  }

  // sanity check passed, decide whether we have to create a Bearer Context at the CU-UP or modify an existing one.
  if (next_config.initial_context_creation) {
    // prepare BearerContextSetupRequest
    fill_e1ap_bearer_context_setup_request(bearer_context_setup_request);

    // call E1AP procedure
    CORO_AWAIT_VALUE(bearer_context_setup_response,
                     e1ap_ctrl_notifier.on_bearer_context_setup_request(bearer_context_setup_request));

    // Handle BearerContextSetupResponse
    if (handle_procedure_response(
            response_msg, ue_context_mod_request, setup_msg, bearer_context_setup_response, next_config, logger) ==
        false) {
      logger.error("ue={}: \"{}\" failed to setup bearer at CU-UP.", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  } else {
    // prepare BearerContextModificationRequest and modify existing bearer
    bearer_context_modification_request.ng_ran_bearer_context_mod_request.emplace(); // initialize fresh message
    fill_initial_e1ap_bearer_context_modification_request(bearer_context_modification_request);

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (handle_procedure_response(response_msg,
                                  ue_context_mod_request,
                                  setup_msg,
                                  bearer_context_modification_response,
                                  next_config,
                                  logger) == false) {
      logger.error("ue={}: \"{}\" failed to modification bearer at CU-UP.", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // Register required DRB resources at DU
  {
    // prepare UE Context Modification Request and call F1 notifier
    ue_context_mod_request.ue_index = setup_msg.ue_index;

    // DRB setup have already added above.
    CORO_AWAIT_VALUE(ue_context_modification_response,
                     f1ap_ue_ctxt_notifier.on_ue_context_modification_request(ue_context_mod_request));

    // Handle UE Context Modification Response
    if (handle_procedure_response(response_msg,
                                  bearer_context_modification_request,
                                  setup_msg,
                                  ue_context_modification_response,
                                  next_config,
                                  logger) == false) {
      logger.error("ue={}: \"{}\" failed to modify UE context at DU.", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // Inform CU-UP about the new TEID for UL F1u traffic
  {
    // add remaining fields to BearerContextModificationRequest
    bearer_context_modification_request.ue_index = setup_msg.ue_index;

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (handle_procedure_response(response_msg,
                                  ue_context_mod_request,
                                  setup_msg,
                                  bearer_context_modification_response,
                                  next_config,
                                  logger) == false) {
      logger.error("ue={}: \"{}\" failed to modification bearer at CU-UP.", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  {
    // prepare RRC Reconfiguration and call RRC UE notifier
    {
      for (const auto& pdu_session_to_add : next_config.pdu_sessions_to_setup_list) {
        // Add radio bearer config
        for (const auto& drb_to_add : pdu_session_to_add.second.drbs) {
          cu_cp_drb_to_add_mod drb_to_add_mod;
          drb_to_add_mod.drb_id   = drb_to_add.first;
          drb_to_add_mod.pdcp_cfg = drb_to_add.second.pdcp_cfg;

          // Add CN association and SDAP config
          cu_cp_cn_assoc cn_assoc;
          cn_assoc.sdap_cfg       = drb_to_add.second.sdap_cfg;
          drb_to_add_mod.cn_assoc = cn_assoc;

          cu_cp_radio_bearer_config radio_bearer_config;
          radio_bearer_config.drb_to_add_mod_list.emplace(drb_to_add.first, drb_to_add_mod);
          rrc_reconfig_args.radio_bearer_cfg = radio_bearer_config;
        }

        // set masterCellGroupConfig as received by DU
        cu_cp_rrc_recfg_v1530_ies rrc_recfg_v1530_ies;
        rrc_recfg_v1530_ies.master_cell_group =
            ue_context_modification_response.du_to_cu_rrc_info.cell_group_cfg.copy();

        // append NAS PDUs as received by AMF
        for (const auto& pdu_session : setup_msg.pdu_session_res_setup_items) {
          if (!pdu_session.pdu_session_nas_pdu.empty()) {
            rrc_recfg_v1530_ies.ded_nas_msg_list.push_back(pdu_session.pdu_session_nas_pdu.copy());
          }
        }
        rrc_reconfig_args.non_crit_ext = rrc_recfg_v1530_ies;
      }
    }

    CORO_AWAIT_VALUE(rrc_reconfig_result, rrc_ue_notifier.on_rrc_reconfiguration_request(rrc_reconfig_args));

    // Handle UE Context Modification Response
    if (handle_procedure_response(response_msg, setup_msg, rrc_reconfig_result, logger) == false) {
      logger.error("ue={}: \"{}\" RRC Reconfiguration failed.", setup_msg.ue_index, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // we are done, all good
  CORO_RETURN(handle_pdu_session_resource_setup_result(true));
}

// Helper to mark all PDU sessions that were requested to be set up as failed.
void mark_all_sessions_as_failed(cu_cp_pdu_session_resource_setup_response&      response_msg,
                                 const cu_cp_pdu_session_resource_setup_request& setup_msg,
                                 cause_t                                         cause)
{
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item> failed_list;
  for (const auto& setup_item : setup_msg.pdu_session_res_setup_items) {
    e1ap_pdu_session_resource_failed_item fail_item;
    fail_item.pdu_session_id = setup_item.pdu_session_id;
    fail_item.cause          = cause;
    failed_list.emplace(setup_item.pdu_session_id, fail_item);
  }
  update_failed_list(response_msg, failed_list);
}

cu_cp_pdu_session_resource_setup_response
pdu_session_resource_setup_routine::handle_pdu_session_resource_setup_result(bool success)
{
  if (success) {
    logger.debug("ue={}: \"{}\" finalized.", setup_msg.ue_index, name());

    // Prepare update for UP resource manager.
    up_config_update_result result;
    for (const auto& pdu_session_to_add : next_config.pdu_sessions_to_setup_list) {
      result.pdu_sessions_added_list.push_back(pdu_session_to_add.second);
    }
    rrc_ue_up_resource_manager.apply_config_update(result);
  } else {
    logger.error("ue={}: \"{}\" failed.", setup_msg.ue_index, name());

    mark_all_sessions_as_failed(response_msg, setup_msg, cause_t::radio_network);
  }

  return response_msg;
}

void fill_e1ap_qos_flow_param_item(e1ap_qos_flow_qos_param_item&      e1ap_qos_item,
                                   srslog::basic_logger&              logger,
                                   const qos_flow_setup_request_item& request_item)
{
  e1ap_qos_item.qos_flow_id = request_item.qos_flow_id;

  if (request_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.has_value()) {
    non_dyn_5qi_descriptor_t non_dyn_5qi;
    non_dyn_5qi.five_qi = request_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value().five_qi;

    // TODO: Add optional values

    e1ap_qos_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi = non_dyn_5qi;
  } else {
    logger.warning("qos_flow_id={}: dynamic 5QI not fully supported.", e1ap_qos_item.qos_flow_id);
    // TODO: Add dynamic 5qi
  }

  e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.prio_level =
      request_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp;
  e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_cap =
      request_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap;
  e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_vulnerability =
      request_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability;
}

void fill_e1ap_drb_pdcp_config(e1ap_pdcp_config& e1ap_pdcp_cfg, const pdcp_config& cu_cp_pdcp_cfg)
{
  e1ap_pdcp_cfg.pdcp_sn_size_ul = cu_cp_pdcp_cfg.tx.sn_size;
  e1ap_pdcp_cfg.pdcp_sn_size_dl = cu_cp_pdcp_cfg.rx.sn_size;
  e1ap_pdcp_cfg.rlc_mod         = cu_cp_pdcp_cfg.rlc_mode;
  if (cu_cp_pdcp_cfg.tx.discard_timer != pdcp_discard_timer::not_configured) {
    e1ap_pdcp_cfg.discard_timer = cu_cp_pdcp_cfg.tx.discard_timer;
  }
  if (cu_cp_pdcp_cfg.rx.t_reordering != pdcp_t_reordering::infinity) {
    e1ap_pdcp_cfg.t_reordering_timer = cu_cp_pdcp_cfg.rx.t_reordering;
  }
}

void fill_e1ap_pdu_session_res_to_setup_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_setup_item>& pdu_session_res_to_setup_list,
    srslog::basic_logger&                                                    logger,
    const up_config_update&                                                  next_config,
    const cu_cp_pdu_session_resource_setup_request&                          setup_msg,
    const ue_configuration&                                                  ue_cfg)
{
  for (const auto& setup_item : next_config.pdu_sessions_to_setup_list) {
    const auto& session = setup_item.second;
    srsran_assert(setup_msg.pdu_session_res_setup_items.contains(session.id),
                  "Setup request doesn't contain config for PDU session id={}",
                  session.id);
    // Obtain PDU session config from original setup request.
    const auto&                        pdu_session_cfg = setup_msg.pdu_session_res_setup_items[session.id];
    e1ap_pdu_session_res_to_setup_item e1ap_pdu_session_item;

    e1ap_pdu_session_item.pdu_session_id    = pdu_session_cfg.pdu_session_id;
    e1ap_pdu_session_item.pdu_session_type  = pdu_session_cfg.pdu_session_type;
    e1ap_pdu_session_item.snssai            = pdu_session_cfg.s_nssai;
    e1ap_pdu_session_item.ng_ul_up_tnl_info = pdu_session_cfg.ul_ngu_up_tnl_info;

    e1ap_pdu_session_item.security_ind.integrity_protection_ind       = "not_needed"; // TODO: Remove hardcoded value
    e1ap_pdu_session_item.security_ind.confidentiality_protection_ind = "not_needed"; // TODO: Remove hardcoded value

    e1ap_pdu_session_item.pdu_session_inactivity_timer = ue_cfg.inactivity_timer;

    for (const auto& drb_to_setup : session.drbs) {
      e1ap_drb_to_setup_item_ng_ran e1ap_drb_setup_item;
      e1ap_drb_setup_item.drb_id               = drb_to_setup.first;
      e1ap_drb_setup_item.drb_inactivity_timer = ue_cfg.inactivity_timer;
      e1ap_drb_setup_item.sdap_cfg             = drb_to_setup.second.sdap_cfg;
      fill_e1ap_drb_pdcp_config(e1ap_drb_setup_item.pdcp_cfg, drb_to_setup.second.pdcp_cfg);

      e1ap_cell_group_info_item e1ap_cell_group_item;
      e1ap_cell_group_item.cell_group_id = 0; // TODO: Remove hardcoded value
      e1ap_drb_setup_item.cell_group_info.push_back(e1ap_cell_group_item);

      for (const auto& request_item : pdu_session_cfg.qos_flow_setup_request_items) {
        e1ap_qos_flow_qos_param_item e1ap_qos_item;
        fill_e1ap_qos_flow_param_item(e1ap_qos_item, logger, request_item);
        e1ap_drb_setup_item.qos_flow_info_to_be_setup.emplace(e1ap_qos_item.qos_flow_id, e1ap_qos_item);
      }

      e1ap_pdu_session_item.drb_to_setup_list_ng_ran.emplace(e1ap_drb_setup_item.drb_id, e1ap_drb_setup_item);
    }

    pdu_session_res_to_setup_list.emplace(pdu_session_cfg.pdu_session_id, e1ap_pdu_session_item);
  }
}

void pdu_session_resource_setup_routine::fill_e1ap_bearer_context_setup_request(
    e1ap_bearer_context_setup_request& e1ap_request)
{
  e1ap_request.ue_index = setup_msg.ue_index;

  // security info
  e1ap_request.security_info.security_algorithm.ciphering_algo                 = security_cfg.cipher_algo;
  e1ap_request.security_info.security_algorithm.integrity_protection_algorithm = security_cfg.integ_algo;
  e1ap_request.security_info.up_security_key.encryption_key                    = security_cfg.k_up_enc;
  e1ap_request.security_info.up_security_key.integrity_protection_key          = security_cfg.k_up_int;

  e1ap_request.ue_dl_aggregate_maximum_bit_rate = setup_msg.ue_aggregate_maximum_bit_rate_dl;
  e1ap_request.serving_plmn                     = setup_msg.serving_plmn;
  e1ap_request.activity_notif_level             = "ue"; // TODO: Remove hardcoded value
  if (e1ap_request.activity_notif_level == "ue") {
    e1ap_request.ue_inactivity_timer = ue_cfg.inactivity_timer;
  }

  // Add new PDU sessions.
  fill_e1ap_pdu_session_res_to_setup_list(
      e1ap_request.pdu_session_res_to_setup_list, logger, next_config, setup_msg, ue_cfg);
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
      setup_msg,
      ue_cfg);

  // Remove PDU sessions.
  for (const auto& pdu_session_id : next_config.pdu_sessions_to_remove_list) {
    e1ap_bearer_context_mod.pdu_session_res_to_rem_list.push_back(pdu_session_id);
  }
}

bool pdu_session_resource_setup_routine::valid_5qi(const qos_flow_setup_request_item& flow)
{
  if (flow.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.has_value()) {
    return rrc_ue_up_resource_manager.valid_5qi(
        flow.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value().five_qi);
  } else if (flow.qos_flow_level_qos_params.qos_characteristics.dyn_5qi.has_value()) {
    if (flow.qos_flow_level_qos_params.qos_characteristics.dyn_5qi.value().five_qi.has_value()) {
      return rrc_ue_up_resource_manager.valid_5qi(
          flow.qos_flow_level_qos_params.qos_characteristics.dyn_5qi.value().five_qi.value());
    }
  } else {
    logger.error("Invalid QoS characteristics. Either dynamic or non-dynamic 5qi must be set");
  }
  return false;
}
