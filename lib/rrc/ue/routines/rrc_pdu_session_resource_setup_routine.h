/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../procedures/rrc_ue_event_manager.h"
#include "../rrc_ue_context.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/rrc/rrc_ue.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/eager_async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Handles the setup of PDU session resources from the RRC viewpoint.
/// TODO Add seqdiag
class rrc_pdu_session_resource_setup_routine
{
public:
  rrc_pdu_session_resource_setup_routine(cu_cp_pdu_session_resource_setup_message& setup_msg_,
                                         rrc_ue_context_t&                         context_,
                                         rrc_ue_e1_control_notifier&               e1_ctrl_notif_,
                                         rrc_ue_f1c_control_notifier&              f1c_ctrl_notif_,
                                         rrc_ue_reconfiguration_proc_notifier&     rrc_ue_notifier_,
                                         rrc_ue_event_manager&                     ev_mng_,
                                         srslog::basic_logger&                     logger_);

  void operator()(coro_context<async_task<cu_cp_pdu_session_resource_setup_response_message>>& ctx);

  static const char* name() { return "RRC PDU Session Creation Routine"; }

private:
  cu_cp_pdu_session_resource_setup_response_message handle_pdu_session_resource_setup_result(bool success);

  cu_cp_pdu_session_resource_setup_message& setup_msg;
  rrc_ue_context_t&                         context;

  std::vector<drb_id_t> drb_to_add_list; // list of DRBs to be added

  rrc_ue_e1_control_notifier&           e1_ctrl_notif;   // to trigger bearer context setup at CU-UP
  rrc_ue_f1c_control_notifier&          f1c_ctrl_notif;  // to trigger UE context modification at DU
  rrc_ue_reconfiguration_proc_notifier& rrc_ue_notifier; // to trigger RRC Reconfiguration at UE
  rrc_ue_event_manager&                 event_mng;       // event manager for the RRC UE entity
  srslog::basic_logger&                 logger;

  // (sub-)routine requests
  e1ap_bearer_context_setup_request              bearer_contest_setup_request;
  rrc_ue_ue_context_modification_request_message ue_context_mod_request;
  rrc_reconfiguration_procedure_args             reconfig_args;

  // (sub-)routine results
  cu_cp_pdu_session_resource_setup_response_message response_msg;
  e1ap_bearer_context_setup_response bearer_context_setup_response; // to initially setup the DRBs at the CU-UP
  rrc_ue_ue_context_modification_response_message ue_context_modification_response; // to inform DU about the new DRBs
  bool                                            rrc_reconfig_result = false;      // the final UE reconfiguration
};

} // namespace srs_cu_cp
} // namespace srsgnb
