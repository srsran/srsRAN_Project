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

#include "srsran/support/rate_limiting/rate_limiter.h"
#include "srsran/support/rate_limiting/token_bucket_config.h"
#include "srsran/support/timers.h"
#include <cstdint>

namespace srsran {

/// \brief Rate limiter class that implements a rate limiter
/// based on the token bucket algorithm.
///
class token_bucket final : public rate_limiter
{
public:
  token_bucket(token_bucket_config cfg);

  bool consume(uint32_t tokens) override;

  void stop() override;

private:
  void refill(uint32_t tokens);

  uint32_t max_tokens;
  uint32_t tokens_in_bucket;

  unique_timer refill_timer;

  bool stopped = false;
};

} // namespace srsran
