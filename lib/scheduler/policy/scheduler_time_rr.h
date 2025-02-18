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
#include "slice_allocator.h"

namespace srsran {

class scheduler_time_rr : public scheduler_policy
{
public:
  scheduler_time_rr(const scheduler_ue_expert_config& expert_cfg_);

  void dl_sched(slice_dl_sched_context& ctxt) override;

  void ul_sched(slice_ul_sched_context& ul_ctxt) override;

private:
  void dl_sched_newtx(slice_dl_sched_context& ctxt);
  void ul_sched_newtx(slice_ul_sched_context& ctxt);

  srslog::basic_logger& logger;
  du_ue_index_t         next_dl_ue_index, next_ul_ue_index;

  const scheduler_ue_expert_config expert_cfg;
};

} // namespace srsran
