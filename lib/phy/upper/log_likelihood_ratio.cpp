#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include <cmath>

using namespace srsgnb;

namespace {
// Helper structure to store results of special cases.
struct special_result {
  bool is_special = false;
  int  value      = 0;
};
} // namespace

// Computes the sum when at least one of the summands is plus/minus infinity.
// The indeterminate case +LLR_INFTY + (-LLR_INFTY) is set to zero.
static void tackle_special_sums(special_result& result, int val_a, int val_b)
{
  result.is_special = true;

  if (val_a == -val_b) {
    result.value = 0;
    return;
  }

  // When at least one of the summands is infinity, the sum is also infinity (with sign).
  // The indeterminate case LLR_INFTY + (-LLR_INFTY) has already been dealt with in the previous case.
  int abs_a = std::abs(val_a);
  int abs_b = std::abs(val_b);
  if (abs_a == log_likelihood_ratio::LLR_INFTY) {
    result.value = val_a;
    return;
  }
  if (abs_b == log_likelihood_ratio::LLR_INFTY) {
    result.value = val_b;
    return;
  }
  result.is_special = false;
}

log_likelihood_ratio log_likelihood_ratio::operator+(log_likelihood_ratio rhs) const
{
  special_result special;

  tackle_special_sums(special, this->value, rhs.value);
  if (special.is_special) {
    return special.value;
  }

  // When not dealing with special cases, classic saturated sum.
  int tmp = value + static_cast<int>(rhs.value);
  if (std::abs(tmp) > LLR_MAX) {
    return ((tmp > 0) ? LLR_MAX : -LLR_MAX);
  }
  return tmp;
}

log_likelihood_ratio log_likelihood_ratio::promotion_sum(log_likelihood_ratio a, log_likelihood_ratio b)
{
  special_result special;

  tackle_special_sums(special, a.value, b.value);
  if (special.is_special) {
    return special.value;
  }

  // When not dealing with special cases, promotion sum: if the sum exceeds LLR_MAX (in absolute value), then return
  // LLR_INFTY (with the proper sign).
  int tmp = a.value + static_cast<int>(b.value);
  if (std::abs(tmp) > log_likelihood_ratio::LLR_MAX) {
    return ((tmp > 0) ? log_likelihood_ratio::LLR_INFTY : -log_likelihood_ratio::LLR_INFTY);
  }
  return tmp;
}
