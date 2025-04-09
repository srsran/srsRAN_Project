/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_bearer_context_modification_routine.h"
#include "../cu_up_manager_helpers.h"

using namespace srsran;
using namespace srs_cu_up;

cu_up_bearer_context_modification_routine ::cu_up_bearer_context_modification_routine(
    ue_context&                                     ue_ctxt_,
    const e1ap_bearer_context_modification_request& msg_) :
  ue_ctxt(ue_ctxt_), msg(msg_), logger(srslog::fetch_basic_logger("CU-UP"))
{
}

void cu_up_bearer_context_modification_routine::operator()(
    coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx)
{
  CORO_BEGIN(ctx);

  ue_ctxt.get_logger().log_debug("Handling BearerContextModificationRequest");

  e1ap_bearer_context_modification_response response = {};
  response.ue_index                                  = ue_ctxt.get_index();
  response.success                                   = true;

  bool new_ul_tnl_info_required = (msg.new_ul_tnl_info_required.has_value() && msg.new_ul_tnl_info_required.value());

  if (msg.security_info.has_value()) {
    security::sec_as_config security_info;
    fill_sec_as_config(security_info, msg.security_info.value());
    ue_ctxt.set_security_config(security_info);
  }

  if (msg.ng_ran_bearer_context_mod_request.has_value()) {
    // Traverse list of PDU sessions to be setup/modified
    for (const auto& pdu_session_item :
         msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_setup_mod_list) {
      ue_ctxt.get_logger().log_debug("Setup/Modification of {}", pdu_session_item.pdu_session_id);
      pdu_session_setup_result session_result = ue_ctxt.setup_pdu_session(pdu_session_item);
      process_successful_pdu_resource_setup_mod_outcome(response.pdu_session_resource_setup_list, session_result);
      response.success &= session_result.success; // Update final result.
    }

    // Traverse list of PDU sessions to be modified.
    for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list) {
      ue_ctxt.get_logger().log_debug("Modifying {}", pdu_session_item.pdu_session_id);
      pdu_session_modification_result session_result =
          ue_ctxt.modify_pdu_session(pdu_session_item, new_ul_tnl_info_required);
      process_successful_pdu_resource_modification_outcome(response.pdu_session_resource_modified_list,
                                                           response.pdu_session_resource_failed_to_modify_list,
                                                           session_result,
                                                           logger);
      ue_ctxt.get_logger().log_debug("Modification {}", session_result.success ? "successful" : "failed");

      response.success &= session_result.success; // Update final result.
    }

    // Traverse list of PDU sessions to be removed.
    for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_rem_list) {
      ue_ctxt.get_logger().log_info("Removing {}", pdu_session_item);
      ue_ctxt.remove_pdu_session(pdu_session_item);
      // There is no IE to confirm successful removal.
    }
  } else {
    ue_ctxt.get_logger().log_warning("Ignoring empty Bearer Context Modification Request");
  }

  // 3. Create response
  response.success = true;
  CORO_RETURN(response);
}
