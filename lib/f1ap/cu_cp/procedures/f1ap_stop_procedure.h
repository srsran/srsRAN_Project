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

#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class f1ap_du_processor_notifier;
class f1ap_ue_context_list;

/// Procedure called during the shutdown of the F1AP to clear up existing transactions.
class f1ap_stop_procedure
{
public:
  f1ap_stop_procedure(f1ap_du_processor_notifier& cu_cp_notifier_, f1ap_ue_context_list& ue_ctxt_list_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<void> handle_transaction_info_loss();

  f1ap_du_processor_notifier& cu_cp_notifier;
  f1ap_ue_context_list&       ue_ctxt_list;
};

} // namespace srs_cu_cp
} // namespace srsran
