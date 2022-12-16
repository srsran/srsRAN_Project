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
    cu_cp_pdu_session_resource_setup_message& setup_msg_,
    rrc_ue_context_t&                         context_,
    rrc_ue_reconfiguration_proc_notifier&     rrc_ue_notifier_,
    rrc_ue_event_manager&                     event_mng_,
    srslog::basic_logger&                     logger_) :
  setup_msg(setup_msg_), context(context_), rrc_ue_notifier(rrc_ue_notifier_), event_mng(event_mng_), logger(logger_)
{
  // Allocate DRB, depending on QoSFlowSetupRequestList, more than one DRB could be needed
  drb_to_add = allocate_new_drb(/* qos params*/);
}

void rrc_pdu_session_resource_setup_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_setup_response_message>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("rnti=0x{:x}: \"{}\" initialized.", context.c_rnti, name());

  // initial sanitfy check, making sure we catch implementation limitations
  if (setup_msg.pdu_session_res_setup_items.size() > 1) {
    logger.error("rnti=0x{:x}: \"{}\" not implemented for more than one PDU Session.", context.c_rnti, name());
    CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
  }

  // TODO: Retrieve PDCP and SDAP configs

  {
    // prepare BearerContextSetupRequest and call E1 notifier
    rrc_ue_bearer_context_setup_request_message request;

    // CORO_AWAIT(e1_notifier->pdu_session_resource_setup_request(context.c_rnti, new_drb.drb_id, new_drb.pdcp_config,
    // new_drb.sdap_config));
    bearer_context_setup_response.success = true;

    // Wait for BearerContextSetupResponse
    if (not bearer_context_setup_response.success) {
      logger.error("rnti=0x{:x}: \"{}\" failed to setup bearer at CU-UP.", context.c_rnti, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }
  }

  // Register required DRB resources at DU
  {
    // prepare UE Context Modification Request and call F1 notifier
    rrc_ue_ue_context_modification_request_message request;

    ue_context_modification_response.success = true;

    // Wait for UE Context Modification Response
    if (not ue_context_modification_response.success) {
      logger.error("rnti=0x{:x}: \"{}\" failed to modify UE context at DU.", context.c_rnti, name());
      CORO_EARLY_RETURN(handle_pdu_session_resource_setup_result(false));
    }

    // store cellGroupConfig, TEID, etc in UE context
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
    // TODO: set field in reconfig_args accordingly

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

    auto& transfer = item.pdu_session_resource_setup_response_transfer;
    transfer.dlqos_flow_per_tnl_info.uptransport_layer_info.transport_layer_address = "127.0.0.1";
    transfer.dlqos_flow_per_tnl_info.uptransport_layer_info.gtp_teid                = 0x12345678;

    cu_cp_associated_qos_flow qos_flow;
    qos_flow.qos_flow_id = 1;
    transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list.push_back(qos_flow);

    // add to response message
    response_msg.pdu_session_res_setup_response_items.push_back(item);

    logger.debug("rnti=0x{:x}: \"{}\" finalized.", context.c_rnti, name());
  } else {
    cu_cp_pdu_session_res_setup_failed_item item;
    item.pdu_session_id = setup_msg.pdu_session_res_setup_items[0].pdu_session_id;
    response_msg.pdu_session_res_failed_to_setup_items.push_back(item);

    logger.error("rnti=0x{:x}: \"{}\" failed.", context.c_rnti, name());
  }

  return response_msg;
}

drb_context rrc_pdu_session_resource_setup_routine::allocate_new_drb()
{
  if (context.drbs.size() >= MAX_NOF_DRBS) {
    logger.error("rnti=0x{:x}: \"{}\" failed. No more DRBs available.", context.c_rnti, name());
    return {};
  }

  drb_id_t new_drb_id = drb_id_t::drb1;
  for (const auto& drbs : context.drbs) {
    if (drbs.drb_id == new_drb_id) {
      /// try next
      new_drb_id = uint_to_drb_id(drb_id_to_uint(new_drb_id) + 1);
      if (new_drb_id == drb_id_t::invalid) {
        logger.error("rnti=0x{:x}: \"{}\" failed. No more DRBs available.", context.c_rnti, name());
        return {};
      }
    } else {
      // found a free DRB
      break;
    }
  }

  drb_context new_drb;
  new_drb.drb_id = new_drb_id;
  context.drbs.push_back(new_drb);

  return new_drb;
}