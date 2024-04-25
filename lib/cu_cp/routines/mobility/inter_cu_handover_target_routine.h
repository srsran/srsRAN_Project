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
#include "../../du_processor/du_processor.h"
#include "srsran/cu_cp/ue_manager.h"
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
                                   du_processor_cu_cp_notifier&           cu_cp_notifier_,
                                   du_processor_ue_manager&               ue_manager_,
                                   const security_indication_t&           default_security_indication_,
                                   srslog::basic_logger&                  logger_);

  void operator()(coro_context<async_task<ngap_handover_resource_allocation_response>>& ctx);

  static const char* name() { return "Inter CU Handover Target Routine"; }

private:
  bool                              fill_e1ap_bearer_context_setup_request(const security::sec_as_config& sec_info);
  optional<security::sec_as_config> generate_security_keys(security::security_context& sec_context);
  void                              create_srb1();
  ngap_handover_resource_allocation_response generate_handover_resource_allocation_response(bool success);

  const ngap_handover_request request;

  du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notifier; // to trigger UE context creation
  du_processor_e1ap_control_notifier&    e1ap_ctrl_notifier;    // to trigger bearer context modification at CU-UP
  du_processor_cu_cp_notifier&           cu_cp_notifier;        // to trigger UE removal if the UE Context Setup fails
  du_processor_ue_manager&               ue_manager;
  srslog::basic_logger&                  logger;

  du_ue*                            ue = nullptr;
  rrc_ue_transfer_context           rrc_context; //< Passed to new RRC UE upon creation.
  up_config_update                  next_config;
  optional<security::sec_as_config> security_cfg;
  const security_indication_t&      default_security_indication; // default if not signaled via NGAP

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
  byte_buffer                                handover_command_pdu;
};

} // namespace srs_cu_cp
} // namespace srsran
