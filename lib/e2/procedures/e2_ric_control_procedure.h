/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#pragma once

#include "../common/e2ap_asn1_utils.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2ap/e2sm_rc.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_event_manager.h"
#include "srsran/e2/e2_messages.h"
#include "srsran/e2/e2sm/e2sm_du.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/e2/e2sm/e2sm_rc.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

class e2_ric_control_procedure
{
public:
  e2_ric_control_procedure(e2_message_notifier& notif_, e2sm_manager& e2sm_mng_, srslog::basic_logger& logger);

  void run_e2_ric_control_procedure(const e2_ric_control_request& request);

  void send_e2_ric_control_acknowledge(const e2_ric_control_request&  ctrl_request,
                                       const e2_ric_control_response& ctrl_response);
  void send_e2_ric_control_failure(const e2_ric_control_request&, const e2_ric_control_response& ctrl_response);

private:
  srslog::basic_logger& logger;
  e2_message_notifier&  ric_notif;
  e2sm_manager&         e2sm_mng;
};
} // namespace srsran