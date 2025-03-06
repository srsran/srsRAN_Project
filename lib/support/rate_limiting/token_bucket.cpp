/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "token_bucket.h"
#include "srsran/support/compiler.h"
#include <cstdint>

using namespace srsran;

token_bucket::token_bucket(uint32_t                  refill_token,
                           std::chrono::milliseconds refill_period,
                           uint32_t                  max_tokens_,
                           timer_factory             timer_factory) :
  max_tokens(max_tokens_)
{
  refill_timer = timer_factory.create_timer();
  refill_timer.set(refill_period, [this, refill_token]() { refill(refill_token); });
  refill_timer.run();
}

bool token_bucket::consume(const uint32_t tokens)
{
  if (SRSRAN_UNLIKELY(stopped)) {
    return false;
  }

  int32_t tokens_left = tokens_in_bucket - tokens;
  if (tokens_left < 0) {
    return false;
  }
  tokens_in_bucket = tokens_left;

  return true;
}

void token_bucket::refill(const uint32_t tokens)
{
  if (stopped) {
    return;
  }
  uint32_t tokens_added = tokens_in_bucket + tokens;
  tokens_in_bucket      = std::max(max_tokens, tokens_added);

  refill_timer.run(); // restart refill timer
}

void token_bucket::stop()
{
  refill_timer.stop();
}
