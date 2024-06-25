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
#include "srsran/ngap/ngap_handover.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class inter_cu_handover_target_routine
{
public:
  inter_cu_handover_target_routine(const ngap_handover_request& request_,
                                   e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng_,
                                   f1ap_ue_context_manager&     f1ap_ue_ctxt_mng_,
                                   cu_cp_ue_removal_handler&    ue_removal_handler_,
                                   ue_manager&                  ue_mng_,
                                   const security_indication_t& default_security_indication_,
                                   srslog::basic_logger&        logger_);

  void operator()(coro_context<async_task<ngap_handover_resource_allocation_response>>& ctx);

  static const char* name() { return "Inter CU Handover Target Routine"; }

private:
  bool fill_e1ap_bearer_context_setup_request(const security::sec_as_config& sec_info);
  void create_srb1();
  ngap_handover_resource_allocation_response generate_handover_resource_allocation_response(bool success);

  const ngap_handover_request request;

  e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng; // to trigger bearer context modification at CU-UP
  f1ap_ue_context_manager&     f1ap_ue_ctxt_mng;     // to trigger UE context creation
  cu_cp_ue_removal_handler&    ue_removal_handler;   // to trigger UE removal if the UE Context Setup fails
  ue_manager&                  ue_mng;

  srslog::basic_logger& logger;

  cu_cp_ue*                    ue = nullptr;
  rrc_ue_transfer_context      rrc_context; //< Passed to new RRC UE upon creation.
  up_config_update             next_config;
  const security_indication_t& default_security_indication; // default if not signaled via NGAP

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
