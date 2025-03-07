/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/timers.h"
#include <chrono>

namespace srsran {

struct token_bucket_config {
  uint32_t                  refill_token;
  std::chrono::milliseconds refill_period;
  uint32_t                  max_tokens;
  timer_factory             timer_f;
};

inline token_bucket_config generate_token_bucket_config(uint64_t                  avg_rate_bps,
                                                        uint64_t                  max_rate_bps,
                                                        std::chrono::milliseconds refill_period,
                                                        timer_factory             timer_f)
{
  token_bucket_config cfg = {};
  cfg.refill_period       = refill_period;
  cfg.refill_token        = avg_rate_bps * refill_period.count() / 8 * 1e-3;
  cfg.max_tokens          = max_rate_bps * refill_period.count() / 8 * 1e-3;
  cfg.timer_f             = timer_f;
  return cfg;
}

} // namespace srsran
