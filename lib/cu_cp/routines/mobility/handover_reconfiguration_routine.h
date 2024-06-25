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

#include "../../ue_manager/ue_manager_impl.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the handover of a UE between two different DUs.
/// TODO Add seqdiag
class handover_reconfiguration_routine
{
public:
  handover_reconfiguration_routine(const rrc_reconfiguration_procedure_request& request_,
                                   const ue_index_t&                            target_ue_index_,
                                   cu_cp_ue&                                    source_ue_,
                                   f1ap_ue_context_manager&                     source_f1ap_ue_ctxt_mng_,
                                   cu_cp_ue_context_manipulation_handler&       cu_cp_handler_,
                                   srslog::basic_logger&                        logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "Handover reconfiguration routine"; }

private:
  void generate_ue_context_modification_request();

  // (sub-)routine requests
  const rrc_reconfiguration_procedure_request request;
  f1ap_ue_context_modification_request        ue_context_mod_request;

  const ue_index_t                       target_ue_index;         // Index of the target UE
  cu_cp_ue&                              source_ue;               // UE in the source DU
  f1ap_ue_context_manager&               source_f1ap_ue_ctxt_mng; // to send UE context modification to source UE
  cu_cp_ue_context_manipulation_handler& cu_cp_handler; // To receive the reconfigurationComplete from target UE

  srslog::basic_logger& logger;

  // (sub-)routine results
  rrc_ue_handover_reconfiguration_context ho_reconf_ctxt;
  f1ap_ue_context_modification_response   ue_context_mod_response;
  bool                                    procedure_result = false;
};

} // namespace srs_cu_cp
} // namespace srsran
