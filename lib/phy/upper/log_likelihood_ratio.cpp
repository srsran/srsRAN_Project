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

#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/srsvec/compare.h"
#include "srsran/srsvec/copy.h"
#include <cmath>
#include <optional>

#ifdef __AVX2__
#include <immintrin.h>
#endif // __AVX2__
#ifdef __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON

using namespace srsran;

/// Computes the sum when at least one of the summands is plus/minus infinity.
/// Note that also the indeterminate case +LLR_INFTY + (-LLR_INFTY) is set to zero.
static std::optional<log_likelihood_ratio> tackle_special_sums(log_likelihood_ratio val_a, log_likelihood_ratio val_b)
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

  return std::nullopt;
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
  srsran_assert(range_limit > 0, "Second input must be positive.");

  float clipped = value;
  if (std::abs(value) > range_limit) {
    clipped = std::copysign(range_limit, value);
  }
  return static_cast<value_type>(std::round(clipped / range_limit * LLR_MAX));
}

#ifdef __AVX2__
// Hard decision function with bit packing.
static void hard_decision_simd(bit_buffer& hard_bits, const int8_t* soft_bits, unsigned len, unsigned offset)
{
  // Number of bits processed on each loop cycle.
  static constexpr unsigned AVX2_B_SIZE = 32;

  unsigned i_bit = 0;

  // Destination buffer.
  span<uint8_t> packed_buffer = hard_bits.get_buffer();

  // Calculate number of unaligned bits.
  unsigned nof_unaligned_bits = std::min(8 - offset % 8, len);
  if (nof_unaligned_bits == 8) {
    nof_unaligned_bits = 0;
  }

  // Process unaligned bits.
  for (; i_bit != nof_unaligned_bits; ++i_bit) {
    uint8_t hard_bit = (soft_bits[i_bit] <= 0) ? 1 : 0;

    hard_bits.insert(hard_bit, i_bit + offset, 1);
  }

  // Advance packed buffer to the next aligned bit.
  packed_buffer = packed_buffer.last(packed_buffer.size() - divide_ceil(offset, 8));

  for (unsigned max_bit = nof_unaligned_bits + ((len - nof_unaligned_bits) / AVX2_B_SIZE) * AVX2_B_SIZE;
       i_bit != max_bit;
       i_bit += AVX2_B_SIZE) {
    // Load AVX2_B_SIZE LLRs.
    __m256i soft_epi8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(soft_bits + i_bit));

    // Shuffle soft_epi8: the soft bits are taken in groups of 8 and, inside each group, we reverse their order (this is
    // because, once we convert the soft bits into hard bits, the hard bits forming a byte need to be reversed before
    // being stored in packed format).
    // Remark: Recall that _mm256_set_epi8 takes as inputs (c31, c30, ..., c1, c0), which means that the control values
    // associated with the bytes of soft_epi8 (as the output of the shuffle operations) must be read down up.
    soft_epi8 = _mm256_shuffle_epi8(soft_epi8,
                                    _mm256_set_epi8(8,
                                                    9,
                                                    10,
                                                    11,
                                                    12,
                                                    13,
                                                    14,
                                                    15,
                                                    0,
                                                    1,
                                                    2,
                                                    3,
                                                    4,
                                                    5,
                                                    6,
                                                    7,
                                                    8,
                                                    9,
                                                    10,
                                                    11,
                                                    12,
                                                    13,
                                                    14,
                                                    15,
                                                    0,
                                                    1,
                                                    2,
                                                    3,
                                                    4,
                                                    5,
                                                    6,
                                                    7));

    // To obtain 32 packed hard bits from soft_epi8, we first compare with 1 - for each LLR, all the bits of the
    // corresponding output byte are set to 0 if the LLR is positive (that is, 1 > LLR is false), and are set to 1 if
    // the LLR is negative or null (that is, 1 > LLR is true). Finally, it suffices to pick the MSB of all bytes.
    soft_epi8      = _mm256_cmpgt_epi8(_mm256_set1_epi8(1), soft_epi8);
    uint32_t bytes = _mm256_movemask_epi8(soft_epi8);

    // Write the packed bits into 4 bytes of the internal buffer.
    std::memcpy(packed_buffer.begin(), &bytes, sizeof(uint32_t));

    // Advance buffer.
    packed_buffer = packed_buffer.last(packed_buffer.size() - (AVX2_B_SIZE / 8));
  }

  for (; i_bit != len; ++i_bit) {
    uint8_t hard_bit = (soft_bits[i_bit] <= 0) ? 1 : 0;

    hard_bits.insert(hard_bit, i_bit + offset, 1);
  }
}
#endif // __AVX2__

