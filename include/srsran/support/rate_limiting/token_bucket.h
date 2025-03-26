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

#include "srsran/support/rate_limiting/token_bucket_config.h"
#include "srsran/support/timers.h"
#include <cstdint>

namespace srsran {

/// \brief Rate limiter class that implements a rate limiter
/// based on the token bucket algorithm.
///
class token_bucket
{
public:
  token_bucket(token_bucket_config cfg);

  /// Consume tokens from the bucket.
  /// \return False if there were not enough available tokens, true otherwise.
  bool consume(uint32_t tokens);

  void stop();

private:
  /// Refill tokens to the bucket.
  /// It will be called internally by the refill timer.
  void refill(uint32_t tokens);

  uint32_t max_tokens;
  uint32_t tokens_in_bucket;

  unique_timer refill_timer;

  bool stopped = false;
};
} // namespace srsran
