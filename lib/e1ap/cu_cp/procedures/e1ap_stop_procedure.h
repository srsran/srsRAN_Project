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

class e1ap_cu_cp_notifier;
class e1ap_ue_context_list;

/// Procedure called during the shutdown of the E1AP to clear up existing transactions.
class e1ap_stop_procedure
{
public:
  e1ap_stop_procedure(e1ap_cu_cp_notifier& cu_cp_notifier_, e1ap_ue_context_list& ue_ctxt_list_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<void> handle_transaction_info_loss();

  e1ap_cu_cp_notifier&  cu_cp_notifier;
  e1ap_ue_context_list& ue_ctxt_list;
};

} // namespace srs_cu_cp
} // namespace srsran
