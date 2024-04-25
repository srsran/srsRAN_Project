/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../du_processor/du_processor.h"
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
