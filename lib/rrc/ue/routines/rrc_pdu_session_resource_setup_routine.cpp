/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_pdu_session_resource_setup_routine.h"
#include "../procedures/rrc_reconfiguration_procedure.h"
#include "../rrc_asn1_helpers.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_pdu_session_resource_setup_routine::rrc_pdu_session_resource_setup_routine(
    const cu_cp_pdu_session_resource_setup_message& setup_msg_,
    rrc_ue_context_t&                               context_,
    rrc_ue_e1_control_notifier&                     e1_ctrl_notif_,
    rrc_ue_f1c_control_notifier&                    f1c_ctrl_notif_,
    rrc_ue_reconfiguration_proc_notifier&           rrc_ue_notifier_,
    rrc_ue_event_manager&                           event_mng_,
    srslog::basic_logger&                           logger_) :
  setup_msg(setup_msg_),
  context(context_),
  e1_ctrl_notif(e1_ctrl_notif_),
  f1c_ctrl_notif(f1c_ctrl_notif_),
  rrc_ue_notifier(rrc_ue_notifier_),
  event_mng(event_mng_),
  logger(logger_)
{
  // calculate DRBs that need to added depending on QoSFlowSetupRequestList, more than one DRB could be needed
  drb_to_add_list = context.drb_mng->calculate_drb_to_add_list(setup_msg);
}

