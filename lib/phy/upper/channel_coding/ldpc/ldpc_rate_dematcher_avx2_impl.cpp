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

#include "ldpc_rate_dematcher_avx2_impl.h"
#include "avx2_support.h"
#include <immintrin.h>

using namespace srsran;

void ldpc_rate_dematcher_avx2_impl::combine_softbits(span<srsran::log_likelihood_ratio>       out,
                                                     span<const srsran::log_likelihood_ratio> in0,
                                                     span<const srsran::log_likelihood_ratio> in1) const
{
  srsran_assert(out.size() == in0.size(), "All sizes must be equal.");
  srsran_assert(out.size() == in1.size(), "All sizes must be equal.");

  unsigned index = 0;

  mm256::avx2_span       avx_out(out, out.size() / AVX2_SIZE_BYTE);
  mm256::avx2_const_span avx_in0(in0, in0.size() / AVX2_SIZE_BYTE);
  mm256::avx2_const_span avx_in1(in1, in1.size() / AVX2_SIZE_BYTE);

  __m256i LLR_MAX_epi8 = _mm256_set1_epi8(static_cast<int8_t>(LLR_MAX));
  __m256i LLR_MIN_epi8 = _mm256_set1_epi8(static_cast<int8_t>(LLR_MIN));

  for (unsigned index_end = (out.size() / AVX2_SIZE_BYTE); index != index_end; ++index) {
    __m256i in0_epi8 = avx_in0.get_at(index);
    __m256i in1_epi8 = avx_in1.get_at(index);

    __m256i result = _mm256_adds_epi8(in0_epi8, in1_epi8);

    __m256i mask_epi8 = _mm256_cmpgt_epi8(result, LLR_MAX_epi8);
    result            = _mm256_blendv_epi8(result, LLR_MAX_epi8, mask_epi8);

    mask_epi8 = _mm256_cmpgt_epi8(LLR_MIN_epi8, result);
    result    = _mm256_blendv_epi8(result, LLR_MIN_epi8, mask_epi8);

    avx_out.set_at(index, result);
  }
  index *= AVX2_SIZE_BYTE;

  for (unsigned index_end = out.size(); index != index_end; ++index) {
    out[index] = in0[index] + in1[index];
  }
}

static void mm256_deinterleave(__m256i& out_even, __m256i& out_odd, __m256i in_lo, __m256i in_hi)
{
  // Convert AVX registers to a linear input.
  alignas(AVX2_SIZE_BYTE) std::array<int8_t, 2 * AVX2_SIZE_BYTE> in = {};
  _mm256_store_si256(reinterpret_cast<__m256i*>(in.data() + 0), in_lo);
  _mm256_store_si256(reinterpret_cast<__m256i*>(in.data() + AVX2_SIZE_BYTE), in_hi);

  // Actual deinterleaver algorithm. Let the compiler optimize.
  alignas(AVX2_SIZE_BYTE) std::array<int8_t, 2 * AVX2_SIZE_BYTE> out = {};
  for (unsigned i = 0; i != AVX2_SIZE_BYTE; ++i) {
    for (unsigned j = 0; j != 2; ++j) {
      out[AVX2_SIZE_BYTE * j + i] = in[i * 2 + j];
    }
  }

  // Convert linear output to AVX registers.
  out_even = _mm256_load_si256(reinterpret_cast<const __m256i*>(out.data() + 0));
  out_odd  = _mm256_load_si256(reinterpret_cast<const __m256i*>(out.data() + AVX2_SIZE_BYTE));
}

static void mm256_deinterleave(__m256i& out0, __m256i& out1, __m256i& out2, __m256i in0, __m256i in1, __m256i in2)
{
  // Convert AVX registers to a linear input.
  alignas(AVX2_SIZE_BYTE) std::array<int8_t, 3 * AVX2_SIZE_BYTE> in = {};
  _mm256_store_si256(reinterpret_cast<__m256i*>(in.data() + 0), in0);
  _mm256_store_si256(reinterpret_cast<__m256i*>(in.data() + 1 * AVX2_SIZE_BYTE), in1);
  _mm256_store_si256(reinterpret_cast<__m256i*>(in.data() + 2 * AVX2_SIZE_BYTE), in2);

  // Actual deinterleaver algorithm. Let the compiler optimize.
  alignas(AVX2_SIZE_BYTE) std::array<int8_t, 3 * AVX2_SIZE_BYTE> out = {};
  for (unsigned i = 0; i != AVX2_SIZE_BYTE; ++i) {
    for (unsigned j = 0; j != 3; ++j) {
      out[AVX2_SIZE_BYTE * j + i] = in[i * 3 + j];
    }
  }

  // Convert linear output to AVX registers.
  out0 = _mm256_load_si256(reinterpret_cast<const __m256i*>(out.data() + 0));
  out1 = _mm256_load_si256(reinterpret_cast<const __m256i*>(out.data() + 1 * AVX2_SIZE_BYTE));
  out2 = _mm256_load_si256(reinterpret_cast<const __m256i*>(out.data() + 2 * AVX2_SIZE_BYTE));
}

