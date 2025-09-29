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

#pragma once

#include "../../cu_cp_impl_interface.h"
#include "../../mobility_manager/mobility_manager_impl.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the handover of a UE between two different cells managed by the same CU.
/// TODO Add seqdiag
class intra_cu_handover_routine
{
public:
  intra_cu_handover_routine(const cu_cp_intra_cu_handover_request& request_,
                            const byte_buffer&                     target_cell_sib1_,
                            f1ap_ue_context_manager&               source_du_f1ap_ue_ctxt_mng_,
                            f1ap_ue_context_manager&               target_du_f1ap_ue_ctxt_mng_,
                            cu_cp_impl_interface&                  cu_cp_handler_,
                            ue_manager&                            ue_mng_,
                            mobility_manager&                      mobility_mng_,
                            srslog::basic_logger&                  logger_);

  void operator()(coro_context<async_task<cu_cp_intra_cu_handover_response>>& ctx);

  static const char* name() { return "Intra CU Handover Routine"; }

private:
  bool generate_ue_context_setup_request(f1ap_ue_context_setup_request&               setup_request,
                                         const static_vector<srb_id_t, MAX_NOF_SRBS>& srbs,
                                         const rrc_ue_transfer_context&               transfer_context);
  void create_srb(cu_cp_ue* ue, srb_id_t srb_id);

  bool add_security_context_to_bearer_context_modification(const srsran::security::sec_as_config& security_cfg);

  const cu_cp_intra_cu_handover_request request;
  const byte_buffer                     target_cell_sib1;

  cu_cp_ue* source_ue = nullptr; // Pointer to UE in the source DU
  cu_cp_ue* target_ue = nullptr; // Pointer to UE in the target DU

  rrc_ue_transfer_context source_rrc_context;

  f1ap_ue_context_manager& source_du_f1ap_ue_ctxt_mng; // to trigger UE context modification at source DU
  f1ap_ue_context_manager& target_du_f1ap_ue_ctxt_mng; // to trigger UE context creation at target DU
  cu_cp_impl_interface&    cu_cp_handler;              // to trigger UE context transfer (if sucessful)
  ue_manager&              ue_mng;                     // to remove UE context from source DU processor
  mobility_manager&        mobility_mng;               // to notify metrics about handover execution
  up_config_update         next_config;
  srslog::basic_logger&    logger;

  // (sub-)routine requests
  f1ap_ue_context_setup_request            target_ue_context_setup_request;
  f1ap_ue_context_modification_request     source_ue_context_mod_request;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  f1ap_ue_context_release_command          ue_context_release_cmd; // If HO fails target UE context needs to be removed.
  cu_cp_ue_context_release_command ue_context_release_command;     // After succesfull HO source UE needs to be removed.

  // (sub-)routine results
  cu_cp_intra_cu_handover_response      response_msg;
  f1ap_ue_context_setup_response        target_ue_context_setup_response;
  f1ap_ue_context_modification_response source_ue_context_modification_response;
  bool                                  rrc_reconfig_sent = false;
};

} // namespace srs_cu_cp
} // namespace srsran
