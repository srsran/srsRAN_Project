/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ldpc_rate_dematcher_neon_impl.h"
#include "neon_support.h"
#include <arm_neon.h>

using namespace srsran;

void ldpc_rate_dematcher_neon_impl::combine_softbits(span<log_likelihood_ratio>       out,
                                                     span<const log_likelihood_ratio> in0,
                                                     span<const log_likelihood_ratio> in1) const
{
  srsran_assert(out.size() == in0.size(), "All sizes must be equal.");
  srsran_assert(out.size() == in1.size(), "All sizes must be equal.");

  unsigned index               = 0;
  unsigned out_size            = out.size();
  bool     process_16_val_tail = out_size & 0x10;
  bool     process_8_val_tail  = out_size & 0x8;

  const int8x16_t LLR_MAX_s8 = vdupq_n_s8(static_cast<int8_t>(LLR_MAX));
  const int8x16_t LLR_MIN_s8 = vdupq_n_s8(static_cast<int8_t>(LLR_MIN));

  // Process batches of 32 values.
  for (unsigned index_end = (out_size / 32) * 32; index != index_end; index += 32) {
    int8x16x2_t in0_s8 = vld1q_s8_x2(reinterpret_cast<const int8_t*>(&in0[index]));
    int8x16x2_t in1_s8 = vld1q_s8_x2(reinterpret_cast<const int8_t*>(&in1[index]));

    int8x16x2_t result;
    // Process first 16 LLRs.
    result.val[0]      = vqaddq_s8(in0_s8.val[0], in1_s8.val[0]);
    uint8x16_t mask_s8 = vcgtq_s8(result.val[0], LLR_MAX_s8);
    result.val[0]      = vbslq_s8(mask_s8, LLR_MAX_s8, result.val[0]);
    mask_s8            = vcgtq_s8(LLR_MIN_s8, result.val[0]);
    result.val[0]      = vbslq_s8(mask_s8, LLR_MIN_s8, result.val[0]);

    // Process next 16 LLRs.
    result.val[1] = vqaddq_s8(in0_s8.val[1], in1_s8.val[1]);
    mask_s8       = vcgtq_s8(result.val[1], LLR_MAX_s8);
    result.val[1] = vbslq_s8(mask_s8, LLR_MAX_s8, result.val[1]);
    mask_s8       = vcgtq_s8(LLR_MIN_s8, result.val[1]);
    result.val[1] = vbslq_s8(mask_s8, LLR_MIN_s8, result.val[1]);

    vst1q_s8_x2(reinterpret_cast<int8_t*>(&out[index]), result);
  }
  // Process batch of 16 values.
  if (process_16_val_tail) {
    int8x16_t in0_s8 = vld1q_s8(reinterpret_cast<const int8_t*>(&in0[index]));
    int8x16_t in1_s8 = vld1q_s8(reinterpret_cast<const int8_t*>(&in1[index]));
    int8x16_t result = vqaddq_s8(in0_s8, in1_s8);

    uint8x16_t mask_s8 = vcgtq_s8(result, LLR_MAX_s8);
    result             = vbslq_s8(mask_s8, LLR_MAX_s8, result);
    mask_s8            = vcgtq_s8(LLR_MIN_s8, result);
    result             = vbslq_s8(mask_s8, LLR_MIN_s8, result);

    vst1q_s8(reinterpret_cast<int8_t*>(&out[index]), result);
    index += 16;
  }
  // Process batch of remaining 8 values.
  if (process_8_val_tail) {
    int8x8_t in0_s8 = vld1_s8(reinterpret_cast<const int8_t*>(&in0[index]));
    int8x8_t in1_s8 = vld1_s8(reinterpret_cast<const int8_t*>(&in1[index]));
    int8x8_t result = vqadd_s8(in0_s8, in1_s8);

    uint8x8_t mask_s8 = vcgt_s8(result, vget_low_s8(LLR_MAX_s8));
    result            = vbsl_s8(mask_s8, vget_low_s8(LLR_MAX_s8), result);
    mask_s8           = vcgt_s8(vget_low_s8(LLR_MIN_s8), result);
    result            = vbsl_s8(mask_s8, vget_low_s8(LLR_MIN_s8), result);

    vst1_s8(reinterpret_cast<int8_t*>(&out[index]), result);
    index += 8;
  }

  // Process tail LLRs not multiple of 8.
  for (; index != out_size; ++index) {
    out[index] = in0[index] + in1[index];
  }
}

