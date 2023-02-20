/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "scheduler_policy.h"

namespace srsgnb {

struct scheduler_strategy_params {
  std::string           strategy = "time_rr";
  srslog::basic_logger* logger;
};

std::unique_ptr<scheduler_policy> create_scheduler_strategy(const scheduler_strategy_params& params);

} // namespace srsgnb
