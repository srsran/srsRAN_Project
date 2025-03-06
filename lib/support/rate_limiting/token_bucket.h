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
#include <cstdint>

namespace srsran {

/// \brief Rate limiter class that implements a rate limiter
/// based on the token bucket algorithm.
///
class token_bucket
{
public:
  token_bucket(uint32_t                  refill_token,
               std::chrono::milliseconds refill_period,
               uint32_t                  max_tokens,
               timer_factory             timer_factory);

  bool consume(uint32_t tokens);

  void stop();

private:
  void refill(uint32_t tokens);

  uint32_t max_tokens;
  uint32_t tokens_in_bucket;

  unique_timer refill_timer;

  bool stopped = false;
};

} // namespace srsran
