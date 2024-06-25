/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "scheduler_policy.h"

namespace srsran {

std::unique_ptr<scheduler_policy> create_scheduler_strategy(const scheduler_ue_expert_config& expert_cfg_);

} // namespace srsran