#ifdef __ARM_NEON
static void hard_decision_simd(bit_buffer& hard_bits, const int8_t* soft_bits, unsigned len, unsigned offset)
{
  const uint8x16_t mask_msb_u8    = vdupq_n_u8(0x80);
  const int8_t     shift_mask[16] = {-7, -6, -5, -4, -3, -2, -1, 0, -7, -6, -5, -4, -3, -2, -1, 0};
  const int8x16_t  shift_mask_s8 =
      vcombine_s8(vcreate_s8(*((uint64_t*)shift_mask)), vcreate_s8(*((uint64_t*)&shift_mask[8])));

  // Number of bits processed on each loop cycle.
  static constexpr unsigned NEON_B_SIZE = 16;

  unsigned i_bit = 0;

  // Destination buffer.
  span<uint8_t> packed_buffer = hard_bits.get_buffer();

  // Calculate number of unaligned bits.
  unsigned nof_unaligned_bits = std::min(8 - offset % 8, len);
  if (nof_unaligned_bits == 8) {
    nof_unaligned_bits = 0;
  }

  // Process unaligned bits.
  for (; i_bit != nof_unaligned_bits; ++i_bit) {
    uint8_t hard_bit = (soft_bits[i_bit] <= 0) ? 1 : 0;

    hard_bits.insert(hard_bit, i_bit + offset, 1);
  }

  // Advance packed buffer to the next aligned bit.
  packed_buffer = packed_buffer.last(packed_buffer.size() - divide_ceil(offset, 8));

  for (unsigned max_bit = nof_unaligned_bits + ((len - nof_unaligned_bits) / NEON_B_SIZE) * NEON_B_SIZE;
       i_bit != max_bit;
       i_bit += NEON_B_SIZE) {
    // Read soft bits.
    uint8x16_t soft_bits_u8 = vld1q_u8(reinterpret_cast<const uint8_t*>(&soft_bits[i_bit]));

    // Replace 0-valued soft bits with -1.
    uint8x16_t zero_mask = vceqq_u8(soft_bits_u8, vdupq_n_u8(0));
    soft_bits_u8         = vbslq_u8(zero_mask, vdupq_n_u8(255), soft_bits_u8);

    // Reverse 8 bytes in every double-word, MSBs of each byte will form a mask.
    soft_bits_u8 = vrev64q_u8(soft_bits_u8);

    // Generate masks of MSB bits shifted to appropriate position.
    uint8x16_t msb_bits_u8 = vandq_u8(soft_bits_u8, mask_msb_u8);
    uint8x16_t mask_u8     = vshlq_u8(msb_bits_u8, shift_mask_s8);

    // Obtain 16 packed hard bits from the mask.
    mask_u8 = vpaddq_u8(mask_u8, mask_u8);
    mask_u8 = vpaddq_u8(mask_u8, mask_u8);
    mask_u8 = vpaddq_u8(mask_u8, mask_u8);

    // Write the packed bits into 2 bytes of the internal buffer.
    vst1q_lane_u16(reinterpret_cast<uint16_t*>(packed_buffer.begin()), vreinterpretq_u16_u8(mask_u8), 0);

    // Advance buffer.
    packed_buffer = packed_buffer.last(packed_buffer.size() - (NEON_B_SIZE / 8));
  }

  for (; i_bit != len; ++i_bit) {
    uint8_t hard_bit = (soft_bits[i_bit] <= 0) ? 1 : 0;

    hard_bits.insert(hard_bit, i_bit + offset, 1);
  }
}
#endif // __ARM_NEON

void srsran::clamp(span<log_likelihood_ratio>       out,
                   span<const log_likelihood_ratio> in,
                   log_likelihood_ratio             low,
                   log_likelihood_ratio             high)
{
  srsran_assert(out.size() == in.size(),
                "Input size (i.e., {}) is not equal to the output size (i.e., {}).",
                in.size(),
                out.size());
  unsigned len = in.size();
  unsigned i   = 0;

#if defined(__AVX512F__) && defined(__AVX512BW__)
  const __m512i* in_ptr  = reinterpret_cast<const __m512i*>(in.data());
  __m512i*       out_ptr = reinterpret_cast<__m512i*>(out.data());

  __m512i low_epi8  = _mm512_set1_epi8(low.to_int());
  __m512i high_epi8 = _mm512_set1_epi8(high.to_int());

  // Clamping function.
  const auto clamp_func = [&low_epi8, &high_epi8](__m512i in_epi8) {
    __m512i temp = in_epi8;

    __mmask64 high_mask = _mm512_cmp_epi8_mask(temp, high_epi8, _MM_CMPINT_GT);
    temp                = _mm512_mask_blend_epi8(high_mask, temp, high_epi8);

    __mmask64 low_mask = _mm512_cmp_epi8_mask(low_epi8, temp, _MM_CMPINT_GT);
    temp               = _mm512_mask_blend_epi8(low_mask, temp, low_epi8);

    return temp;
  };

  // Clamps 64 LLR at a time.
  for (unsigned len_simd = (len / 64) * 64; i != len_simd; i += 64) {
    _mm512_storeu_si512(out_ptr++, clamp_func(_mm512_loadu_si512(in_ptr++)));
  }

  // Clamps the rest.
  __mmask64 remainder_mask = (1UL << (len % 64UL)) - 1UL;
  _mm512_mask_storeu_epi8(out_ptr, remainder_mask, clamp_func(_mm512_maskz_loadu_epi8(remainder_mask, in_ptr)));
#else // defined(__AVX512F__) && defined(__AVX512BW__)
#ifdef __AVX2__
  const __m256i* in_ptr  = reinterpret_cast<const __m256i*>(in.data());
  __m256i*       out_ptr = reinterpret_cast<__m256i*>(out.data());

  __m256i low_epi8  = _mm256_set1_epi8(low.to_int());
  __m256i high_epi8 = _mm256_set1_epi8(high.to_int());

  for (unsigned len_simd = (len / 32) * 32; i != len_simd; i += 32) {
    __m256i temp = _mm256_loadu_si256(in_ptr++);

    __m256i high_mask = _mm256_cmpgt_epi8(temp, high_epi8);
    temp              = _mm256_blendv_epi8(temp, high_epi8, high_mask);

    __m256i low_mask = _mm256_cmpgt_epi8(low_epi8, temp);
    temp             = _mm256_blendv_epi8(temp, low_epi8, low_mask);

    _mm256_storeu_si256(out_ptr++, temp);
  }
#endif // __AVX2__
#ifdef __ARM_NEON
  int8x16_t low_s8  = vdupq_n_s8(static_cast<int8_t>(low.to_int()));
  int8x16_t high_s8 = vdupq_n_s8(static_cast<int8_t>(high.to_int()));

  const int8_t* in_ptr  = reinterpret_cast<const int8_t*>(in.data());
  int8_t*       out_ptr = reinterpret_cast<int8_t*>(out.data());

  for (unsigned len_simd = (len / 16) * 16; i != len_simd; i += 16) {
    int8x16_t temp = vld1q_s8(in_ptr + i);

    uint8x16_t high_mask = vcgtq_s8(temp, high_s8);
    temp                 = vbslq_s8(high_mask, high_s8, temp);

    uint8x16_t low_mask = vcltq_s8(temp, low_s8);
    temp                = vbslq_s8(low_mask, low_s8, temp);

    vst1q_s8(out_ptr + i, temp);
  }
#endif // __ARM_NEON

  for (; i != len; ++i) {
    out[i] = std::clamp(in[i], low, high);
  }
#endif // defined(__AVX512F__) && defined(__AVX512BW__)
}

bool srsran::hard_decision(bit_buffer& hard_bits, span<const log_likelihood_ratio> soft_bits, unsigned offset)
{
  // Make sure that there is enough space in the output to accommodate the hard bits.
  srsran_assert(soft_bits.size() <= hard_bits.size(),
                "Input size (i.e., {}) does not fit into the output buffer with size {}",
                soft_bits.size(),
                hard_bits.size());

  unsigned nof_bits = soft_bits.size();

#if defined(__AVX2__) || defined(__ARM_NEON)

  hard_decision_simd(hard_bits, reinterpret_cast<const int8_t*>(soft_bits.data()), nof_bits, offset);

#else
  for (unsigned i_bit = 0; i_bit != nof_bits; ++i_bit) {
    // Compute hard bit.
    uint8_t hard_bit = soft_bits[i_bit].to_hard_bit();

    // Insert into the bit buffer.
    hard_bits.insert(hard_bit, offset + i_bit, 1);
  }
#endif // __AVX2__ or __ARM_NEON

  // Return false if it finds a zero in the soft bits.
  return srsvec::find(soft_bits, log_likelihood_ratio(0)) == soft_bits.end();
}

#ifdef __AVX2__
static int32_t dot_prod_sign_avx2(__m256i x_epi8, __m256i y_epi8)
{
  // Negate x according to the sign of y.
  x_epi8 = _mm256_sign_epi8(x_epi8, y_epi8);

  // Convert to 16-bit integers.
  __m256i x_low_epi16  = _mm256_cvtepi8_epi16(_mm256_castsi256_si128(x_epi8));
  __m256i x_high_epi16 = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(x_epi8, 1));

  // Sum 16-bit integer registers.
  __m256i sum_epi16 = _mm256_add_epi16(x_low_epi16, x_high_epi16);

  // Split in two 128-bit registers.
  __m128i sum_low_epi16  = _mm256_castsi256_si128(sum_epi16);
  __m128i sum_high_epi16 = _mm256_extracti128_si256(sum_epi16, 1);

  // Reduce 128-bit registers.
  __m128i reduced_sum_epi16 = _mm_add_epi16(sum_low_epi16, sum_high_epi16);

  // Keep reducing values.
  reduced_sum_epi16 = _mm_hadd_epi16(reduced_sum_epi16, reduced_sum_epi16);
  reduced_sum_epi16 = _mm_hadd_epi16(reduced_sum_epi16, reduced_sum_epi16);
  reduced_sum_epi16 = _mm_hadd_epi16(reduced_sum_epi16, reduced_sum_epi16);

  // Extract the lowest 16-bit integer, convert to 32-bits and return.
  int16_t result = _mm_extract_epi16(reduced_sum_epi16, 0);
  return static_cast<int32_t>(result);
}

#endif // __AVX2__

int32_t log_likelihood_ratio::dot_prod_sign(span<const srsran::log_likelihood_ratio> x, span<const int8_t> y)
{
  srsran_assert(x.size() == y.size(), "Sizes must be equal.");

#ifdef __AVX2__
  // Optimized calculation in AVX2 for 32 values.
  if (x.size() == 32) {
    __m256i x_epi8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(x.data()));
    __m256i y_epi8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(y.data()));
    return dot_prod_sign_avx2(x_epi8, y_epi8);
  }
#endif // __AVX2__

  int32_t ret =
      std::inner_product(x.begin(), x.end(), y.begin(), 0, std::plus<int32_t>(), [](log_likelihood_ratio a, int8_t b) {
        return a.to_int() * std::copysign(1, b);
      });

  return ret;
}

#ifdef __AVX2__
static inline __m256i avx2_sum_llr(__m256i in0_epi8, __m256i in1_epi8)
{
  __m256i LLR_MAX_epi8 = _mm256_set1_epi8(static_cast<int8_t>(LLR_MAX));
  __m256i LLR_MIN_epi8 = _mm256_set1_epi8(static_cast<int8_t>(LLR_MIN));

  __m256i result = _mm256_adds_epi8(in0_epi8, in1_epi8);

  __m256i mask_epi8 = _mm256_cmpgt_epi8(result, LLR_MAX_epi8);
  result            = _mm256_blendv_epi8(result, LLR_MAX_epi8, mask_epi8);

  mask_epi8 = _mm256_cmpgt_epi8(LLR_MIN_epi8, result);
  return _mm256_blendv_epi8(result, LLR_MIN_epi8, mask_epi8);
}
#endif // __AVX2__

#ifdef __ARM_NEON
static inline int8x16_t neon_sum_llr(int8x16_t in0_s8, int8x16_t in1_s8)
{
  const int8x16_t LLR_MAX_s8 = vdupq_n_s8(static_cast<int8_t>(LLR_MAX));
  const int8x16_t LLR_MIN_s8 = vdupq_n_s8(static_cast<int8_t>(LLR_MIN));

  int8x16_t result = vqaddq_s8(in0_s8, in1_s8);

  uint8x16_t mask_s8 = vcgtq_s8(result, LLR_MAX_s8);
  result             = vbslq_s8(mask_s8, LLR_MAX_s8, result);
  mask_s8            = vcgtq_s8(LLR_MIN_s8, result);
  result             = vbslq_s8(mask_s8, LLR_MIN_s8, result);

  return result;
}
#endif // __ARM_NEON

