/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../../ue_manager_impl.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class inter_cu_handover_target_routine
{
public:
  inter_cu_handover_target_routine(const ngap_handover_request&           request_,
                                   du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notif_,
                                   du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
                                   du_processor_ue_manager&               ue_manager_,
                                   srslog::basic_logger&                  logger_);

  void operator()(coro_context<async_task<ngap_handover_resource_allocation_response>>& ctx);

  static const char* name() { return "Inter CU Handover Target Routine"; }

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
  ngap_handover_resource_allocation_response response;
  f1ap_ue_context_setup_response             target_ue_context_setup_response;
};

} // namespace srs_cu_cp
} // namespace srsran
