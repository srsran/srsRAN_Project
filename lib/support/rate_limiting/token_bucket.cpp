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

#include "srsran/support/rate_limiting/token_bucket.h"
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

  if (not refill_timer.is_running()) {
    refill_timer.run(); // restart refill timer
  }

  return true;
}

void token_bucket::refill(uint32_t tokens)
{
  if (stopped) {
    return;
  }
  uint32_t tokens_added = tokens_in_bucket + tokens;
  tokens_in_bucket      = std::min(max_tokens, tokens_added);
  if (tokens_in_bucket == max_tokens) {
    refill_timer.stop(); // stop refill timer. it will be restarted on the next consume.
  } else {
    refill_timer.run(); // restart refill timer
  }
}

void token_bucket::stop()
{
  if (not stopped) {
    refill_timer.stop();
    stopped = true;
  }
}
