/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  const gnbdu_config_update_request& request;
  f1ap_message_notifier&             cu_notif;
  f1ap_event_manager&                ev_mng;
  srslog::basic_logger&              logger;

  f1ap_transaction transaction;
};

} // namespace srs_du
} // namespace srsran
