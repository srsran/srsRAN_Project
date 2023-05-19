/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdu_session_resource_modification_routine.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

pdu_session_resource_modification_routine::pdu_session_resource_modification_routine(
    const cu_cp_pdu_session_resource_modify_request& modify_request_,
    du_processor_e1ap_control_notifier&              e1ap_ctrl_notif_,
    du_processor_f1ap_ue_context_notifier&           f1ap_ue_ctxt_notif_,
    up_resource_manager&                             rrc_ue_up_resource_manager_,
    srslog::basic_logger&                            logger_) :
  modify_request(modify_request_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  rrc_ue_up_resource_manager(rrc_ue_up_resource_manager_),
  logger(logger_)
{
}

void pdu_session_resource_modification_routine::operator()(
    coro_context<async_task<cu_cp_pdu_session_resource_modify_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", modify_request.ue_index, name());

  // Perform initial sanity checks.
  if (modify_request.pdu_session_res_modify_items.empty()) {
    logger.info("ue={}: \"{}\" Skipping empty PDU Session Resource Modification", modify_request.ue_index, name());
    CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
  }

  for (const auto& modify_item : modify_request.pdu_session_res_modify_items) {
    if (!rrc_ue_up_resource_manager.has_pdu_session(modify_item.pdu_session_id)) {
      logger.error("ue={}: \"{}\" PDU session ID {} doesn't exist.",
                   modify_request.ue_index,
                   name(),
                   modify_item.pdu_session_id);
      CORO_EARLY_RETURN(generate_pdu_session_resource_modify_response(false));
    }
  }

  {
    // Calculate next user-plane configuration based on incoming setup message.
    // next_config = rrc_ue_up_resource_manager.calculate_update(setup_msg);
  }

  // we are done
  CORO_RETURN(generate_pdu_session_resource_modify_response(true));
}

// Helper to mark all PDU sessions that were requested to be set up as failed.
void mark_all_sessions_as_failed(cu_cp_pdu_session_resource_modify_response&      response_msg,
                                 const cu_cp_pdu_session_resource_modify_request& modify_request)
{
  for (const auto& modify_item : modify_request.pdu_session_res_modify_items) {
    cu_cp_pdu_session_resource_failed_to_modify_item failed_item;
    failed_item.pdu_session_id = modify_item.pdu_session_id;
    response_msg.pdu_session_res_failed_to_modify_list.push_back(failed_item);
  }
}

cu_cp_pdu_session_resource_modify_response
pdu_session_resource_modification_routine::generate_pdu_session_resource_modify_response(bool success)
{
  if (success) {
    logger.debug("ue={}: \"{}\" finalized.", modify_request.ue_index, name());

    // TODO: Prepare update for UP resource manager.
    up_config_update_result result;
    rrc_ue_up_resource_manager.apply_config_update(result);
  } else {
    logger.error("ue={}: \"{}\" failed.", modify_request.ue_index, name());
    mark_all_sessions_as_failed(response_msg, modify_request);
  }
  return response_msg;
}