void ldpc_rate_dematcher_avx2_impl::deinterleave_qpsk(span<log_likelihood_ratio>       out,
                                                      span<const log_likelihood_ratio> in) const
{
  unsigned E = out.size();
  unsigned K = E / 2;
  unsigned i = 0;

  // Reinterpret pointers.
  mm256::avx2_const_span avx2_in(in, E / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out0(out.subspan(0UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out1(out.subspan(1UL * K, K), K / AVX2_SIZE_BYTE);

  // Do SIMD processing.
  for (unsigned i_end = (K / AVX2_SIZE_BYTE), i_in = 0; i != i_end; ++i) {
    __m256i in0 = avx2_in.get_at(i_in++);
    __m256i in1 = avx2_in.get_at(i_in++);

    __m256i out0;
    __m256i out1;
    mm256_deinterleave(out0, out1, in0, in1);

    avx2_out0.set_at(i, out0);
    avx2_out1.set_at(i, out1);
  }
  i *= AVX2_SIZE_BYTE;

  // Remainder of soft bits.
  for (; i != K; ++i) {
    out[K * 0U + i] = in[2U * i + 0];
    out[K * 1U + i] = in[2U * i + 1];
  }
}

void ldpc_rate_dematcher_avx2_impl::deinterleave_qam16(span<log_likelihood_ratio>       out,
                                                       span<const log_likelihood_ratio> in) const
{
  unsigned E = out.size();
  unsigned K = E / 4;
  unsigned i = 0;

  // Reinterpret pointers.
  mm256::avx2_const_span avx2_in(in, E / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out0(out.subspan(0UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out1(out.subspan(1UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out2(out.subspan(2UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out3(out.subspan(3UL * K, K), K / AVX2_SIZE_BYTE);

  // Do SIMD processing.
  for (unsigned i_end = (K / AVX2_SIZE_BYTE), i_in = 0; i != i_end; ++i) {
    __m256i in0 = avx2_in.get_at(i_in++);
    __m256i in1 = avx2_in.get_at(i_in++);
    __m256i in2 = avx2_in.get_at(i_in++);
    __m256i in3 = avx2_in.get_at(i_in++);

    __m256i deinterleaved_01_even;
    __m256i deinterleaved_01_odd;
    mm256_deinterleave(deinterleaved_01_even, deinterleaved_01_odd, in0, in1);

    __m256i deinterleaved_23_even;
    __m256i deinterleaved_23_odd;
    mm256_deinterleave(deinterleaved_23_even, deinterleaved_23_odd, in2, in3);

    __m256i out0;
    __m256i out2;
    mm256_deinterleave(out0, out2, deinterleaved_01_even, deinterleaved_23_even);

    __m256i out1;
    __m256i out3;
    mm256_deinterleave(out1, out3, deinterleaved_01_odd, deinterleaved_23_odd);

    avx2_out0.set_at(i, out0);
    avx2_out1.set_at(i, out1);
    avx2_out2.set_at(i, out2);
    avx2_out3.set_at(i, out3);
  }
  i *= AVX2_SIZE_BYTE;

  // Remainder of soft bits.
  for (; i != K; ++i) {
    out[K * 0U + i] = in[4U * i + 0];
    out[K * 1U + i] = in[4U * i + 1];
    out[K * 2U + i] = in[4U * i + 2];
    out[K * 3U + i] = in[4U * i + 3];
  }
}

void ldpc_rate_dematcher_avx2_impl::deinterleave_qam64(span<log_likelihood_ratio>       out,
                                                       span<const log_likelihood_ratio> in) const
{
  unsigned E = out.size();
  unsigned K = E / 6;
  unsigned i = 0;

  // Reinterpret pointers.
  mm256::avx2_const_span avx2_in(in, E / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out0(out.subspan(0UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out1(out.subspan(1UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out2(out.subspan(2UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out3(out.subspan(3UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out4(out.subspan(4UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out5(out.subspan(5UL * K, K), K / AVX2_SIZE_BYTE);

  // Do SIMD processing.
  for (unsigned i_end = (K / AVX2_SIZE_BYTE), i_in = 0; i != i_end; ++i) {
    __m256i in0 = avx2_in.get_at(i_in++);
    __m256i in1 = avx2_in.get_at(i_in++);
    __m256i in2 = avx2_in.get_at(i_in++);
    __m256i in3 = avx2_in.get_at(i_in++);
    __m256i in4 = avx2_in.get_at(i_in++);
    __m256i in5 = avx2_in.get_at(i_in++);

    __m256i deinterleaved_0;
    __m256i deinterleaved_1;
    __m256i deinterleaved_2;
    mm256_deinterleave(deinterleaved_0, deinterleaved_1, deinterleaved_2, in0, in1, in2);

    __m256i deinterleaved_3;
    __m256i deinterleaved_4;
    __m256i deinterleaved_5;
    mm256_deinterleave(deinterleaved_3, deinterleaved_4, deinterleaved_5, in3, in4, in5);

    __m256i out0;
    __m256i out3;
    mm256_deinterleave(out0, out3, deinterleaved_0, deinterleaved_3);

    __m256i out1;
    __m256i out4;
    mm256_deinterleave(out1, out4, deinterleaved_1, deinterleaved_4);

    __m256i out2;
    __m256i out5;
    mm256_deinterleave(out2, out5, deinterleaved_2, deinterleaved_5);

    avx2_out0.set_at(i, out0);
    avx2_out1.set_at(i, out1);
    avx2_out2.set_at(i, out2);
    avx2_out3.set_at(i, out3);
    avx2_out4.set_at(i, out4);
    avx2_out5.set_at(i, out5);
  }
  i *= AVX2_SIZE_BYTE;

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
void ldpc_rate_dematcher_avx2_impl::deinterleave_qam256(span<log_likelihood_ratio>       out,
                                                        span<const log_likelihood_ratio> in) const
{
  unsigned E = out.size();
  unsigned K = E / 8;
  unsigned i = 0;

  // Reinterpret pointers.
  mm256::avx2_const_span avx2_in(in, E / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out0(out.subspan(0UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out1(out.subspan(1UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out2(out.subspan(2UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out3(out.subspan(3UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out4(out.subspan(4UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out5(out.subspan(5UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out6(out.subspan(6UL * K, K), K / AVX2_SIZE_BYTE);
  mm256::avx2_span       avx2_out7(out.subspan(7UL * K, K), K / AVX2_SIZE_BYTE);

  // Do SIMD processing.
  for (unsigned i_end = (K / AVX2_SIZE_BYTE), i_in = 0; i != i_end; ++i) {
    __m256i in0 = avx2_in.get_at(i_in++);
    __m256i in1 = avx2_in.get_at(i_in++);
    __m256i in2 = avx2_in.get_at(i_in++);
    __m256i in3 = avx2_in.get_at(i_in++);
    __m256i in4 = avx2_in.get_at(i_in++);
    __m256i in5 = avx2_in.get_at(i_in++);
    __m256i in6 = avx2_in.get_at(i_in++);
    __m256i in7 = avx2_in.get_at(i_in++);

    __m256i deinterleaved_01_even;
    __m256i deinterleaved_01_odd;
    mm256_deinterleave(deinterleaved_01_even, deinterleaved_01_odd, in0, in1);

    __m256i deinterleaved_23_even;
    __m256i deinterleaved_23_odd;
    mm256_deinterleave(deinterleaved_23_even, deinterleaved_23_odd, in2, in3);

    __m256i deinterleaved_45_even;
    __m256i deinterleaved_45_odd;
    mm256_deinterleave(deinterleaved_45_even, deinterleaved_45_odd, in4, in5);

    __m256i deinterleaved_67_even;
    __m256i deinterleaved_67_odd;
    mm256_deinterleave(deinterleaved_67_even, deinterleaved_67_odd, in6, in7);

    __m256i deinterleaved_0123_even_even;
    __m256i deinterleaved_0123_even_odd;
    mm256_deinterleave(
        deinterleaved_0123_even_even, deinterleaved_0123_even_odd, deinterleaved_01_even, deinterleaved_23_even);

    __m256i deinterleaved_4567_even_even;
    __m256i deinterleaved_4567_even_odd;
    mm256_deinterleave(
        deinterleaved_4567_even_even, deinterleaved_4567_even_odd, deinterleaved_45_even, deinterleaved_67_even);

    __m256i deinterleaved_0123_odd_even;
    __m256i deinterleaved_0123_odd_odd;
    mm256_deinterleave(
        deinterleaved_0123_odd_even, deinterleaved_0123_odd_odd, deinterleaved_01_odd, deinterleaved_23_odd);

    __m256i deinterleaved_4567_odd_even;
    __m256i deinterleaved_4567_odd_odd;
    mm256_deinterleave(
        deinterleaved_4567_odd_even, deinterleaved_4567_odd_odd, deinterleaved_45_odd, deinterleaved_67_odd);

    __m256i out0;
    __m256i out4;
    mm256_deinterleave(out0, out4, deinterleaved_0123_even_even, deinterleaved_4567_even_even);

    __m256i out1;
    __m256i out5;
    mm256_deinterleave(out1, out5, deinterleaved_0123_odd_even, deinterleaved_4567_odd_even);

    __m256i out2;
    __m256i out6;
    mm256_deinterleave(out2, out6, deinterleaved_0123_even_odd, deinterleaved_4567_even_odd);

    __m256i out3;
    __m256i out7;
    mm256_deinterleave(out3, out7, deinterleaved_0123_odd_odd, deinterleaved_4567_odd_odd);

    avx2_out0.set_at(i, out0);
    avx2_out1.set_at(i, out1);
    avx2_out2.set_at(i, out2);
    avx2_out3.set_at(i, out3);
    avx2_out4.set_at(i, out4);
    avx2_out5.set_at(i, out5);
    avx2_out6.set_at(i, out6);
    avx2_out7.set_at(i, out7);
  }
  i *= AVX2_SIZE_BYTE;

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
