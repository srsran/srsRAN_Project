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

#include "../ngap_asn1_utils.h"
#include "ngap_transaction_manager.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/async_timer.h"

namespace srsgnb {
namespace srs_cu_cp {

class ng_setup_procedure
{
public:
  ng_setup_procedure(const ng_setup_request_message& request_,
                     ngc_message_notifier&           amf_notif_,
                     ngap_transaction_manager&       ev_mng_,
                     timer_manager&                  timers,
                     srslog::basic_logger&           logger_);

  void operator()(coro_context<async_task<ng_setup_response_message>>& ctx);

private:
  /// Send NG SETUP REQUEST to AMF.
  void send_ng_setup_request();

  /// Checks whether the NGC should attempt again to connect to AMF.
  bool retry_required();

  /// Creates procedure result to send back to procedure caller.
  ng_setup_response_message create_ng_setup_result();

  const ng_setup_request_message request;
  ngc_message_notifier&          amf_notifier;
  ngap_transaction_manager&      ev_mng;
  srslog::basic_logger&          logger;

  unique_timer ng_setup_wait_timer;

  unsigned ng_setup_retry_no = 0;
  unsigned time_to_wait      = 0;

  protocol_transaction_outcome_observer<asn1::ngap::ng_setup_resp_s, asn1::ngap::ng_setup_fail_s> transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsgnb
