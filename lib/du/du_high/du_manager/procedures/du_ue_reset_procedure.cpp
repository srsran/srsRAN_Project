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

#include "du_ue_reset_procedure.h"
#include "../du_ue/du_ue_manager.h"
#include "await_all_ues.h"
#include "ue_deletion_procedure.h"

using namespace srsran;
using namespace srs_du;

du_ue_reset_procedure::du_ue_reset_procedure(const std::vector<du_ue_index_t>&                  ues_to_reset_,
                                             du_ue_manager&                                     ue_mng_,
                                             const du_manager_params&                           du_params_,
                                             const std::optional<f1_reset_request::cause_type>& cause_) :
  ues_to_reset(ues_to_reset_), ue_mng(ue_mng_), du_params(du_params_), cause(cause_)
{
}

void du_ue_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Launch tasks to remove UEs in their respective schedulers.
  CORO_AWAIT(reset_ues());

  if (cause.has_value()) {
    // Trigger F1 Reset towards CU.
    CORO_AWAIT(du_params.f1ap.conn_mng.handle_f1_reset_request(f1_reset_request{ues_to_reset, cause.value()}));
  }

  CORO_RETURN();
}

async_task<void> du_ue_reset_procedure::reset_ues()
{
  if (ues_to_reset.empty()) {
    // Need to delete all UEs. Update ues_to_reset with current UEs.
    auto& ue_db = ue_mng.get_du_ues();
    ues_to_reset.reserve(ue_db.size());
    for (const auto& u : ue_db) {
      ues_to_reset.push_back(u.ue_index);
    }
  }

  // Remove UEs from within their own task scheduler.
  // Note: This is needed to ensure sequential handling of the UE procedures and state.
  return await_all_ues(ue_mng, ues_to_reset, [this](du_ue& u) {
    return launch_async<ue_deletion_procedure>(u.ue_index, ue_mng, du_params);
  });
}