void rrc_pdu_session_resource_setup_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_setup_response_message>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("rnti=0x{:x}: \"{}\" initialized.", context.c_rnti, name());

  // initial sanitfy check, making sure we catch implementation limitations
  if (setup_msg.pdu_session_res_setup_items.size() != 1) {
    logger.error("rnti=0x{:x}: \"{}\" supports only one PDU Session ({} requested).",
                 context.c_rnti,
                 name(),
                 setup_msg.pdu_session_res_setup_items.size());
    CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
  }

  // TODO: Retrieve PDCP and SDAP configs

  {
    // prepare BearerContextSetupRequest
    bearer_contest_setup_request.pdu_session_res_to_setup_list.resize(1);

    // call E1 procedure
    CORO_AWAIT_VALUE(bearer_context_setup_response,
                     e1_ctrl_notif.on_bearer_context_setup_request(bearer_contest_setup_request));

    // Handle BearerContextSetupResponse
    if (not bearer_context_setup_response.success) {
      logger.error("rnti=0x{:x}: \"{}\" failed to setup bearer at CU-UP.", context.c_rnti, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // Register required DRB resources at DU
  {
    // prepare UE Context Modification Request and call F1 notifier
    ue_context_mod_request.ue_index = context.ue_index;
    ue_context_mod_request.rrc_ue_drb_setup_msgs.resize(drb_to_add_list.size());
    for (uint32_t i = 0; i < drb_to_add_list.size(); ++i) {
      srsgnb_assert(drb_to_add_list[i] != drb_id_t::invalid, "Invalid DRB ID");

      auto& rrc_ue_drb_setup_message_item  = ue_context_mod_request.rrc_ue_drb_setup_msgs[i];
      rrc_ue_drb_setup_message_item.drb_id = drb_id_to_uint(drb_to_add_list[i]);
      up_transport_layer_info gtp_tunnel;
      gtp_tunnel.gtp_teid = int_to_gtp_teid(0x12345678); // TODO: take from CU-UP response
      rrc_ue_drb_setup_message_item.gtp_tunnels.push_back(gtp_tunnel);

      rrc_ue_drb_setup_message_item.rlc = rlc_mode::am; // TODO: is this coming from FiveQI mapping?

      const auto& mapped_flows = context.drb_mng->get_mapped_qos_flows(drb_to_add_list[i]);
      for (const auto& qos_flow : mapped_flows) {
        qos_flow_setup_request_item mapped_flow = {};
        mapped_flow.qos_flow_id                 = qos_flow;
        rrc_ue_drb_setup_message_item.qos_flows_mapped_to_drb.push_back(mapped_flow);
      }
    }

    CORO_AWAIT_VALUE(ue_context_modification_response,
                     f1c_ctrl_notif.on_new_pdu_session_resource_setup_request(ue_context_mod_request));

    // Handle UE Context Modification Response
    if (not ue_context_modification_response.success) {
      logger.error("rnti=0x{:x}: \"{}\" failed to modify UE context at DU.", context.c_rnti, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // Inform CU-UP about the new TEID for UL F1u traffic
  {
      // prepare BearerContextModificationRequest and call E1 notifier
      // TODO: add modification request to E1AP
      // e1ap_pdu_session_resource_setup_message request;

      // Wait for BearerContextModificationResponse
  }

  {
    // prepare RRC Reconfiguration
    {
      // Add required DRBs to RRC Reconfiguration
      asn1::rrc_nr::radio_bearer_cfg_s radio_bearer_cfg;
      radio_bearer_cfg.drb_to_add_mod_list.resize(drb_to_add_list.size());
      for (uint32_t i = 0; i < drb_to_add_list.size(); ++i) {
        srsgnb_assert(drb_to_add_list[i] != drb_id_t::invalid, "Invalid DRB ID");
        auto& drb_to_add_mod_list_item            = radio_bearer_cfg.drb_to_add_mod_list[i];
        drb_to_add_mod_list_item.drb_id           = drb_id_to_uint(drb_to_add_list[i]);
        drb_to_add_mod_list_item.pdcp_cfg_present = true;
        drb_to_add_mod_list_item.pdcp_cfg         = context.drb_mng->get_pdcp_config(drb_to_add_list[i]);

        // Add CN association and SDAP config
        drb_to_add_mod_list_item.cn_assoc_present = true;
        drb_to_add_mod_list_item.cn_assoc.set_sdap_cfg();
        drb_to_add_mod_list_item.cn_assoc.sdap_cfg() = context.drb_mng->get_sdap_config(drb_to_add_list[i]);
      }
      reconfig_args.radio_bearer_cfg = radio_bearer_cfg;

      // set masterCellGroupConfig as received by DU
      reconfig_args.master_cell_group_config =
          asn1::dyn_octstring(ue_context_modification_response.du_to_cu_rrc_info.cell_group_cfg.copy());

      // append first PDU sessions NAS PDU as received by AMF
      if (not setup_msg.pdu_session_res_setup_items[0].pdu_session_nas_pdu.empty()) {
        reconfig_args.nas_pdu =
            asn1::dyn_octstring(setup_msg.pdu_session_res_setup_items[0].pdu_session_nas_pdu.copy());
      }
    }

    // Trigger reconfig procedure
    CORO_AWAIT_VALUE(
        rrc_reconfig_result,
        launch_async<rrc_reconfiguration_procedure>(context, reconfig_args, rrc_ue_notifier, event_mng, logger));
    if (not rrc_reconfig_result) {
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // we are done, all good
  CORO_RETURN(handle_pdu_session_resource_setup_result(true));
}

cu_cp_pdu_session_resource_setup_response_message
rrc_pdu_session_resource_setup_routine::handle_pdu_session_resource_setup_result(bool success)
{
  if (success) {
    cu_cp_pdu_session_res_setup_response_item item;
    item.pdu_session_id = setup_msg.pdu_session_res_setup_items[0].pdu_session_id;

    auto& transfer                                    = item.pdu_session_resource_setup_response_transfer;
    transfer.dlqos_flow_per_tnl_info.up_tp_layer_info = {transport_layer_address{"127.0.0.1"},
                                                         int_to_gtp_teid(0x12345678)};

    cu_cp_associated_qos_flow qos_flow;
    qos_flow.qos_flow_id = 1;
    transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list.push_back(qos_flow);

    // add to response message
    response_msg.pdu_session_res_setup_response_items.push_back(item);

    logger.debug("rnti=0x{:x}: \"{}\" finalized.", context.c_rnti, name());
  } else {
    // mark all PDU sessions as failed
    for (const auto& setup_item : setup_msg.pdu_session_res_setup_items) {
      cu_cp_pdu_session_res_setup_failed_item item;
      item.pdu_session_id                                         = setup_item.pdu_session_id;
      item.pdu_session_resource_setup_unsuccessful_transfer.cause = cu_cp_cause_t::protocol;
      response_msg.pdu_session_res_failed_to_setup_items.push_back(item);
    }

    logger.error("rnti=0x{:x}: \"{}\" failed.", context.c_rnti, name());
  }

  return response_msg;
}