static void neon_deinterleave(int8x16_t& out_even, int8x16_t& out_odd, int8x16_t in_lo, int8x16_t in_hi)
{
  out_even = vuzp1q_s8(in_lo, in_hi);
  out_odd  = vuzp2q_s8(in_lo, in_hi);
}

static void
neon_deinterleave(int8x16_t& out0, int8x16_t& out1, int8x16_t& out2, int8x16_t in0, int8x16_t in1, int8x16_t in2)
{
  out0 = vsetq_lane_s8(vgetq_lane_s8(in0, 0), out0, 0);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in0, 3), out0, 1);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in0, 6), out0, 2);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in0, 9), out0, 3);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in0, 12), out0, 4);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in0, 15), out0, 5);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in1, 2), out0, 6);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in1, 5), out0, 7);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in1, 8), out0, 8);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in1, 11), out0, 9);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in1, 14), out0, 10);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in2, 1), out0, 11);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in2, 4), out0, 12);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in2, 7), out0, 13);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in2, 10), out0, 14);
  out0 = vsetq_lane_s8(vgetq_lane_s8(in2, 13), out0, 15);

  out1 = vsetq_lane_s8(vgetq_lane_s8(in0, 1), out1, 0);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in0, 4), out1, 1);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in0, 7), out1, 2);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in0, 10), out1, 3);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in0, 13), out1, 4);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in1, 0), out1, 5);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in1, 3), out1, 6);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in1, 6), out1, 7);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in1, 9), out1, 8);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in1, 12), out1, 9);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in1, 15), out1, 10);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in2, 2), out1, 11);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in2, 5), out1, 12);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in2, 8), out1, 13);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in2, 11), out1, 14);
  out1 = vsetq_lane_s8(vgetq_lane_s8(in2, 14), out1, 15);

  out2 = vsetq_lane_s8(vgetq_lane_s8(in0, 2), out2, 0);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in0, 5), out2, 1);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in0, 8), out2, 2);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in0, 11), out2, 3);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in0, 14), out2, 4);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in1, 1), out2, 5);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in1, 4), out2, 6);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in1, 7), out2, 7);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in1, 10), out2, 8);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in1, 13), out2, 9);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in2, 0), out2, 10);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in2, 3), out2, 11);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in2, 6), out2, 12);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in2, 9), out2, 13);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in2, 12), out2, 14);
  out2 = vsetq_lane_s8(vgetq_lane_s8(in2, 15), out2, 15);
}

