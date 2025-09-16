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

#include "cu_cp/ue_context/f1ap_cu_ue_context.h"
#include "f1ap_cu_event_manager.h"
#include "srsran/f1ap/cu_cp/f1ap_configuration.h"
#include "srsran/f1ap/f1ap_message_notifier.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handle the CU-CP initiated F1 Reset Procedure as per TS 38.473 section 8.2.1.
class cu_cp_f1_reset_procedure
{
public:
  cu_cp_f1_reset_procedure(const f1ap_configuration& f1ap_cfg_,
                           const cu_cp_reset&        reset_,
                           f1ap_message_notifier&    pdu_notifier_,
                           f1ap_event_manager&       ev_mng_,
                           f1ap_ue_context_list&     ue_ctxt_list_,
                           srslog::basic_logger&     logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "F1 Reset Procedure"; }

  void send_f1_reset();

  const f1ap_configuration& f1ap_cfg;
  const cu_cp_reset&        reset;
  f1ap_message_notifier&    pdu_notifier;
  f1ap_event_manager&       ev_mng;
  f1ap_ue_context_list&     ue_ctxt_list;
  srslog::basic_logger&     logger;

  f1ap_transaction transaction;
};

} // namespace srs_cu_cp
} // namespace srsran
