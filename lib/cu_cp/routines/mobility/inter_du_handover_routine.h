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

#include "../../ue_manager_impl.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the handover of a UE between two different DUs.
/// TODO Add seqdiag
class inter_du_handover_routine
{
public:
  inter_du_handover_routine(const cu_cp_inter_du_handover_request& command_,
                            du_processor_f1ap_ue_context_notifier& source_du_f1ap_ue_ctxt_notif_,
                            du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notif_,
                            du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
                            du_processor_ue_manager&               ue_manager_,
                            up_resource_manager&                   ue_up_resource_manager_,
                            srslog::basic_logger&                  logger_);

  void operator()(coro_context<async_task<cu_cp_inter_du_handover_response>>& ctx);

  static const char* name() { return "Inter DU Handover Routine"; }

private:
  bool generate_ue_context_setup_request(f1ap_ue_context_setup_request&            setup_request,
                                         const std::map<srb_id_t, cu_srb_context>& srbs);

  const cu_cp_inter_du_handover_request command;

  du_ue* source_ue = nullptr; // Pointer to UE in the source DU
  du_ue* target_ue = nullptr; // Pointer to UE in the target DU

  du_processor_f1ap_ue_context_notifier& source_du_f1ap_ue_ctxt_notifier; // to trigger UE context creation at target DU
  du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notifier; // to trigger UE context creation at target DU
  du_processor_e1ap_control_notifier&    e1ap_ctrl_notifier;     // to trigger bearer context modification at CU-UP
  du_processor_ue_manager&               ue_manager;             // to remove UE context from source DU processor
  up_resource_manager&                   ue_up_resource_manager; // to figure out if there is an active E1 context
  up_config_update                       next_config;
  srslog::basic_logger&                  logger;

  // (sub-)routine requests
  f1ap_ue_context_setup_request            target_ue_context_setup_request;
  f1ap_ue_context_modification_request     source_ue_context_mod_request;
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;
  f1ap_ue_context_release_command target_ue_context_release_cmd; // If HO fails and context needs to be removed again.

  // (sub-)routine results
  cu_cp_inter_du_handover_response response_msg;
  f1ap_ue_context_setup_response   target_ue_context_setup_response;
  e1ap_bearer_context_modification_response
      bearer_context_modification_response; // to inform CU-UP about the new TEID for UL F1u traffic
  f1ap_ue_context_modification_response source_ue_context_modification_response;
  ue_index_t                            target_ue_context_release_result = ue_index_t::invalid;
};

} // namespace srs_cu_cp
} // namespace srsran
