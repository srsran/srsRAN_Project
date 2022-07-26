/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/adt/optional.h"
#include <cmath>

using namespace srsgnb;

// Computes the sum when at least one of the summands is plus/minus infinity.
// Note that also the indeterminate case +LLR_INFTY + (-LLR_INFTY) is set to zero.
static optional<log_likelihood_ratio> tackle_special_sums(log_likelihood_ratio val_a, log_likelihood_ratio val_b)
{
  if (val_a == -val_b) {
    return log_likelihood_ratio(0);
  }

  // When at least one of the summands is infinity, the sum is also infinity (with sign).
  // The indeterminate case LLR_INFTY + (-LLR_INFTY) has already been dealt with in the previous case.
  if (log_likelihood_ratio::isinf(val_a)) {
    return val_a;
  }
  if (log_likelihood_ratio::isinf(val_b)) {
    return val_b;
  }
  return {};
}

log_likelihood_ratio log_likelihood_ratio::operator+=(log_likelihood_ratio rhs)
{
  if (auto special = tackle_special_sums(*this, rhs)) {
    *this = special.value();
    return *this;
  }

  // When not dealing with special cases, classic saturated sum.
  int tmp = value + static_cast<int>(rhs.value);
  if (std::abs(tmp) > LLR_MAX) {
    *this = ((tmp > 0) ? LLR_MAX : -LLR_MAX);
    return *this;
  }
  *this = tmp;
  return *this;
}

log_likelihood_ratio log_likelihood_ratio::promotion_sum(log_likelihood_ratio a, log_likelihood_ratio b)
{
  if (auto special = tackle_special_sums(a, b)) {
    return special.value();
  }

  // When not dealing with special cases, promotion sum: if the sum exceeds LLR_MAX (in absolute value), then return
  // LLR_INFTY (with the proper sign).
  int tmp = a.value + static_cast<int>(b.value);
  if (std::abs(tmp) > log_likelihood_ratio::LLR_MAX) {
    return ((tmp > 0) ? log_likelihood_ratio::LLR_INFTY : -log_likelihood_ratio::LLR_INFTY);
  }
  return tmp;
}

log_likelihood_ratio log_likelihood_ratio::quantize(float value, float range_limit)
{
  srsgnb_assert(range_limit > 0, "Second input must be positive.");

  float clipped = value;
  if (std::abs(value) > range_limit) {
    clipped = std::copysign(range_limit, value);
  }
  return static_cast<value_type>(std::round(clipped / range_limit * LLR_MAX));
}
