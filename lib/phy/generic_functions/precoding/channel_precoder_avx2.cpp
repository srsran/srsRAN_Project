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

#include "channel_precoder_avx2.h"
#include <immintrin.h>

using namespace srsran;

namespace {

// Size of an AVX2 register in complex numbers with 32-bit floating point precision.
static constexpr unsigned AVX2_CF_SIZE = 4;

// Size of an AVX2 register in complex numbers with 8-bit fixed point precision.
static constexpr unsigned AVX2_CI8_SIZE = 16;

// Representation of a set of complex numbers using a pair of AVX2 registers, for the real and imaginary parts.
struct simd_cf_t {
  __m256 re;
  __m256 im;

  // Sets the registers using a complex constant.
  void set1(const cf_t a)
  {
    re = _mm256_set1_ps(a.real());
    im = _mm256_set1_ps(a.imag());
  }
};

// Type to hold a set of complex numbers using an AVX2 register, with interleaved real and imaginary parts.
using simd_cf_interleaved = __m256;

// Multiplication operator for the precoding weights.
simd_cf_interleaved operator*(const simd_cf_interleaved& re, const simd_cf_t& weight)
{
  return _mm256_fmaddsub_ps(re, weight.re, _mm256_mul_ps(_mm256_shuffle_ps(re, re, 0xb1), weight.im));
}

} // namespace

void channel_precoder_avx2::apply_precoding_port(span<cf_t>              port_re,
                                                 const re_buffer_reader& input_re,
                                                 span<const cf_t>        port_weights) const
{
  unsigned nof_re     = input_re.get_nof_re();
  unsigned nof_layers = input_re.get_nof_slices();

  // Create a list of all the input layer RE views.
  std::array<span<const cf_t>, precoding_constants::MAX_NOF_LAYERS> layer_re_view_list;

  // Array holding SIMD registers initialized with the precoding weights.
  std::array<simd_cf_t, precoding_constants::MAX_NOF_LAYERS> port_weights_simd;

  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    // Fill the RE view list.
    layer_re_view_list[i_layer] = input_re.get_slice(i_layer);

    // Set the SIMD registers with the port weights.
    port_weights_simd[i_layer].set1(port_weights[i_layer]);
  }

  unsigned i_re = 0;

  for (unsigned max_re = (nof_re / AVX2_CF_SIZE) * AVX2_CF_SIZE; i_re != max_re; i_re += AVX2_CF_SIZE) {
    // Load layer 0 RE.
    simd_cf_interleaved re_in = _mm256_loadu_ps(reinterpret_cast<const float*>(&layer_re_view_list[0][i_re]));

    // Multiply the input REs by the precoding coefficient.
    simd_cf_interleaved re_out = re_in * port_weights_simd[0];

    for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
      // Load layer RE.
      re_in = _mm256_loadu_ps(reinterpret_cast<const float*>(&layer_re_view_list[i_layer][i_re]));

      // Multiply the input REs by the coefficient and add to the contributions of other layers.
      re_out += re_in * port_weights_simd[i_layer];
    }

    // Store.
    _mm256_storeu_ps(reinterpret_cast<float*>(&port_re[i_re]), re_out);
  }

  for (; i_re != nof_re; ++i_re) {
    port_re[i_re] = layer_re_view_list[0][i_re] * port_weights[0];

    for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
      port_re[i_re] += layer_re_view_list[i_layer][i_re] * port_weights[i_layer];
    }
  }
}

// Converts ci8_t symbols into cf_t and stores them on four AVX2 registers.
static inline void from_ci8_to_cf(simd_cf_interleaved& out0,
                                  simd_cf_interleaved& out1,
                                  simd_cf_interleaved& out2,
                                  simd_cf_interleaved& out3,
                                  const __m256i        in)
{
  // Expand to 16 bit.
  __m256i in16_0 = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(in, 0));
  __m256i in16_1 = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(in, 1));

  // Expand to 32 bit.
  __m256i in32_0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(in16_0, 0));
  __m256i in32_1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(in16_0, 1));
  __m256i in32_2 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(in16_1, 0));
  __m256i in32_3 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(in16_1, 1));

  // Convert to floating point.
  out0 = _mm256_cvtepi32_ps(in32_0);
  out1 = _mm256_cvtepi32_ps(in32_1);
  out2 = _mm256_cvtepi32_ps(in32_2);
  out3 = _mm256_cvtepi32_ps(in32_3);
}

