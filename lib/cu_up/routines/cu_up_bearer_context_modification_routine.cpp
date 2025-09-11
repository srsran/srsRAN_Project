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

  response.ue_index = ue_ctxt.get_index();
  response.success  = true;

  if (msg.security_info.has_value()) {
    fill_sec_as_config(security_info, msg.security_info.value());

    // Await pending crypto processing to be finished, so that keys
    // can be safely replaced. No more SDUs/PDUs will arrive at the PDCP as this procedure will
    // block the process of further packets to this UE.
    ue_ctxt.notify_pdcp_pdu_processing_stopped();
    CORO_AWAIT(ue_ctxt.await_rx_crypto_tasks());
    CORO_AWAIT(ue_ctxt.await_tx_crypto_tasks());

    // SDU/PDU processing is finished, safely update the keys now.
    ue_ctxt.set_security_config(security_info);
    ue_ctxt.restart_pdcp_pdu_processing();
  }

  if (not msg.ng_ran_bearer_context_mod_request.has_value()) {
    ue_ctxt.get_logger().log_warning("Bearer Context Modification Request does not setup/modify any NR PDU sessions");
    CORO_EARLY_RETURN(response);
  }

  // Traverse list of PDU sessions to be setup/modified
  for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_setup_mod_list) {
    ue_ctxt.get_logger().log_debug("Setup/Modification of {}", pdu_session_item.pdu_session_id);
    pdu_session_setup_result session_result = ue_ctxt.setup_pdu_session(pdu_session_item);
    process_successful_pdu_resource_setup_mod_outcome(response.pdu_session_resource_setup_list, session_result);
    response.success &= session_result.success; // Update final result.
  }

  // Traverse list of PDU sessions to be modified.
  for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list) {
    ue_ctxt.get_logger().log_debug("Modifying {}", pdu_session_item.pdu_session_id);
    pdu_session_modification_result session_result = ue_ctxt.modify_pdu_session(
        pdu_session_item, msg.new_ul_tnl_info_required.has_value() && msg.new_ul_tnl_info_required.value());
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

  // 3. Create response
  response.success = true;
  CORO_RETURN(response);
}
