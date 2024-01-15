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
#include "f1ap_du_event_manager.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

struct f1ap_du_context;

/// F1 Setup Procedure for the gNB-DU as per TS 38.473, 8.2.3.
class f1ap_du_setup_procedure
{
public:
  f1ap_du_setup_procedure(const f1_setup_request_message& request_,
                          f1ap_message_notifier&          cu_notif_,
                          f1ap_event_manager&             ev_mng_,
                          timer_factory&                  timers,
                          f1ap_du_context&                du_ctxt_);

  void operator()(coro_context<async_task<f1_setup_response_message>>& ctx);

private:
  /// Send F1 SETUP REQUEST to CU.
  void send_f1_setup_request();

  /// Checks whether the DU should attempt again to connect to CU.
  bool retry_required();

  /// Creates procedure result to send back to procedure caller.
  f1_setup_response_message create_f1_setup_result();

  const f1_setup_request_message request;
  f1ap_message_notifier&         cu_notifier;
  f1ap_event_manager&            ev_mng;
  srslog::basic_logger&          logger;
  f1ap_du_context&               du_ctxt;

  unique_timer f1_setup_wait_timer;

  f1ap_transaction     transaction;
  unsigned             f1_setup_retry_no = 0;
  std::chrono::seconds time_to_wait{0};
}; // namespace srs_du

} // namespace srs_du
} // namespace srsran
