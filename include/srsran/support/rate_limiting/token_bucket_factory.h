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
#include <memory>

namespace srsran {

/// Creates an instance of a token bucket.
std::unique_ptr<rate_limiter> create_token_bucket(const token_bucket_config& cfg);

} // namespace srsran
