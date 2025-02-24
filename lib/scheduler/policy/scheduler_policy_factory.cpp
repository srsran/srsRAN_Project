/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_policy_factory.h"
#include "scheduler_time_qos.h"
#include "scheduler_time_rr.h"

using namespace srsran;

std::unique_ptr<scheduler_policy> srsran::create_scheduler_strategy(const scheduler_ue_expert_config& expert_cfg_,
                                                                    du_cell_index_t                   cell_index)
{
  if (std::holds_alternative<time_rr_scheduler_expert_config>(expert_cfg_.strategy_cfg)) {
    return std::make_unique<scheduler_time_rr>(expert_cfg_);
  }
  if (std::holds_alternative<time_qos_scheduler_expert_config>(expert_cfg_.strategy_cfg)) {
    return std::make_unique<scheduler_time_qos>(expert_cfg_, cell_index);
  }
  return nullptr;
}
