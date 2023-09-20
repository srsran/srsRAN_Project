/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "srsran/cu_cp/du_processor.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the modification of an existing PDU session resources.
/// TODO Add seqdiag
class reestablishment_context_modification_routine
{
public:
  reestablishment_context_modification_routine(ue_index_t                                    ue_index_,
                                               du_processor_e1ap_control_notifier&           e1ap_ctrl_notif_,
                                               du_processor_f1ap_ue_context_notifier&        f1ap_ue_ctxt_notif_,
                                               du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier_,
                                               up_resource_manager&  rrc_ue_up_resource_manager_,
                                               srslog::basic_logger& logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "Reestablishment Context Modification Routine"; }

private:
  bool generate_bearer_context_modification_request_for_new_ul_tnl();
  bool generate_ue_context_modification_request(
      f1ap_ue_context_modification_request& ue_context_mod_req,
      const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>&
          e1ap_pdu_session_resource_modify_list);

  bool generate_bearer_context_modification(e1ap_bearer_context_modification_request&        bearer_ctxt_mod_req,
                                            const e1ap_bearer_context_modification_response& bearer_ctxt_mod_resp,
                                            const f1ap_ue_context_modification_response& ue_context_modification_resp,
                                            up_resource_manager&                         up_resource_manager);

  ue_index_t                                    ue_index = ue_index_t::invalid;
  du_processor_e1ap_control_notifier&           e1ap_ctrl_notifier;         // to trigger bearer context setup at CU-UP
  du_processor_f1ap_ue_context_notifier&        f1ap_ue_ctxt_notifier;      // to trigger UE context modification at DU
  du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier;            // to trigger RRC Reconfiguration at UE
  up_resource_manager&                          rrc_ue_up_resource_manager; // to get RRC DRB config
  srslog::basic_logger&                         logger;

  // failure message
  cu_cp_ue_context_release_request ue_context_release_request;

  // (sub-)routine requests
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  f1ap_ue_context_modification_request     ue_context_mod_request;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;

  // (sub-)routine results
  cu_cp_pdu_session_resource_modify_response response_msg;                     // Final routine result.
  f1ap_ue_context_modification_response      ue_context_modification_response; // to inform DU about the new DRBs
  e1ap_bearer_context_modification_response
       bearer_context_modification_response; // to inform CU-UP about the new TEID for UL F1u traffic
  bool rrc_reconfig_result = false;          // the final UE reconfiguration
};

} // namespace srs_cu_cp
} // namespace srsran
