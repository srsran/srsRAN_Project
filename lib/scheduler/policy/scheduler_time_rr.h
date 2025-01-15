/*
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "scheduler_policy.h"

namespace srsran {

class scheduler_time_rr : public scheduler_policy
{
public:
  scheduler_time_rr(const scheduler_ue_expert_config& expert_cfg_);

  void dl_sched(ue_pdsch_allocator&          pdsch_alloc,
                const ue_resource_grid_view& res_grid,
                dl_ran_slice_candidate&      slice_candidate,
                dl_harq_pending_retx_list    harq_pending_retx_list) override;

  void ul_sched(ue_pusch_allocator&          pusch_alloc,
                const ue_resource_grid_view& res_grid,
                ul_ran_slice_candidate&      slice_candidate,
                ul_harq_pending_retx_list    harq_pending_retx_list) override;

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
