/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/rate_limiting/token_bucket_factory.h"
#include "token_bucket.h"

using namespace srsran;

std::unique_ptr<rate_limiter> srsran::create_token_bucket(const token_bucket_config& cfg)
{
  std::unique_ptr<rate_limiter> rlim = std::make_unique<token_bucket>(cfg);
  return rlim;
}
