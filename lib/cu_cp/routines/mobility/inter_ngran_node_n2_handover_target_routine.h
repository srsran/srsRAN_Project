/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../ue_manager_impl.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class inter_ngran_node_n2_handover_target_routine
{
public:
  inter_ngran_node_n2_handover_target_routine(const ngap_handover_request&           request_,
                                              du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notif_,
                                              du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
                                              du_processor_ue_manager&               ue_manager_,
                                              srslog::basic_logger&                  logger_);

  void operator()(coro_context<async_task<cu_cp_inter_ngran_node_n2_handover_target_response>>& ctx);

  static const char* name() { return "Inter NGRAN Node N2Handover Target Routine"; }

private:
  bool generate_ue_context_setup_request(f1ap_ue_context_setup_request& setup_request,
                                         const up_config_update&        up_config);

  const ngap_handover_request request;

  du_ue*           ue = nullptr;
  up_config_update next_config;

  du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notifier; // to trigger UE context creation
  du_processor_e1ap_control_notifier&    e1ap_ctrl_notifier;    // to trigger bearer context modification at CU-UP
  du_processor_ue_manager&               ue_manager;
  srslog::basic_logger&                  logger;

  // (sub-)routine requests
  f1ap_ue_context_setup_request            target_ue_context_setup_request;
  e1ap_bearer_context_modification_request bearer_context_modification_command;

  // (sub-)routine results
  cu_cp_inter_ngran_node_n2_handover_target_response response = {false};
  f1ap_ue_context_setup_response                     target_ue_context_setup_response;
};

} // namespace srs_cu_cp
} // namespace srsran
