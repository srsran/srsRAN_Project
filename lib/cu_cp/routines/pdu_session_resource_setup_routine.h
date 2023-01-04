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

#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/eager_async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Handles the setup of PDU session resources from the RRC viewpoint.
/// TODO Add seqdiag
class pdu_session_resource_setup_routine
{
public:
  pdu_session_resource_setup_routine(const cu_cp_pdu_session_resource_setup_message& setup_msg_,
                                     du_processor_e1ap_control_notifier&             e1ap_ctrl_notif_,
                                     f1c_ue_context_manager&                         f1c_ue_ctxt_mng_,
                                     rrc_ue_control_message_handler&                 rrc_ue_ctrl_handler_,
                                     drb_manager&                                    rrc_ue_drb_manager_,
                                     srslog::basic_logger&                           logger_);

  void operator()(coro_context<async_task<cu_cp_pdu_session_resource_setup_response_message>>& ctx);

  static const char* name() { return "PDU Session Creation Routine"; }

private:
  cu_cp_pdu_session_resource_setup_response_message handle_pdu_session_resource_setup_result(bool success);

  const cu_cp_pdu_session_resource_setup_message setup_msg;

  std::vector<drb_id_t> drb_to_add_list; // list of DRBs to be added

  du_processor_e1ap_control_notifier& e1ap_ctrl_notifier; // to trigger bearer context setup at CU-UP
  f1c_ue_context_manager&             f1c_ue_ctxt_mng;    // to trigger UE context modification at DU
  rrc_ue_control_message_handler&     rrc_ue_notifier;    // to trigger RRC Reconfiguration at UE
  drb_manager&                        rrc_ue_drb_manager; // to get RRC DRB config
  srslog::basic_logger&               logger;

  // (sub-)routine requests
  e1ap_bearer_context_setup_request           bearer_contest_setup_request;
  cu_cp_ue_context_modification_request       ue_context_mod_request;
  cu_cp_rrc_reconfiguration_procedure_message rrc_reconfig_args;

  // (sub-)routine results
  cu_cp_pdu_session_resource_setup_response_message response_msg;
  e1ap_bearer_context_setup_response     bearer_context_setup_response;    // to initially setup the DRBs at the CU-UP
  cu_cp_ue_context_modification_response ue_context_modification_response; // to inform DU about the new DRBs
  bool                                   rrc_reconfig_result = false;      // the final UE reconfiguration
};

} // namespace srs_cu_cp
} // namespace srsgnb
