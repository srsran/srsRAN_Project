/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "f1ap_du_event_manager.h"
#include "srsran/f1ap/du/f1ap_du_connection_manager.h"
#include "srsran/f1ap/f1ap_message_notifier.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

/// DU config update procedure for the gNB-DU as per TS 38.473, 8.2.4.
class f1ap_du_gnbdu_config_update_procedure
{
public:
  f1ap_du_gnbdu_config_update_procedure(const gnbdu_config_update_request& request_,
                                        f1ap_message_notifier&             cu_notif_,
                                        f1ap_event_manager&                ev_mng_);

  void operator()(coro_context<async_task<gnbdu_config_update_response>>& ctx);

private:
  const char* name() const { return "gNB-DU Configuration Update"; }

  void send_gnbdu_cu_update_request();

  gnbdu_config_update_response generate_du_response() const;

  const gnbdu_config_update_request request;
  f1ap_message_notifier&            cu_notif;
  f1ap_event_manager&               ev_mng;
  srslog::basic_logger&             logger;

  f1ap_transaction transaction;
};

} // namespace srs_du
} // namespace srsran
