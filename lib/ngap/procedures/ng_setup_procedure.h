/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ngap_transaction_manager.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_context.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ng_setup_procedure
{
public:
  ng_setup_procedure(ngap_context_t&           context_,
                     const ngap_message&       request_,
                     const unsigned            max_setup_retries_,
                     ngap_message_notifier&    amf_notif_,
                     ngap_transaction_manager& ev_mng_,
                     timer_factory             timers,
                     srslog::basic_logger&     logger_);

  void operator()(coro_context<async_task<ngap_ng_setup_result>>& ctx);

  static const char* name() { return "NG Setup Procedure"; }

private:
  /// Checks whether the NGAP should attempt again to connect to AMF.
  bool retry_required();

  /// Creates procedure result to send back to procedure caller.
  ngap_ng_setup_result create_ng_setup_result();

  static bool is_failure_misconfiguration(const asn1::ngap::cause_c& cause);

  ngap_context_t&           context;
  const ngap_message        request;
  const unsigned            max_setup_retries;
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
