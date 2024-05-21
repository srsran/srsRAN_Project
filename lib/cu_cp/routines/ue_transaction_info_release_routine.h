/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../ue_manager/ue_manager_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ue_transaction_info_release_routine
{
public:
  ue_transaction_info_release_routine(const std::vector<ue_index_t>& ues_to_release,
                                      ue_manager&                    ue_mng,
                                      cu_cp_ue_removal_handler&      ue_rem_handler);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  void launch_ue_removal(ue_index_t ue_idx);

  const std::vector<ue_index_t> ues_to_release;
  ue_manager&                   ue_mng;
  cu_cp_ue_removal_handler&     ue_rem_handler;

  unsigned          ues_remaining_count{0};
  manual_event_flag all_ues_reset;
};

} // namespace srs_cu_cp
} // namespace srsran