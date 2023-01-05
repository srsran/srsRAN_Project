/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdu_session_resource_setup_routine.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::rrc_nr;

pdu_session_resource_setup_routine::pdu_session_resource_setup_routine(
    const cu_cp_pdu_session_resource_setup_message& setup_msg_,
    const srsgnb::security::sec_as_config&          security_cfg_,
    du_processor_e1ap_control_notifier&             e1ap_ctrl_notif_,
    f1c_ue_context_manager&                         f1c_ue_ctxt_mng_,
    rrc_ue_control_message_handler&                 rrc_ue_notifier_,
    drb_manager&                                    rrc_ue_drb_manager_,
    srslog::basic_logger&                           logger_) :
  setup_msg(setup_msg_),
  security_cfg(security_cfg_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  f1c_ue_ctxt_mng(f1c_ue_ctxt_mng_),
  rrc_ue_notifier(rrc_ue_notifier_),
  rrc_ue_drb_manager(rrc_ue_drb_manager_),
  logger(logger_)
{
  // calculate DRBs that need to added depending on QoSFlowSetupRequestList, more than one DRB could be needed
  drb_to_add_list = rrc_ue_drb_manager.calculate_drb_to_add_list(setup_msg);
}

void pdu_session_resource_setup_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_setup_response_message>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", setup_msg.cu_cp_ue_id, name());

  // initial sanitfy check, making sure we catch implementation limitations
  if (setup_msg.pdu_session_res_setup_items.size() != 1) {
    logger.error("ue={}: \"{}\" supports only one PDU Session ({} requested).",
                 setup_msg.cu_cp_ue_id,
                 name(),
                 setup_msg.pdu_session_res_setup_items.size());
    CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
  }

  // TODO: Retrieve PDCP and SDAP configs

  {
    // prepare BearerContextSetupRequest
    fill_e1ap_bearer_context_setup_request(bearer_context_setup_request);

    // call E1 procedure
    CORO_AWAIT_VALUE(bearer_context_setup_response,
                     e1ap_ctrl_notifier.on_bearer_context_setup_request(bearer_context_setup_request));

    // Handle BearerContextSetupResponse
    if (not bearer_context_setup_response.success) {
      logger.error("ue={}: \"{}\" failed to setup bearer at CU-UP.", setup_msg.cu_cp_ue_id, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // Register required DRB resources at DU
  {
    // prepare UE Context Modification Request and call F1 notifier
    ue_context_mod_request.ue_index = get_ue_index_from_cu_cp_ue_id(setup_msg.cu_cp_ue_id);
    for (const auto& drb_to_add : drb_to_add_list) {
      srsgnb_assert(drb_to_add != drb_id_t::invalid, "Invalid DRB ID");

      cu_cp_drb_setup_message rrc_ue_drb_setup_message_item;

      rrc_ue_drb_setup_message_item.drb_id = drb_to_add;

      for (const auto& cu_up_gtp_tunnel :
           bearer_context_setup_response
               .pdu_session_resource_setup_list[rrc_ue_drb_manager.get_pdu_session_id(drb_to_add)]
               .drb_setup_list_ng_ran[drb_id_to_uint(drb_to_add)]
               .ul_up_transport_params) {
        up_transport_layer_info gtp_tunnel;
        gtp_tunnel.gtp_teid   = cu_up_gtp_tunnel.up_tnl_info.gtp_teid;
        gtp_tunnel.tp_address = cu_up_gtp_tunnel.up_tnl_info.tp_address;
        rrc_ue_drb_setup_message_item.gtp_tunnels.push_back(gtp_tunnel);
      }

      rrc_ue_drb_setup_message_item.rlc = rlc_mode::am; // TODO: is this coming from FiveQI mapping?

      const auto& mapped_flows = rrc_ue_drb_manager.get_mapped_qos_flows(drb_to_add);
      for (const auto& qos_flow : mapped_flows) {
        qos_flow_setup_request_item mapped_flow = {};
        mapped_flow.qos_flow_id                 = qos_flow;
        rrc_ue_drb_setup_message_item.qos_flows_mapped_to_drb.push_back(mapped_flow);
      }

      ue_context_mod_request.cu_cp_drb_setup_msgs.push_back(rrc_ue_drb_setup_message_item);
    }

    CORO_AWAIT_VALUE(ue_context_modification_response,
                     f1c_ue_ctxt_mng.handle_ue_context_modification_request(ue_context_mod_request));

    // Handle UE Context Modification Response
    if (not ue_context_modification_response.success) {
      logger.error("ue={}: \"{}\" failed to modify UE context at DU.", setup_msg.cu_cp_ue_id, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // Inform CU-UP about the new TEID for UL F1u traffic
  {
    // prepare BearerContextModificationRequest
    fill_e1ap_bearer_context_modification_request(bearer_context_modification_request);

    // call E1 procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (not bearer_context_modification_response.success) {
      logger.error("ue={}: \"{}\" failed to modification bearer at CU-UP.", setup_msg.cu_cp_ue_id, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  {
    // prepare RRC Reconfiguration and call RRC UE notifier
    {
      for (const auto& drb_to_add : drb_to_add_list) {
        cu_cp_drb_to_add_mod drb_to_add_mod;
        drb_to_add_mod.drb_id   = drb_to_add;
        drb_to_add_mod.pdcp_cfg = rrc_ue_drb_manager.get_pdcp_config(drb_to_add);

        // Add CN association and SDAP config
        drb_to_add_mod.cn_assoc                  = cu_cp_cn_assoc{};
        drb_to_add_mod.cn_assoc.value().sdap_cfg = rrc_ue_drb_manager.get_sdap_config(drb_to_add);

        rrc_reconfig_args.radio_bearer_cfg = cu_cp_radio_bearer_config{};
        rrc_reconfig_args.radio_bearer_cfg.value().drb_to_add_mod_list.push_back(drb_to_add_mod);

        // set masterCellGroupConfig as received by DU
        rrc_reconfig_args.non_crit_ext = cu_cp_rrc_recfg_v1530_ies{};
        rrc_reconfig_args.non_crit_ext.value().master_cell_group =
            ue_context_modification_response.du_to_cu_rrc_info.cell_group_cfg.copy();

        // append first PDU sessions NAS PDU as received by AMF
        if (not setup_msg.pdu_session_res_setup_items[0].pdu_session_nas_pdu.empty()) {
          rrc_reconfig_args.non_crit_ext.value().ded_nas_msg_list.push_back(
              setup_msg.pdu_session_res_setup_items[0].pdu_session_nas_pdu.copy());
        }
      }
    }

    CORO_AWAIT_VALUE(rrc_reconfig_result, rrc_ue_notifier.handle_rrc_reconfiguration_request(rrc_reconfig_args));

    // Handle UE Context Modification Response
    if (not rrc_reconfig_result) {
      logger.error("ue={}: \"{}\" RRC Reconfiguration failed.", setup_msg.cu_cp_ue_id, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // we are done, all good
  CORO_RETURN(handle_pdu_session_resource_setup_result(true));
}

cu_cp_pdu_session_resource_setup_response_message
pdu_session_resource_setup_routine::handle_pdu_session_resource_setup_result(bool success)
{
  if (success) {
    for (const auto& setup_item : setup_msg.pdu_session_res_setup_items) {
      cu_cp_pdu_session_res_setup_response_item item;
      item.pdu_session_id = setup_item.pdu_session_id;

      auto& transfer = item.pdu_session_resource_setup_response_transfer;
      transfer.dlqos_flow_per_tnl_info.up_tp_layer_info =
          bearer_context_setup_response.pdu_session_resource_setup_list[setup_item.pdu_session_id].ng_dl_up_tnl_info;

      cu_cp_associated_qos_flow qos_flow;
      qos_flow.qos_flow_id = 1;
      transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list.push_back(qos_flow);

      response_msg.pdu_session_res_setup_response_items.push_back(item);
    }

    logger.debug("ue={}: \"{}\" finalized.", setup_msg.cu_cp_ue_id, name());
  } else {
    // mark all PDU sessions as failed
    for (const auto& setup_item : setup_msg.pdu_session_res_setup_items) {
      cu_cp_pdu_session_res_setup_failed_item item;
      item.pdu_session_id                                         = setup_item.pdu_session_id;
      item.pdu_session_resource_setup_unsuccessful_transfer.cause = cu_cp_cause_t::protocol;
      response_msg.pdu_session_res_failed_to_setup_items.push_back(item);
    }

    logger.error("ue={}: \"{}\" failed.", setup_msg.cu_cp_ue_id, name());
  }

  return response_msg;
}

void pdu_session_resource_setup_routine::fill_e1ap_bearer_context_setup_request(
    e1ap_bearer_context_setup_request& e1ap_request)
{
  e1ap_request.cu_cp_ue_id = setup_msg.cu_cp_ue_id;

  // security info
  e1ap_request.security_info.security_algorithm.ciphering_algo                 = security_cfg.cipher_algo;
  e1ap_request.security_info.security_algorithm.integrity_protection_algorithm = security_cfg.integ_algo;
  e1ap_request.security_info.up_security_key.encryption_key                    = security_cfg.k_up_enc;
  e1ap_request.security_info.up_security_key.integrity_protection_key          = security_cfg.k_up_int;

  e1ap_request.ue_dl_aggregate_maximum_bit_rate = setup_msg.ue_aggregate_maximum_bit_rate_dl;
  e1ap_request.serving_plmn                     = setup_msg.serving_plmn;
  e1ap_request.activity_notif_level             = "ue"; // TODO: Remove hardcoded value

  for (const auto& pdu_session_to_setup : setup_msg.pdu_session_res_setup_items) {
    e1ap_pdu_session_res_to_setup_item e1ap_pdu_session_item;

    e1ap_pdu_session_item.pdu_session_id    = pdu_session_to_setup.pdu_session_id;
    e1ap_pdu_session_item.pdu_session_type  = pdu_session_to_setup.pdu_session_type;
    e1ap_pdu_session_item.snssai            = pdu_session_to_setup.s_nssai;
    e1ap_pdu_session_item.ng_ul_up_tnl_info = pdu_session_to_setup.ul_ngu_up_tnl_info;

    e1ap_pdu_session_item.security_ind.integrity_protection_ind       = "not_needed"; // TODO: Remove hardcoded value
    e1ap_pdu_session_item.security_ind.confidentiality_protection_ind = "not_needed"; // TODO: Remove hardcoded value

    for (const auto& drb_to_setup : drb_to_add_list) {
      e1ap_drb_to_setup_item_ng_ran e1ap_drb_setup_item;
      e1ap_drb_setup_item.drb_id   = drb_to_setup;
      e1ap_drb_setup_item.sdap_cfg = rrc_ue_drb_manager.get_sdap_config(drb_to_setup);

      const auto& cu_cp_pdcp_cfg = rrc_ue_drb_manager.get_pdcp_config(drb_to_setup);

      e1ap_drb_setup_item.pdcp_cfg.pdcp_sn_size_ul = cu_cp_pdcp_cfg.drb.value().pdcp_sn_size_ul.value();
      e1ap_drb_setup_item.pdcp_cfg.pdcp_sn_size_dl = cu_cp_pdcp_cfg.drb.value().pdcp_sn_size_dl.value();
      e1ap_drb_setup_item.pdcp_cfg.rlc_mod         = srsgnb::rlc_mode::am; // TODO: Remove hardcoded value

      e1ap_cell_group_info_item e1ap_cell_group_item;
      e1ap_cell_group_item.cell_group_id = 0; // TODO: Remove hardcoded value
      e1ap_drb_setup_item.cell_group_info.push_back(e1ap_cell_group_item);

      for (const auto& qos_item : pdu_session_to_setup.qos_flow_setup_request_items) {
        e1ap_qos_flow_qos_param_item e1ap_qos_item;
        e1ap_qos_item.qos_flow_id = qos_item.qos_flow_id;

        if (!qos_item.qos_characteristics.is_dynamic_5qi) {
          e1ap_non_dynamic_5qi_descriptor non_dyn_5qi;
          non_dyn_5qi.five_qi = qos_item.qos_characteristics.five_qi;

          // TODO: Add optional values

          e1ap_qos_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi = non_dyn_5qi;
        } else {
          // TODO: Add dynamic 5qi
        }

        e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.prio_level =
            qos_item.qos_characteristics.prio_level_arp;
        e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_cap =
            qos_item.qos_characteristics.pre_emption_cap;
        e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_vulnerability =
            qos_item.qos_characteristics.pre_emption_vulnerability;

        e1ap_drb_setup_item.qos_flow_info_to_be_setup.push_back(e1ap_qos_item);
      }

      e1ap_pdu_session_item.drb_to_setup_list_ng_ran.push_back(e1ap_drb_setup_item);
    }

    e1ap_request.pdu_session_res_to_setup_list.push_back(e1ap_pdu_session_item);
  }
}

void pdu_session_resource_setup_routine::fill_e1ap_bearer_context_modification_request(
    e1ap_bearer_context_modification_request& e1ap_request)
{
  e1ap_request.cu_cp_ue_id = setup_msg.cu_cp_ue_id;

  e1ap_ng_ran_bearer_context_mod_request e1ap_bearer_context_mod;

  // pdu session res to modify list
  for (const auto& pdu_session : bearer_context_setup_response.pdu_session_resource_setup_list) {
    e1ap_pdu_session_res_to_modify_item e1ap_mod_item;

    e1ap_mod_item.pdu_session_id = pdu_session.pdu_session_id;

    for (const auto& drb_item : ue_context_modification_response.drbs_setup_mod_list) {
      e1ap_drb_to_modify_item_ng_ran e1ap_drb_item;
      e1ap_drb_item.drb_id = drb_item.drb_id;

      for (const auto& dl_up_param : drb_item.dl_up_tnl_info_to_be_setup_list) {
        e1ap_up_params_item e1ap_dl_up_param;

        e1ap_dl_up_param.up_tnl_info   = dl_up_param.dl_up_tnl_info;
        e1ap_dl_up_param.cell_group_id = 0; // TODO: Remove hardcoded value

        e1ap_drb_item.dl_up_params.push_back(e1ap_dl_up_param);
      }
      e1ap_mod_item.drb_to_modify_list_ng_ran.push_back(e1ap_drb_item);
    }

    e1ap_bearer_context_mod.pdu_session_res_to_modify_list.push_back(e1ap_mod_item);
  }

  e1ap_request.ng_ran_bearer_context_mod_request = e1ap_bearer_context_mod;
}
