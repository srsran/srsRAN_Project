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

#include "scheduler_policy.h"

namespace srsran {

class scheduler_time_rr : public scheduler_policy
{
public:
  scheduler_time_rr(const scheduler_ue_expert_config& expert_cfg_);

  void dl_sched(dl_sched_context dl_ctxt) override;

  void ul_sched(ul_sched_context ul_ctxt) override;

private:
  void dl_sched_newtx(ue_pdsch_allocator&          pdsch_alloc,
                      const ue_resource_grid_view& res_grid,
                      dl_ran_slice_candidate&      slice_candidate);
  void ul_sched_newtx(ue_pusch_allocator&          pusch_alloc,
                      const ue_resource_grid_view& res_grid,
                      ul_ran_slice_candidate&      slice_candidate);

  srslog::basic_logger& logger;
  du_ue_index_t         next_dl_ue_index, next_ul_ue_index;

  const scheduler_ue_expert_config expert_cfg;
};

} // namespace srsran
