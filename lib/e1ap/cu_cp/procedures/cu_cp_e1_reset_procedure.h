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

#include "cu_cp/procedures/e1ap_transaction_manager.h"
#include "cu_cp/ue_context/e1ap_cu_cp_ue_context.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_cp/e1ap_configuration.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handle the CU-CP initiated E1 Reset Procedure as per TS 38.483 section 8.2.1.
class cu_cp_e1_reset_procedure
{
public:
  cu_cp_e1_reset_procedure(const e1ap_configuration& e1ap_cfg_,
                           const cu_cp_reset&        reset_,
                           e1ap_message_notifier&    pdu_notifier_,
                           e1ap_transaction_manager& ev_mng_,
                           e1ap_ue_context_list&     ue_ctxt_list_,
                           srslog::basic_logger&     logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "E1 Reset Procedure"; }

  void send_e1_reset();

  const e1ap_configuration  e1ap_cfg;
  const cu_cp_reset         reset;
  e1ap_message_notifier&    pdu_notifier;
  e1ap_transaction_manager& ev_mng;
  e1ap_ue_context_list&     ue_ctxt_list;
  srslog::basic_logger&     logger;

  e1ap_transaction transaction;
};

} // namespace srs_cu_cp
} // namespace srsran
