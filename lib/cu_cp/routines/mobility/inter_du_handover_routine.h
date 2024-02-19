/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../../du_processor/du_processor_impl_interface.h"
#include "../../ue_manager/ue_manager_impl.h"
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
                            du_processor_cu_cp_notifier&           cu_cp_notifier_,
                            du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notif_,
                            du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
                            du_processor_ue_context_notifier&      source_du_processor_notifier_,
                            du_processor_ue_context_notifier&      target_du_processor_notifier_,
                            du_processor_ue_manager&               ue_manager_,
                            srslog::basic_logger&                  logger_);

  void operator()(coro_context<async_task<cu_cp_inter_du_handover_response>>& ctx);

  static const char* name() { return "Inter DU Handover Routine"; }

private:
  bool generate_ue_context_setup_request(f1ap_ue_context_setup_request&               setup_request,
                                         const static_vector<srb_id_t, MAX_NOF_SRBS>& srbs,
                                         const rrc_ue_transfer_context&               transfer_context);
  void create_srb(du_ue* ue, srb_id_t srb_id);

  const cu_cp_inter_du_handover_request command;

  du_ue* source_ue = nullptr; // Pointer to UE in the source DU
  du_ue* target_ue = nullptr; // Pointer to UE in the target DU

  rrc_ue_transfer_context source_rrc_context;

  du_processor_cu_cp_notifier&
      cu_cp_notifier; // to trigger UE removal (if setup fails) or context transfer (if sucessful)
  du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notifier; // to trigger UE context creation at target DU
  du_processor_e1ap_control_notifier&    e1ap_ctrl_notifier; // to trigger bearer context modification at CU-UP
  du_processor_ue_context_notifier&      source_du_processor_notifier; // to remove source UE context
  du_processor_ue_context_notifier&      target_du_processor_notifier; // to remove target UE context
  du_processor_ue_manager&               ue_manager;                   // to remove UE context from source DU processor
  up_config_update                       next_config;
  srslog::basic_logger&                  logger;

  // (sub-)routine requests
  f1ap_ue_context_setup_request            target_ue_context_setup_request;
  f1ap_ue_context_modification_request     source_ue_context_mod_request;
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;
  f1ap_ue_context_release_command          ue_context_release_cmd; // If HO fails target UE context needs to be removed.
  cu_cp_ue_context_release_command ue_context_release_command;     // After succesfull HO source UE needs to be removed.

  // (sub-)routine results
  cu_cp_inter_du_handover_response response_msg;
  f1ap_ue_context_setup_response   target_ue_context_setup_response;
  e1ap_bearer_context_modification_response
      bearer_context_modification_response; // to inform CU-UP about the new TEID for UL F1u traffic
  f1ap_ue_context_modification_response source_ue_context_modification_response;
  bool                                  reconf_result = false;
};

} // namespace srs_cu_cp
} // namespace srsran