// Applies layer mapping for two layers and converts the symbols to cf_t.
static inline void layer2_map_and_ci8_to_cf(simd_cf_interleaved& out0_l0,
                                            simd_cf_interleaved& out0_l1,
                                            simd_cf_interleaved& out1_l0,
                                            simd_cf_interleaved& out1_l1,
                                            const __m256i        in)
{
  // Apply layer mapping to each 128 bit lane. The lower 128 bits of the input register contain REs 0..3, while the
  // higher 128 bits contain REs 4..7.
  __m256i idx = _mm256_setr_epi8(
      0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15, 0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15);
  __m256i tmp = _mm256_shuffle_epi8(in, idx);

  from_ci8_to_cf(out0_l0, out0_l1, out1_l0, out1_l1, tmp);
}

// Applies layer mapping for three layers and converts the symbols to cf_t.
static inline void layer3_map_and_ci8_to_cf(simd_cf_interleaved& out_l0,
                                            simd_cf_interleaved& out_l1,
                                            simd_cf_interleaved& out_l2,
                                            const __m256i        in)
{
  simd_cf_interleaved unused;

  // Align the symbol corresponding to RE 2 and layer 0 to the 128 bit boundary.
  __m256i idx = _mm256_setr_epi32(0, 1, 2, 0, 3, 4, 5, 0);
  __m256i tmp = _mm256_permutevar8x32_epi32(in, idx);

  // Deinterlave 16 bit symbols belonging to REs 0 and 1 using the lower 128 bits and symbols belonging to REs 2 and 3
  // using the higher 128 bits. This achieves layer mapping for REs 0..1 and 2..3 separately.
  idx = _mm256_setr_epi8(
      0, 1, 6, 7, 2, 3, 8, 9, 4, 5, 10, 11, 0, 0, 0, 0, 0, 1, 6, 7, 2, 3, 8, 9, 4, 5, 10, 11, 0, 0, 0, 0);
  tmp = _mm256_shuffle_epi8(tmp, idx);

  // Interleave the lower and higher 128 bits in groups of 2 symbols, this concatenates each REs 0..4 for all layers.
  idx = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 0, 0);
  tmp = _mm256_permutevar8x32_epi32(tmp, idx);

  from_ci8_to_cf(out_l0, out_l1, out_l2, unused, tmp);
}

// Applies layer mapping for four layers and converts the symbols to cf_t.
static inline void layer4_map_and_ci8_to_cf(simd_cf_interleaved& out_l0,
                                            simd_cf_interleaved& out_l1,
                                            simd_cf_interleaved& out_l2,
                                            simd_cf_interleaved& out_l3,
                                            const __m256i        in)
{
  // Apply layer mapping to each 128 bit lane. The lower 128 bits of the input register contain REs 0 and 1, while the
  // higher 128 bits contain REs 2 and 3.
  __m256i idx = _mm256_setr_epi8(
      0, 1, 8, 9, 2, 3, 10, 11, 4, 5, 12, 13, 6, 7, 14, 15, 0, 1, 8, 9, 2, 3, 10, 11, 4, 5, 12, 13, 6, 7, 14, 15);
  __m256i tmp = _mm256_shuffle_epi8(in, idx);

  // Interleave the lower and higher 128 bits in groups of 2 symbols, this concatenates each REs 0..4 for all layers.
  idx = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);
  tmp = _mm256_permutevar8x32_epi32(tmp, idx);

  from_ci8_to_cf(out_l0, out_l1, out_l2, out_l3, tmp);
}

