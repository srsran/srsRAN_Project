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
#include "ue_deletion_procedure.h"

using namespace srsran;
using namespace srs_du;

du_ue_reset_procedure::du_ue_reset_procedure(const std::vector<du_ue_index_t>& ues_to_reset_,
                                             du_ue_manager&                    ue_mng_,
                                             const du_manager_params&          du_params_) :
  ues_to_reset(ues_to_reset_), ue_mng(ue_mng_), du_params(du_params_)
{
}

void du_ue_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Launch tasks to remove UEs in their respective schedulers.
  launch_rem_ues_tasks();

  // Wait for all removal tasks to complete.
  CORO_AWAIT(complete_flag);

  CORO_RETURN();
}

void du_ue_reset_procedure::launch_rem_ues_tasks()
{
  if (ues_to_reset.empty()) {
    // Need to delete all UEs. Update ues_to_reset with current UEs.
    unsigned nof_ues = ue_mng.nof_ues();
    ues_to_reset.reserve(nof_ues);
    for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
      if (ue_mng.find_ue(to_du_ue_index(i)) != nullptr) {
        ues_to_reset.push_back(to_du_ue_index(i));
        if (ues_to_reset.size() == nof_ues) {
          break;
        }
      }
    }
  }

  ue_rem = ues_to_reset.size();
  for (unsigned ue_count = 0; ue_count != ues_to_reset.size(); ++ue_count) {
    ue_mng.schedule_async_task(ues_to_reset[ue_count], launch_ue_rem_task(ues_to_reset[ue_count]));
  }
}

async_task<void> du_ue_reset_procedure::launch_ue_rem_task(du_ue_index_t ue_index)
{
  return launch_async([this, ue_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    CORO_AWAIT(launch_async<ue_deletion_procedure>(ue_index, ue_mng, du_params));

    ue_rem--;
    if (ue_rem == 0) {
      // This is the last task to remove UEs. Flag completion.
      complete_flag.set();
    }

    CORO_RETURN();
  });
}
