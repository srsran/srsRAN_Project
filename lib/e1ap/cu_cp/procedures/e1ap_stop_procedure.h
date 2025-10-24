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
