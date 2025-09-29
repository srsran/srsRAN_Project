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

#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_up {

/// This coroutines handles the E1AP CU UP release procedure as per TS 38.483, 8.2.7.2.2.
class e1ap_cu_up_reset_procedure
{
public:
  e1ap_cu_up_reset_procedure(asn1::e1ap::reset_s          reset_msg_,
                             e1ap_cu_up_manager_notifier& cu_up_notifier_,
                             e1ap_message_notifier&       tx_pdu_notifier_,
                             srslog::basic_logger&        logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "E1AP CU-UP Reset Procedure"; }

  asn1::e1ap::reset_s          reset_msg;
  e1ap_cu_up_manager_notifier& cu_up_notifier;
  e1ap_message_notifier&       tx_pdu_notifier;
  srslog::basic_logger&        logger;
};

} // namespace srs_cu_up
} // namespace srsran
