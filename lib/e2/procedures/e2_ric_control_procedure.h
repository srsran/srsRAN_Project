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

#include "../common/e2ap_asn1_utils.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2ap/e2sm_rc.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_event_manager.h"
#include "srsran/e2/e2_messages.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/e2/e2sm/e2sm_rc.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

class e2_ric_control_procedure
{
public:
  e2_ric_control_procedure(const e2_ric_control_request& request_,
                           e2_message_notifier&          notif_,
                           e2sm_manager&                 e2sm_mng_,
                           srslog::basic_logger&         logger);

  void operator()(coro_context<async_task<void>>& ctx);

  void send_e2_ric_control_acknowledge(const e2_ric_control_request&  ctrl_request,
                                       const e2_ric_control_response& ctrl_response);
  void send_e2_ric_control_failure(const e2_ric_control_request&, const e2_ric_control_response& ctrl_response);

private:
  srslog::basic_logger&        logger;
  e2_message_notifier&         ric_notif;
  e2sm_manager&                e2sm_mng;
  const e2_ric_control_request e2_request;
  e2sm_ric_control_response    e2sm_response;
  e2_ric_control_response      e2_response;
  e2sm_interface*              e2sm_iface;
  e2sm_ric_control_request     ric_ctrl_req;
  e2sm_control_service*        control_service;
};
} // namespace srsran