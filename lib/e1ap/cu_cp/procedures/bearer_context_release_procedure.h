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

#include "../e1ap_cu_cp_impl.h"
#include "../ue_context/e1ap_cu_cp_ue_context.h"
#include "common/e1ap_asn1_utils.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class bearer_context_release_procedure
{
public:
  bearer_context_release_procedure(const ue_index_t       ue_index_,
                                   const e1ap_message&    command_,
                                   e1ap_ue_context_list&  ue_ctxt_list_,
                                   e1ap_message_notifier& e1ap_notif_,
                                   srslog::basic_logger&  logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Bearer Context Release Procedure"; }

private:
  /// Send Bearer Context Release Command to CU-UP.
  void send_bearer_context_release_command();

  /// Handles procedure result and returns back to procedure caller.
  void handle_bearer_context_release_complete();

  const ue_index_t       ue_index;
  const e1ap_message     command;
  e1ap_ue_context_list&  ue_ctxt_list;
  e1ap_message_notifier& e1ap_notifier;
  srslog::basic_logger&  logger;

  protocol_transaction_outcome_observer<asn1::e1ap::bearer_context_release_complete_s> transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsran