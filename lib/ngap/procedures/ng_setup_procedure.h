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

#include "../ngap_asn1_utils.h"
#include "../ngap_context.h"
#include "ngap_transaction_manager.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/async_timer.h"

namespace srsran {
namespace srs_cu_cp {

class ng_setup_procedure
{
public:
  ng_setup_procedure(ngap_context_t&           context_,
                     const ng_setup_request&   request_,
                     ngap_message_notifier&    amf_notif_,
                     ngap_transaction_manager& ev_mng_,
                     timer_factory             timers,
                     srslog::basic_logger&     logger_);

  void operator()(coro_context<async_task<ng_setup_response>>& ctx);

private:
  /// Send NG SETUP REQUEST to AMF.
  void send_ng_setup_request();

  /// Checks whether the NGAP should attempt again to connect to AMF.
  bool retry_required();

  /// Creates procedure result to send back to procedure caller.
  ng_setup_response create_ng_setup_result();

  ngap_context_t&           context;
  const ng_setup_request    request;
  ngap_message_notifier&    amf_notifier;
  ngap_transaction_manager& ev_mng;
  srslog::basic_logger&     logger;

  unique_timer ng_setup_wait_timer;

  unsigned                  ng_setup_retry_no = 0;
  std::chrono::milliseconds time_to_wait{0};

  protocol_transaction_outcome_observer<asn1::ngap::ng_setup_resp_s, asn1::ngap::ng_setup_fail_s> transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsran
