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

#include "../../common/f1ap_asn1_utils.h"
#include "../f1ap_cu_impl.h"
#include "../ue_context/f1ap_cu_ue_context.h"
#include "cu_cp/ue_context/f1ap_cu_ue_transaction_manager.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ue_context_modification_procedure
{
public:
  ue_context_modification_procedure(const f1ap_ue_context_modification_request& request_,
                                    f1ap_ue_context&                            ue_ctxt_,
                                    f1ap_message_notifier&                      f1ap_notif_);

  void operator()(coro_context<async_task<f1ap_ue_context_modification_response>>& ctx);

  static const char* name() { return "UE Context Modification Procedure"; }

private:
  /// Send F1 UE Context Modification Request to DU.
  void send_ue_context_modification_request();

  /// Creates procedure result to send back to procedure caller.
  f1ap_ue_context_modification_response create_ue_context_modification_result();

  const f1ap_ue_context_modification_request request;
  f1ap_ue_context&                           ue_ctxt;
  f1ap_message_notifier&                     f1ap_notifier;
  srslog::basic_logger&                      logger;

  protocol_transaction_outcome_observer<asn1::f1ap::ue_context_mod_resp_s, asn1::f1ap::ue_context_mod_fail_s>
      transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsran