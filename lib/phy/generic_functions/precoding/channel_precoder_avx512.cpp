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

#include "channel_precoder_avx512.h"
#include <immintrin.h>

using namespace srsran;

namespace {

// Size of an AVX512 register in complex numbers with 32-bit floating point precision.
static constexpr unsigned AVX512_CF_SIZE = 8;

// Size of an AVX512 register in complex numbers with 8-bit fixed point precision.
static constexpr unsigned AVX512_CI8_SIZE = 32;

// Representation of a set of complex numbers using a pair of AVX512 registers, for the real and imaginary parts.
struct simd_cf_t {
  __m512 re;
  __m512 im;

  // Sets the registers using a complex constant.
  void set1(const cf_t a)
  {
    re = _mm512_set1_ps(a.real());
    im = _mm512_set1_ps(a.imag());
  }
};

// Type to hold a set of complex numbers using an AVX512 register, with interleaved real and imaginary parts.
using simd_cf_interleaved = __m512;

} // namespace

// Multiplication operator for the precoding weights.
simd_cf_interleaved operator*(const simd_cf_interleaved& re, const simd_cf_t& weight)
{
  return _mm512_fmaddsub_ps(re, weight.re, _mm512_mul_ps(_mm512_shuffle_ps(re, re, 0xb1), weight.im));
}

static inline __m512i _mm512_setr_epi16(int16_t __e0,
                                        int16_t __e1,
                                        int16_t __e2,
                                        int16_t __e3,
                                        int16_t __e4,
                                        int16_t __e5,
                                        int16_t __e6,
                                        int16_t __e7,
                                        int16_t __e8,
                                        int16_t __e9,
                                        int16_t __e10,
                                        int16_t __e11,
                                        int16_t __e12,
                                        int16_t __e13,
                                        int16_t __e14,
                                        int16_t __e15,
                                        int16_t __e16,
                                        int16_t __e17,
                                        int16_t __e18,
                                        int16_t __e19,
                                        int16_t __e20,
                                        int16_t __e21,
                                        int16_t __e22,
                                        int16_t __e23,
                                        int16_t __e24,
                                        int16_t __e25,
                                        int16_t __e26,
                                        int16_t __e27,
                                        int16_t __e28,
                                        int16_t __e29,
                                        int16_t __e30,
                                        int16_t __e31)
{
  return __extension__(__m512i)(__v32hi){__e0,  __e1,  __e2,  __e3,  __e4,  __e5,  __e6,  __e7,  __e8,  __e9,  __e10,
                                         __e11, __e12, __e13, __e14, __e15, __e16, __e17, __e18, __e19, __e20, __e21,
                                         __e22, __e23, __e24, __e25, __e26, __e27, __e28, __e29, __e30, __e31};
}

static inline void from_ci8_to_cf(simd_cf_interleaved& out0,
                                  simd_cf_interleaved& out1,
                                  simd_cf_interleaved& out2,
                                  simd_cf_interleaved& out3,
                                  __m512i              in)
{
  __m512i in16_0 = _mm512_cvtepi8_epi16(_mm512_extracti64x4_epi64(in, 0));
  __m512i in16_1 = _mm512_cvtepi8_epi16(_mm512_extracti64x4_epi64(in, 1));

  __m512i in32_0 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(in16_0, 0));
  __m512i in32_1 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(in16_0, 1));
  __m512i in32_2 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(in16_1, 0));
  __m512i in32_3 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(in16_1, 1));

  const int rounding = _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC;
  out0               = _mm512_cvt_roundepi32_ps(in32_0, rounding);
  out1               = _mm512_cvt_roundepi32_ps(in32_1, rounding);
  out2               = _mm512_cvt_roundepi32_ps(in32_2, rounding);
  out3               = _mm512_cvt_roundepi32_ps(in32_3, rounding);
}

static inline void layer2_map_and_ci8_to_cf(simd_cf_interleaved& out0,
                                            simd_cf_interleaved& out1,
                                            simd_cf_interleaved& out2,
                                            simd_cf_interleaved& out3,
                                            const __m512i&       in)
{
  __m512i idx = _mm512_setr_epi16(0,
                                  2,
                                  4,
                                  6,
                                  8,
                                  10,
                                  12,
                                  14,
                                  16,
                                  18,
                                  20,
                                  22,
                                  24,
                                  26,
                                  28,
                                  30,
                                  1,
                                  3,
                                  5,
                                  7,
                                  9,
                                  11,
                                  13,
                                  15,
                                  17,
                                  19,
                                  21,
                                  23,
                                  25,
                                  27,
                                  29,
                                  31);
  __m512i tmp = _mm512_permutexvar_epi16(idx, in);

  from_ci8_to_cf(out0, out1, out2, out3, tmp);
}

static inline void layer3_map_and_ci8_to_cf(simd_cf_interleaved& out0,
                                            simd_cf_interleaved& out1,
                                            simd_cf_interleaved& out2,
                                            const __m512i&       in)
{
  __m512i idx = _mm512_setr_epi16(
      0, 3, 6, 9, 12, 15, 18, 21, 1, 4, 7, 10, 13, 16, 19, 22, 2, 5, 8, 11, 14, 17, 20, 23, 0, 0, 0, 0, 0, 0, 0, 0);
  __m512i tmp = _mm512_permutexvar_epi16(idx, in);

  __m512 unused;
  from_ci8_to_cf(out0, out1, out2, unused, tmp);
}

static inline void layer4_map_and_ci8_to_cf(simd_cf_interleaved& out0,
                                            simd_cf_interleaved& out1,
                                            simd_cf_interleaved& out2,
                                            simd_cf_interleaved& out3,
                                            const __m512i&       in)
{
  __m512i idx = _mm512_setr_epi16(0,
                                  4,
                                  8,
                                  12,
                                  16,
                                  20,
                                  24,
                                  28,
                                  1,
                                  5,
                                  9,
                                  13,
                                  17,
                                  21,
                                  25,
                                  29,
                                  2,
                                  6,
                                  10,
                                  14,
                                  18,
                                  22,
                                  26,
                                  30,
                                  3,
                                  7,
                                  11,
                                  15,
                                  19,
                                  23,
                                  27,
                                  31);
  __m512i tmp = _mm512_permutexvar_epi16(idx, in);

  from_ci8_to_cf(out0, out1, out2, out3, tmp);
}

void channel_precoder_avx512::apply_precoding_port(span<cf_t>              port_re,
                                                   const re_buffer_reader& input_re,
                                                   span<const cf_t>        port_weights) const
{
  unsigned nof_re     = input_re.get_nof_re();
  unsigned nof_layers = input_re.get_nof_slices();

  // Create a list of all the input layer RE views.
  std::array<span<const cf_t>, precoding_constants::MAX_NOF_LAYERS> layer_re_view_list;

  // Array holding SIMD registers initialized with the precoding weights. Use C-style for GCC compatibility in Debug.
  simd_cf_t port_weights_simd[precoding_constants::MAX_NOF_LAYERS];

  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    // Fill the RE view list.
    layer_re_view_list[i_layer] = input_re.get_slice(i_layer);

    // Set the SIMD registers with the port weights.
    port_weights_simd[i_layer].set1(port_weights[i_layer]);
  }

  unsigned i_re = 0;

  for (unsigned max_re = (nof_re / AVX512_CF_SIZE) * AVX512_CF_SIZE; i_re != max_re; i_re += AVX512_CF_SIZE) {
    // Load layer 0 RE.
    simd_cf_interleaved re_in = _mm512_loadu_ps(reinterpret_cast<const float*>(&layer_re_view_list[0][i_re]));

    // Multiply the input REs by the precoding coefficient.
    simd_cf_interleaved re_out = re_in * port_weights_simd[0];

    for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
      // Load layer RE.
      re_in = _mm512_loadu_ps(reinterpret_cast<const float*>(&layer_re_view_list[i_layer][i_re]));

      // Multiply the input REs by the coefficient and add to the contributions of other layers.
      re_out += re_in * port_weights_simd[i_layer];
    }

    // Store.
    _mm512_storeu_ps(reinterpret_cast<float*>(&port_re[i_re]), re_out);
  }

  for (; i_re != nof_re; ++i_re) {
    port_re[i_re] = layer_re_view_list[0][i_re] * port_weights[0];

    for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
      port_re[i_re] += layer_re_view_list[i_layer][i_re] * port_weights[i_layer];
    }
  }
}

