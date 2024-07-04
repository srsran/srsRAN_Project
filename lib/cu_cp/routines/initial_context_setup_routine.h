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

#include "../ue_manager/ue_manager_impl.h"
#include "srsran/ngap/ngap_init_context_setup.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the setup of PDU session resources from the RRC viewpoint.
class initial_context_setup_routine
{
public:
  initial_context_setup_routine(const ngap_init_context_setup_request& request_,
                                rrc_ue_interface&                      rrc_ue_,
                                ue_security_manager&                   security_mng_,
                                f1ap_ue_context_manager&               f1ap_ue_ctxt_mng_,
                                cu_cp_ngap_handler&                    pdu_session_setup_handler_,
                                srslog::basic_logger&                  logger_);

  void operator()(
      coro_context<async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>>& ctx);

  static const char* name() { return "Initial Context Setup Routine"; }

  void handle_failure();
  void handle_nas_pdu(byte_buffer nas_pdu);

private:
  ngap_init_context_setup_request request;

  rrc_ue_interface&        rrc_ue;
  ue_security_manager&     security_mng;
  f1ap_ue_context_manager& f1ap_ue_ctxt_mng;          // to trigger UE context setup at F1AP
  cu_cp_ngap_handler&      pdu_session_setup_handler; // to setup PDU sessions
  srslog::basic_logger&    logger;

  // (sub-)routine requests
  rrc_ue_security_mode_command_context  rrc_smc_ctxt;
  f1ap_ue_context_setup_request         ue_context_setup_request;
  rrc_reconfiguration_procedure_request rrc_reconfig_args;

  // (sub-)routine results
  f1ap_ue_context_setup_response            ue_context_setup_response;
  cu_cp_pdu_session_resource_setup_response pdu_session_setup_response;
  bool                                      rrc_reconfig_result = false; // the final UE reconfiguration
  ngap_init_context_setup_failure           fail_msg;
  ngap_init_context_setup_response          resp_msg;
};

} // namespace srs_cu_cp
} // namespace srsran
