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
#include "ngc_event_manager.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class ng_setup_procedure
{
public:
  ng_setup_procedure(const ng_setup_request_message& request_,
                     ngc_message_notifier&           cu_notif_,
                     ngc_event_manager&              ev_mng_,
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
  ngc_event_manager&             ev_mng;
  srslog::basic_logger&          logger;

  unsigned ng_setup_retry_no = 0;
  unsigned time_to_wait      = 0;

  ngc_event_manager::ng_setup_outcome_t ng_setup_outcome;
};

} // namespace srs_cu_cp
} // namespace srsgnb
