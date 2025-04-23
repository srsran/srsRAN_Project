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
