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

#include "../../common/e1ap_asn1_utils.h"
#include "e1_cu_cp_event_manager.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/e1_interface/cu_cp/e1_cu_cp.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

struct f1ap_du_context;

/// CU-CP initiated E1 Setup Procedure as per TS 38.463, 8.2.4.
class cu_cp_e1_setup_procedure
{
public:
  cu_cp_e1_setup_procedure(const cu_cp_e1_setup_request_message& request_,
                           e1_message_notifier&                  cu_up_notif_,
                           e1_event_manager&                     ev_mng_,
                           timer_manager&                        timers,
                           srslog::basic_logger&                 logger_);

  void operator()(coro_context<async_task<cu_cp_e1_setup_response_message>>& ctx);

private:
  /// Send E1 SETUP REQUEST to CU-UP.
  void send_e1_setup_request();

  /// Checks whether the CU-CP should attempt again to connect to CU-UP.
  bool retry_required();

  /// Creates procedure result to send back to procedure caller.
  cu_cp_e1_setup_response_message create_e1_setup_result();

  const cu_cp_e1_setup_request_message request;
  e1_message_notifier&                 cu_up_notifier;
  e1_event_manager&                    ev_mng;
  srslog::basic_logger&                logger;

  unique_timer e1_setup_wait_timer;

  e1ap_transaction transaction;
  unsigned         e1_setup_retry_no = 0;
  unsigned         time_to_wait      = 0;
};

} // namespace srs_cu_cp
} // namespace srsgnb
