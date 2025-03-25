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
#include "srsran/support/srsran_assert.h"
#include <cstdint>

using namespace srsran;

token_bucket::token_bucket(token_bucket_config cfg) : max_tokens(cfg.max_tokens), tokens_in_bucket(cfg.max_tokens)
{
  srsran_assert(cfg.refill_token != 0, "Incorrectly configured bucket. refill_token=0 ");
  srsran_assert(cfg.max_tokens != 0, "Incorrectly configured bucket. max_tokens=0 ");
  srsran_assert(cfg.refill_token <= max_tokens,
                "Incorrectly configured bucket. refill_token={} > max_tokens={}",
                cfg.refill_token,
                cfg.max_tokens);
  refill_timer = cfg.timer_f.create_timer();
  refill_timer.set(cfg.refill_period,
                   [this, refill_token = cfg.refill_token](timer_id_t /*timer_id*/) { refill(refill_token); });
  refill_timer.run();
}

bool token_bucket::consume(uint32_t tokens)
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

void token_bucket::refill(uint32_t tokens)
{
  if (stopped) {
    return;
  }
  uint32_t tokens_added = tokens_in_bucket + tokens;
  tokens_in_bucket      = std::min(max_tokens, tokens_added);

  refill_timer.run(); // restart refill timer
}

void token_bucket::stop()
{
  if (not stopped) {
    refill_timer.stop();
    stopped = true;
  }
}
