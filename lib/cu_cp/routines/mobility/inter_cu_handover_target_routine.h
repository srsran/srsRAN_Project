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
#include "srsran/ngap/ngap_handover.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class inter_cu_handover_target_routine
{
public:
  inter_cu_handover_target_routine(const ngap_handover_request&           request_,
                                   du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notif_,
                                   du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
                                   du_processor_interface*                du_proc_,
                                   du_processor_ue_manager&               ue_manager_,
                                   srslog::basic_logger&                  logger_);

  void operator()(coro_context<async_task<ngap_handover_resource_allocation_response>>& ctx);

  static const char* name() { return "Inter CU Handover Target Routine"; }

private:
  void                                       fill_e1ap_bearer_context_setup_request();
  bool                                       generate_security_keys();
  bool                                       create_srb1();
  ngap_handover_resource_allocation_response generate_handover_resource_allocation_response(bool success);

  const ngap_handover_request request;

  du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notifier; // to trigger UE context creation
  du_processor_e1ap_control_notifier&    e1ap_ctrl_notifier;    // to trigger bearer context modification at CU-UP
  du_processor_interface*                du_processor = nullptr;
  du_processor_ue_manager&               ue_manager;
  srslog::basic_logger&                  logger;

  du_ue*                     ue = nullptr;
  up_config_update           next_config;
  security::security_context sec_context;
  security::sec_as_config    security_cfg;

  // (sub-)routine requests
  e1ap_bearer_context_setup_request        bearer_context_setup_request;
  f1ap_ue_context_setup_request            ue_context_setup_request;
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;

  // (sub-)routine results
  ngap_handover_resource_allocation_response response;
  e1ap_bearer_context_setup_response         bearer_context_setup_response;
  f1ap_ue_context_setup_response             ue_context_setup_response;
  e1ap_bearer_context_modification_response  bearer_context_modification_response;
  byte_buffer                                rrc_reconfiguration;
};

} // namespace srs_cu_cp
} // namespace srsran