void ldpc_rate_dematcher_neon_impl::deinterleave_qpsk(span<log_likelihood_ratio>       out,
                                                      span<const log_likelihood_ratio> in) const
{
  unsigned E = out.size();
  unsigned K = E / 2;
  unsigned i = 0;

  // Reinterpret pointers.
  neon::neon_const_span neon_in(in, E / NEON_SIZE_BYTE);
  neon::neon_span       neon_out0(out.subspan(0UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out1(out.subspan(1UL * K, K), K / NEON_SIZE_BYTE);

  // Do SIMD processing.
  for (unsigned i_end = (K / NEON_SIZE_BYTE), i_in = 0; i != i_end; ++i) {
    int8x16_t in0 = neon_in.get_at(i_in++);
    int8x16_t in1 = neon_in.get_at(i_in++);

    int8x16_t out0;
    int8x16_t out1;
    neon_deinterleave(out0, out1, in0, in1);

    neon_out0.set_at(i, out0);
    neon_out1.set_at(i, out1);
  }
  i *= NEON_SIZE_BYTE;

  // Remainder of soft bits.
  for (; i != K; ++i) {
    out[K * 0U + i] = in[2U * i + 0];
    out[K * 1U + i] = in[2U * i + 1];
  }
}

void ldpc_rate_dematcher_neon_impl::deinterleave_qam16(span<log_likelihood_ratio>       out,
                                                       span<const log_likelihood_ratio> in) const
{
  unsigned E = out.size();
  unsigned K = E / 4;
  unsigned i = 0;

  // Reinterpret pointers.
  neon::neon_const_span neon_in(in, E / NEON_SIZE_BYTE);
  neon::neon_span       neon_out0(out.subspan(0UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out1(out.subspan(1UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out2(out.subspan(2UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out3(out.subspan(3UL * K, K), K / NEON_SIZE_BYTE);

  // Do SIMD processing.
  for (unsigned i_end = (K / NEON_SIZE_BYTE), i_in = 0; i != i_end; ++i) {
    int8x16_t in0 = neon_in.get_at(i_in++);
    int8x16_t in1 = neon_in.get_at(i_in++);
    int8x16_t in2 = neon_in.get_at(i_in++);
    int8x16_t in3 = neon_in.get_at(i_in++);

    int8x16_t deinterleaved_01_even;
    int8x16_t deinterleaved_01_odd;
    neon_deinterleave(deinterleaved_01_even, deinterleaved_01_odd, in0, in1);

    int8x16_t deinterleaved_23_even;
    int8x16_t deinterleaved_23_odd;
    neon_deinterleave(deinterleaved_23_even, deinterleaved_23_odd, in2, in3);

    int8x16_t out0;
    int8x16_t out2;
    neon_deinterleave(out0, out2, deinterleaved_01_even, deinterleaved_23_even);

    int8x16_t out1;
    int8x16_t out3;
    neon_deinterleave(out1, out3, deinterleaved_01_odd, deinterleaved_23_odd);

    neon_out0.set_at(i, out0);
    neon_out1.set_at(i, out1);
    neon_out2.set_at(i, out2);
    neon_out3.set_at(i, out3);
  }
  i *= NEON_SIZE_BYTE;

  // Remainder of soft bits.
  for (; i != K; ++i) {
    out[K * 0U + i] = in[4U * i + 0];
    out[K * 1U + i] = in[4U * i + 1];
    out[K * 2U + i] = in[4U * i + 2];
    out[K * 3U + i] = in[4U * i + 3];
  }
}

void ldpc_rate_dematcher_neon_impl::deinterleave_qam64(span<log_likelihood_ratio>       out,
                                                       span<const log_likelihood_ratio> in) const
{
  unsigned E = out.size();
  unsigned K = E / 6;
  unsigned i = 0;

  // Reinterpret pointers.
  neon::neon_const_span neon_in(in, E / NEON_SIZE_BYTE);
  neon::neon_span       neon_out0(out.subspan(0UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out1(out.subspan(1UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out2(out.subspan(2UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out3(out.subspan(3UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out4(out.subspan(4UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out5(out.subspan(5UL * K, K), K / NEON_SIZE_BYTE);

  // Do SIMD processing.
  for (unsigned i_end = (K / NEON_SIZE_BYTE), i_in = 0; i != i_end; ++i) {
    int8x16_t in0 = neon_in.get_at(i_in++);
    int8x16_t in1 = neon_in.get_at(i_in++);
    int8x16_t in2 = neon_in.get_at(i_in++);
    int8x16_t in3 = neon_in.get_at(i_in++);
    int8x16_t in4 = neon_in.get_at(i_in++);
    int8x16_t in5 = neon_in.get_at(i_in++);

    int8x16_t deinterleaved_0;
    int8x16_t deinterleaved_1;
    int8x16_t deinterleaved_2;
    neon_deinterleave(deinterleaved_0, deinterleaved_1, deinterleaved_2, in0, in1, in2);

    int8x16_t deinterleaved_3;
    int8x16_t deinterleaved_4;
    int8x16_t deinterleaved_5;
    neon_deinterleave(deinterleaved_3, deinterleaved_4, deinterleaved_5, in3, in4, in5);

    int8x16_t out0;
    int8x16_t out3;
    neon_deinterleave(out0, out3, deinterleaved_0, deinterleaved_3);

    int8x16_t out1;
    int8x16_t out4;
    neon_deinterleave(out1, out4, deinterleaved_1, deinterleaved_4);

    int8x16_t out2;
    int8x16_t out5;
    neon_deinterleave(out2, out5, deinterleaved_2, deinterleaved_5);

    neon_out0.set_at(i, out0);
    neon_out1.set_at(i, out1);
    neon_out2.set_at(i, out2);
    neon_out3.set_at(i, out3);
    neon_out4.set_at(i, out4);
    neon_out5.set_at(i, out5);
  }
  i *= NEON_SIZE_BYTE;

  // Remainder of soft bits.
  for (; i != K; ++i) {
    out[K * 0U + i] = in[6U * i + 0];
    out[K * 1U + i] = in[6U * i + 1];
    out[K * 2U + i] = in[6U * i + 2];
    out[K * 3U + i] = in[6U * i + 3];
    out[K * 4U + i] = in[6U * i + 4];
    out[K * 5U + i] = in[6U * i + 5];
  }
}
void ldpc_rate_dematcher_neon_impl::deinterleave_qam256(span<log_likelihood_ratio>       out,
                                                        span<const log_likelihood_ratio> in) const
{
  unsigned E = out.size();
  unsigned K = E / 8;
  unsigned i = 0;

  // Reinterpret pointers.
  neon::neon_const_span neon_in(in, E / NEON_SIZE_BYTE);
  neon::neon_span       neon_out0(out.subspan(0UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out1(out.subspan(1UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out2(out.subspan(2UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out3(out.subspan(3UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out4(out.subspan(4UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out5(out.subspan(5UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out6(out.subspan(6UL * K, K), K / NEON_SIZE_BYTE);
  neon::neon_span       neon_out7(out.subspan(7UL * K, K), K / NEON_SIZE_BYTE);

  // Do SIMD processing.
  for (unsigned i_end = (K / NEON_SIZE_BYTE), i_in = 0; i != i_end; ++i) {
    int8x16_t in0 = neon_in.get_at(i_in++);
    int8x16_t in1 = neon_in.get_at(i_in++);
    int8x16_t in2 = neon_in.get_at(i_in++);
    int8x16_t in3 = neon_in.get_at(i_in++);
    int8x16_t in4 = neon_in.get_at(i_in++);
    int8x16_t in5 = neon_in.get_at(i_in++);
    int8x16_t in6 = neon_in.get_at(i_in++);
    int8x16_t in7 = neon_in.get_at(i_in++);

    int8x16_t deinterleaved_01_even;
    int8x16_t deinterleaved_01_odd;
    neon_deinterleave(deinterleaved_01_even, deinterleaved_01_odd, in0, in1);

    int8x16_t deinterleaved_23_even;
    int8x16_t deinterleaved_23_odd;
    neon_deinterleave(deinterleaved_23_even, deinterleaved_23_odd, in2, in3);

    int8x16_t deinterleaved_45_even;
    int8x16_t deinterleaved_45_odd;
    neon_deinterleave(deinterleaved_45_even, deinterleaved_45_odd, in4, in5);

    int8x16_t deinterleaved_67_even;
    int8x16_t deinterleaved_67_odd;
    neon_deinterleave(deinterleaved_67_even, deinterleaved_67_odd, in6, in7);

    int8x16_t deinterleaved_0123_even_even;
    int8x16_t deinterleaved_0123_even_odd;
    neon_deinterleave(
        deinterleaved_0123_even_even, deinterleaved_0123_even_odd, deinterleaved_01_even, deinterleaved_23_even);

    int8x16_t deinterleaved_4567_even_even;
    int8x16_t deinterleaved_4567_even_odd;
    neon_deinterleave(
        deinterleaved_4567_even_even, deinterleaved_4567_even_odd, deinterleaved_45_even, deinterleaved_67_even);

    int8x16_t deinterleaved_0123_odd_even;
    int8x16_t deinterleaved_0123_odd_odd;
    neon_deinterleave(
        deinterleaved_0123_odd_even, deinterleaved_0123_odd_odd, deinterleaved_01_odd, deinterleaved_23_odd);

    int8x16_t deinterleaved_4567_odd_even;
    int8x16_t deinterleaved_4567_odd_odd;
    neon_deinterleave(
        deinterleaved_4567_odd_even, deinterleaved_4567_odd_odd, deinterleaved_45_odd, deinterleaved_67_odd);

    int8x16_t out0;
    int8x16_t out4;
    neon_deinterleave(out0, out4, deinterleaved_0123_even_even, deinterleaved_4567_even_even);

    int8x16_t out1;
    int8x16_t out5;
    neon_deinterleave(out1, out5, deinterleaved_0123_odd_even, deinterleaved_4567_odd_even);

    int8x16_t out2;
    int8x16_t out6;
    neon_deinterleave(out2, out6, deinterleaved_0123_even_odd, deinterleaved_4567_even_odd);

    int8x16_t out3;
    int8x16_t out7;
    neon_deinterleave(out3, out7, deinterleaved_0123_odd_odd, deinterleaved_4567_odd_odd);

    neon_out0.set_at(i, out0);
    neon_out1.set_at(i, out1);
    neon_out2.set_at(i, out2);
    neon_out3.set_at(i, out3);
    neon_out4.set_at(i, out4);
    neon_out5.set_at(i, out5);
    neon_out6.set_at(i, out6);
    neon_out7.set_at(i, out7);
  }
  i *= NEON_SIZE_BYTE;

  // Remainder of soft bits.
  for (; i != K; ++i) {
    out[K * 0U + i] = in[8U * i + 0];
    out[K * 1U + i] = in[8U * i + 1];
    out[K * 2U + i] = in[8U * i + 2];
    out[K * 3U + i] = in[8U * i + 3];
    out[K * 4U + i] = in[8U * i + 4];
    out[K * 5U + i] = in[8U * i + 5];
    out[K * 6U + i] = in[8U * i + 6];
    out[K * 7U + i] = in[8U * i + 7];
  }
}