void channel_precoder_avx512::apply_layer_map_and_precoding(re_buffer_writer&              output,
                                                            span<const ci8_t>              input,
                                                            const precoding_weight_matrix& precoding) const
{
  unsigned nof_re     = output.get_nof_re();
  unsigned nof_layers = precoding.get_nof_layers();
  unsigned nof_ports  = precoding.get_nof_ports();
  unsigned i_re       = 0;

  simd_cf_t  weights[4][4];
  span<cf_t> outputs[4];
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    span<const cf_t> port_coeff = precoding.get_port_coefficients(i_port);
    outputs[i_port]             = output.get_slice(i_port);
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      weights[i_port][i_layer].set1(port_coeff[i_layer]);
    }
  }

  if (nof_layers == 1) {
    for (unsigned i_re_end = (nof_re / AVX512_CI8_SIZE) * AVX512_CI8_SIZE; i_re != i_re_end; i_re += AVX512_CI8_SIZE) {
      __m512i in8 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(input.data() + i_re));

      simd_cf_interleaved infp_0, infp_1, infp_2, infp_3;
      from_ci8_to_cf(infp_0, infp_1, infp_2, infp_3, in8);

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        simd_cf_interleaved result0 = infp_0 * weights[i_port][0];
        simd_cf_interleaved result1 = infp_1 * weights[i_port][0];
        simd_cf_interleaved result2 = infp_2 * weights[i_port][0];
        simd_cf_interleaved result3 = infp_3 * weights[i_port][0];

        _mm512_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re]), result0);
        _mm512_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re + 8]), result1);
        _mm512_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re + 16]), result2);
        _mm512_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re + 24]), result3);
      }
    }
  }

  if (nof_layers == 2) {
    static constexpr unsigned AVX512_NOF_RE = AVX512_CI8_SIZE / 2;
    for (unsigned i_re_end = (nof_re / AVX512_NOF_RE) * AVX512_NOF_RE; i_re != i_re_end; i_re += AVX512_NOF_RE) {
      __m512i in8 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(&input[2 * i_re]));

      simd_cf_interleaved infp_0, infp_1, infp_2, infp_3;
      layer2_map_and_ci8_to_cf(infp_0, infp_1, infp_2, infp_3, in8);

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        simd_cf_interleaved result0 = infp_0 * weights[i_port][0] + infp_2 * weights[i_port][1];
        simd_cf_interleaved result1 = infp_1 * weights[i_port][0] + infp_3 * weights[i_port][1];

        _mm512_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re]), result0);
        _mm512_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re + AVX512_CF_SIZE]), result1);
      }
    }
  }

  if (nof_layers == 3) {
    static constexpr unsigned AVX512_NOF_RE = AVX512_CI8_SIZE / 4;

    // For three layers, more input symbols are loaded than those actually used. The last loop iteration must be skipped
    // in order to avoid exceeding the input buffer boundaries.
    unsigned i_re_end = (nof_re > AVX512_NOF_RE) ? (nof_re / AVX512_NOF_RE) * AVX512_NOF_RE - AVX512_NOF_RE : 0U;
    for (; i_re != i_re_end; i_re += 8) {
      __m512i in8 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(&input[3 * i_re]));

      simd_cf_interleaved infp_0, infp_1, infp_2;
      layer3_map_and_ci8_to_cf(infp_0, infp_1, infp_2, in8);

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        simd_cf_interleaved result =
            infp_0 * weights[i_port][0] + infp_1 * weights[i_port][1] + infp_2 * weights[i_port][2];
        _mm512_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re]), result);
      }
    }
  }

  if (nof_layers == 4) {
    static constexpr unsigned AVX512_NOF_RE = AVX512_CI8_SIZE / 4;
    for (unsigned i_re_end = (nof_re / AVX512_NOF_RE) * AVX512_NOF_RE; i_re != i_re_end; i_re += AVX512_NOF_RE) {
      __m512i in8 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(&input[4 * i_re]));

      simd_cf_interleaved infp_0, infp_1, infp_2, infp_3;
      layer4_map_and_ci8_to_cf(infp_0, infp_1, infp_2, infp_3, in8);

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        simd_cf_interleaved result = infp_0 * weights[i_port][0] + infp_1 * weights[i_port][1] +
                                     infp_2 * weights[i_port][2] + infp_3 * weights[i_port][3];
        _mm512_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re]), result);
      }
    }
  }

  for (; i_re != nof_re; ++i_re) {
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      span<const cf_t> port_weights = precoding.get_port_coefficients(i_port);
      span<cf_t>       port_re      = output.get_slice(i_port);

      cf_t sum = to_cf(input[nof_layers * i_re]) * port_weights[0];
      for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
        sum += to_cf(input[nof_layers * i_re + i_layer]) * port_weights[i_layer];
      }
      port_re[i_re] = sum;
    }
  }
}
