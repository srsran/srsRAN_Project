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

#ifdef HAVE_SSE
#include <immintrin.h>
#endif // HAVE_SSE

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

#ifdef HAVE_SSE
static void hard_decision_simd(bit_buffer& hard_bits, const int8_t* soft_bits, unsigned len)
{
  // Number of bits in a byte.
  static constexpr unsigned NBIT_BYTE = 8;

  unsigned i_bit = 0;

  for (; i_bit != (len / NBIT_BYTE) * NBIT_BYTE; i_bit += NBIT_BYTE) {
    // Generate a mask from the LLR.
    __m64 mask = _mm_cmpgt_pi8(_mm_set1_pi8(0), *reinterpret_cast<const __m64*>(&soft_bits[i_bit]));

    // Reverse mask.
    mask = _mm_shuffle_pi8(mask, _mm_set_pi8(0, 1, 2, 3, 4, 5, 6, 7));

    // Obtain 8 packed hard bits from the mask.
    uint8_t byte = _mm_movemask_pi8(mask);

    // Insert hard bits into the bit buffer.
    hard_bits.insert(byte, i_bit, NBIT_BYTE);
  }

  for (; i_bit != len; ++i_bit) {
    unsigned hard_bit = soft_bits[i_bit] < 0 ? 1 : 0;

    hard_bits.insert(hard_bit, i_bit, 1);
  }
}
#endif // HAVE_SSE

void srsgnb::hard_decision_packed(bit_buffer& hard_bits, span<const int8_t> soft_bits)
{
  srsgnb_assert(soft_bits.size() == hard_bits.size(),
                "Input size (i.e., {}) and output size (i.e., {}) must be equal",
                soft_bits.size(),
                hard_bits.size());

  unsigned nof_bits = hard_bits.size();

#ifdef HAVE_SSE

  hard_decision_simd(hard_bits, soft_bits.data(), nof_bits);

#else
  for (unsigned i_bit = 0; i_bit != nof_bits; ++i_bit) {
    // Compute hard bit.
    uint8_t hard_bit = soft_bits[i_bit] >= 0 ? 0 : 1;

    // Insert into the bit buffer.
    hard_bits.insert(hard_bit, i_bit, 1);
  }
#endif // HAVE_SSE
}

void srsgnb::hard_decision_packed(bit_buffer& hard_bits, span<const log_likelihood_ratio> soft_bits)
{
  span<const int8_t> soft_bits_span(reinterpret_cast<const int8_t*>(soft_bits.begin()), soft_bits.size());
  srsgnb::hard_decision_packed(hard_bits, soft_bits_span);
}