void channel_precoder_avx2::apply_layer_map_and_precoding(re_buffer_writer&              output,
                                                          span<const ci8_t>              input,
                                                          const precoding_weight_matrix& precoding) const
{
  unsigned nof_re     = output.get_nof_re();
  unsigned nof_layers = precoding.get_nof_layers();
  unsigned nof_ports  = precoding.get_nof_ports();

  // Registers to load the precoding weights.
  simd_cf_t weights[precoding_constants::MAX_NOF_PORTS][precoding_constants::MAX_NOF_LAYERS];

  // Views to store the precoded symbols.
  span<cf_t> outputs[precoding_constants::MAX_NOF_PORTS];

  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    span<const cf_t> port_coeff = precoding.get_port_coefficients(i_port);
    outputs[i_port]             = output.get_slice(i_port);
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      weights[i_port][i_layer].set1(port_coeff[i_layer]);
    }
  }

  unsigned i_re = 0;
  if (nof_layers == 1) {
    for (unsigned i_re_end = (nof_re / AVX2_CI8_SIZE) * AVX2_CI8_SIZE; i_re != i_re_end; i_re += AVX2_CI8_SIZE) {
      // Load input.
      __m256i in8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(input.data() + i_re));

      // Convert to complex float.
      simd_cf_interleaved infp_0, infp_1, infp_2, infp_3;
      from_ci8_to_cf(infp_0, infp_1, infp_2, infp_3, in8);

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        // Apply precoding.
        simd_cf_interleaved result0 = infp_0 * weights[i_port][0];
        simd_cf_interleaved result1 = infp_1 * weights[i_port][0];
        simd_cf_interleaved result2 = infp_2 * weights[i_port][0];
        simd_cf_interleaved result3 = infp_3 * weights[i_port][0];

        // Store.
        _mm256_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re]), result0);
        _mm256_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re + AVX2_CF_SIZE]), result1);
        _mm256_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re + (2 * AVX2_CF_SIZE)]), result2);
        _mm256_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re + (3 * AVX2_CF_SIZE)]), result3);
      }
    }
  }

  if (nof_layers == 2) {
    static constexpr unsigned AVX2_NOF_RE = AVX2_CI8_SIZE / 2;
    for (unsigned i_re_end = (nof_re / AVX2_NOF_RE) * AVX2_NOF_RE; i_re != i_re_end; i_re += AVX2_NOF_RE) {
      // Load input.
      __m256i in8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&input[2 * i_re]));

      // Layer map and convert to complex float.
      simd_cf_interleaved infp0_l0, infp0_l1, infp1_l0, infp1_l1;
      layer2_map_and_ci8_to_cf(infp0_l0, infp0_l1, infp1_l0, infp1_l1, in8);

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        // Apply precoding.
        simd_cf_interleaved result0 = infp0_l0 * weights[i_port][0] + infp0_l1 * weights[i_port][1];
        simd_cf_interleaved result1 = infp1_l0 * weights[i_port][0] + infp1_l1 * weights[i_port][1];

        // Store.
        _mm256_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re]), result0);
        _mm256_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re + AVX2_CF_SIZE]), result1);
      }
    }
  }

  if (nof_layers == 3) {
    static constexpr unsigned AVX2_NOF_RE = AVX2_CI8_SIZE / 4;
    // For three layers, more input symbols are loaded than those actually used. The last loop iteration must be skipped
    // in order to avoid exceeding the input buffer boundaries.
    unsigned i_re_end = (nof_re > AVX2_NOF_RE) ? (nof_re / AVX2_NOF_RE) * AVX2_NOF_RE - AVX2_NOF_RE : 0U;
    for (; i_re != i_re_end; i_re += AVX2_NOF_RE) {
      // Load input.
      __m256i in8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&input[3 * i_re]));

      // Layer map and convert to complex float.
      simd_cf_interleaved infp_0, infp_1, infp_2;
      layer3_map_and_ci8_to_cf(infp_0, infp_1, infp_2, in8);

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        // Apply precoding.
        simd_cf_interleaved result =
            infp_0 * weights[i_port][0] + infp_1 * weights[i_port][1] + infp_2 * weights[i_port][2];

        // Store.
        _mm256_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re]), result);
      }
    }
  }

  if (nof_layers == 4) {
    static constexpr unsigned AVX2_NOF_RE = AVX2_CI8_SIZE / 4;
    for (unsigned i_re_end = (nof_re / AVX2_NOF_RE) * AVX2_NOF_RE; i_re != i_re_end; i_re += AVX2_NOF_RE) {
      // Load input.
      __m256i in8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&input[4 * i_re]));

      // Layer map and convert to complex float.
      simd_cf_interleaved infp_0, infp_1, infp_2, infp_3;
      layer4_map_and_ci8_to_cf(infp_0, infp_1, infp_2, infp_3, in8);

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        // Apply precoding.
        simd_cf_interleaved result = infp_0 * weights[i_port][0] + infp_1 * weights[i_port][1] +
                                     infp_2 * weights[i_port][2] + infp_3 * weights[i_port][3];

        // Store.
        _mm256_storeu_ps(reinterpret_cast<float*>(&outputs[i_port][i_re]), result);
      }
    }
  }

  // Process remaining symbols.
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
