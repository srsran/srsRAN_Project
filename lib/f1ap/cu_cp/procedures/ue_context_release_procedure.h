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

#include "../f1ap_cu_impl.h"
#include "../ue_context/f1ap_cu_ue_context.h"
#include "common/f1ap_asn1_utils.h"
#include "cu_cp/ue_context/f1ap_cu_ue_transaction_manager.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ue_context_release_procedure
{
public:
  ue_context_release_procedure(const f1ap_ue_context_release_command& cmd_,
                               f1ap_ue_context&                       ue_ctxt_,
                               f1ap_message_notifier&                 f1ap_notif_);

  void operator()(coro_context<async_task<ue_index_t>>& ctx);

  static const char* name() { return "UE Context Release Procedure"; }

private:
  /// Send F1 UE Context Release Command to DU.
  void send_ue_context_release_command();

  /// Creates procedure result to send back to procedure caller.
  ue_index_t create_ue_context_release_complete(const asn1::f1ap::ue_context_release_complete_s& msg);

  f1ap_ue_context&                     ue_ctxt;
  asn1::f1ap::ue_context_release_cmd_s command;
  f1ap_message_notifier&               f1ap_notifier;
  srslog::basic_logger&                logger;

  protocol_transaction_outcome_observer<asn1::f1ap::ue_context_release_complete_s> transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsran