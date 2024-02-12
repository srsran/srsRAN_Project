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

#include "../du_processor/du_processor_impl_interface.h"
#include "../ue_manager/ue_manager_impl.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the setup of PDU session resources from the RRC viewpoint.
/// TODO Add seqdiag
class ue_context_release_routine
{
public:
  ue_context_release_routine(const cu_cp_ue_context_release_command& command_,
                             du_processor_e1ap_control_notifier&     e1ap_ctrl_notif_,
                             du_processor_f1ap_ue_context_notifier&  f1ap_ue_ctxt_notif_,
                             du_processor_cu_cp_notifier&            cu_cp_notifier_,
                             du_processor_ue_manager&                ue_manager_,
                             srslog::basic_logger&                   logger_);

  void operator()(coro_context<async_task<cu_cp_ue_context_release_complete>>& ctx);

  static const char* name() { return "UE Context Release Routine"; }

private:
  const cu_cp_ue_context_release_command command;

  du_processor_e1ap_control_notifier&    e1ap_ctrl_notifier;    // to trigger bearer context setup at CU-UP
  du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notifier; // to trigger UE context modification at DU
  du_processor_cu_cp_notifier&           cu_cp_notifier;        // to remove UE
  du_processor_ue_manager&               ue_manager;
  srslog::basic_logger&                  logger;

  // (sub-)routine requests
  rrc_ue_release_context              release_context;
  f1ap_ue_context_release_command     f1ap_ue_context_release_cmd;
  e1ap_bearer_context_release_command bearer_context_release_command;

  // (sub-)routine results
  ue_index_t                        f1ap_ue_context_release_result;
  cu_cp_ue_context_release_complete release_complete;
};

} // namespace srs_cu_cp
} // namespace srsran