void log_likelihood_ratio::sum(span<log_likelihood_ratio>       out,
                               span<const log_likelihood_ratio> in0,
                               span<const log_likelihood_ratio> in1)
{
  srsran_assert(out.size() == in0.size(), "All sizes must be equal.");
  srsran_assert(out.size() == in1.size(), "All sizes must be equal.");

#if defined(__AVX2__)
  unsigned index = 0;

  static constexpr unsigned AVX2_SIZE_BYTE = 32;
  __m256i*                  avx_out        = reinterpret_cast<__m256i*>(out.data());
  const __m256i*            avx_in0        = reinterpret_cast<const __m256i*>(in0.data());
  const __m256i*            avx_in1        = reinterpret_cast<const __m256i*>(in1.data());

  for (unsigned index_end = (out.size() / AVX2_SIZE_BYTE); index != index_end; ++index) {
    __m256i in0_epi8 = _mm256_loadu_si256(avx_in0++);
    __m256i in1_epi8 = _mm256_loadu_si256(avx_in1++);
    _mm256_storeu_si256(avx_out++, avx2_sum_llr(in0_epi8, in1_epi8));
  }
  index *= AVX2_SIZE_BYTE;

  // Calculate the remainder of bits.
  unsigned remainder = out.size() - index;

  // Copy the end of the input bits in vectors of AVX register size.
  std::array<log_likelihood_ratio, AVX2_SIZE_BYTE> in0_remainder;
  std::array<log_likelihood_ratio, AVX2_SIZE_BYTE> in1_remainder;
  srsvec::copy(span<log_likelihood_ratio>(in0_remainder).first(remainder), in0.last(remainder));
  srsvec::copy(span<log_likelihood_ratio>(in1_remainder).first(remainder), in1.last(remainder));

  // Load the remainder bits in AVX registers.
  __m256i in0_epi8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in0_remainder.data()));
  __m256i in1_epi8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in1_remainder.data()));

  // Process last AVX block and store it in a vector of AVX register size.
  std::array<log_likelihood_ratio, AVX2_SIZE_BYTE> out_remainder;
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(out_remainder.data()), avx2_sum_llr(in0_epi8, in1_epi8));

  // Copy the last bits.
  srsvec::copy(out.last(remainder), span<log_likelihood_ratio>(out_remainder).first(remainder));
#elif defined(__ARM_NEON)
  unsigned                  index          = 0;
  unsigned                  out_size       = out.size();
  static constexpr unsigned NEON_SIZE_BYTE = 16;

  // Process batches of 32 values.
  for (unsigned index_end = (out_size / 32) * 32; index != index_end; index += 32) {
    int8x16x2_t in0_s8 = vld1q_s8_x2(reinterpret_cast<const int8_t*>(&in0[index]));
    int8x16x2_t in1_s8 = vld1q_s8_x2(reinterpret_cast<const int8_t*>(&in1[index]));

    int8x16x2_t result;
    result.val[0] = neon_sum_llr(in0_s8.val[0], in1_s8.val[0]);
    result.val[1] = neon_sum_llr(in0_s8.val[1], in1_s8.val[1]);

    vst1q_s8_x2(reinterpret_cast<int8_t*>(&out[index]), result);
  }

  // Process batch of 16 values.
  if (out_size & 0x10) {
    int8x16_t in0_s8 = vld1q_s8(reinterpret_cast<const int8_t*>(&in0[index]));
    int8x16_t in1_s8 = vld1q_s8(reinterpret_cast<const int8_t*>(&in1[index]));
    int8x16_t result = neon_sum_llr(in0_s8, in1_s8);

    vst1q_s8(reinterpret_cast<int8_t*>(&out[index]), result);
    index += 16;
  }

  // Calculate the remainder of bits.
  unsigned remainder = out.size() - index;

  // Copy the end of the input bits in vectors of AVX register size.
  std::array<log_likelihood_ratio, NEON_SIZE_BYTE> in0_remainder;
  std::array<log_likelihood_ratio, NEON_SIZE_BYTE> in1_remainder;
  srsvec::copy(span<log_likelihood_ratio>(in0_remainder).first(remainder), in0.last(remainder));
  srsvec::copy(span<log_likelihood_ratio>(in1_remainder).first(remainder), in1.last(remainder));

  // Load the remainder bits in NEON registers.
  int8x16_t in0_s8 = vld1q_s8(reinterpret_cast<const int8_t*>(in0_remainder.data()));
  int8x16_t in1_s8 = vld1q_s8(reinterpret_cast<const int8_t*>(in1_remainder.data()));

  // Process last NEON block and store it in a vector of NEON register size.
  std::array<log_likelihood_ratio, NEON_SIZE_BYTE> out_remainder;
  vst1q_s8(reinterpret_cast<int8_t*>(out_remainder.data()), neon_sum_llr(in0_s8, in1_s8));

  // Copy the last bits.
  srsvec::copy(out.last(remainder), span<log_likelihood_ratio>(out_remainder).first(remainder));
#else
  std::transform(
      in0.begin(), in0.end(), in1.begin(), out.begin(), [](log_likelihood_ratio left, log_likelihood_ratio right) {
        return left + right;
      });
#endif
}
