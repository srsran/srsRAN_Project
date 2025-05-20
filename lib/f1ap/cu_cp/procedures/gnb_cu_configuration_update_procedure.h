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

#include "cu_cp/procedures/f1ap_cu_event_manager.h"
#include "srsran/f1ap/cu_cp/f1ap_configuration.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include "srsran/f1ap/f1ap_message_notifier.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class gnb_cu_configuration_update_procedure
{
public:
  gnb_cu_configuration_update_procedure(const f1ap_configuration&               f1ap_cfg_,
                                        const f1ap_gnb_cu_configuration_update& request_,
                                        f1ap_message_notifier&                  f1ap_notif_,
                                        f1ap_event_manager&                     ev_mng_,
                                        srslog::basic_logger&                   logger_);

  void operator()(coro_context<async_task<f1ap_gnb_cu_configuration_update_response>>& ctx);

  static const char* name() { return "GNB CU Configuration Update Procedure"; }

private:
  /// Send F1 GNB CU Configuration Update to DU.
  void send_gnb_cu_configuration_update();

  /// Creates procedure result to send back to procedure caller.
  f1ap_gnb_cu_configuration_update_response handle_procedure_result();

  const f1ap_configuration&              f1ap_cfg;
  const f1ap_gnb_cu_configuration_update request;
  f1ap_message_notifier&                 f1ap_notifier;
  f1ap_event_manager&                    ev_mng;
  srslog::basic_logger&                  logger;

  f1ap_transaction transaction;

  f1ap_gnb_cu_configuration_update_response response;
};

} // namespace srs_cu_cp
} // namespace srsran
