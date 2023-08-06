/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

// Representation of a set of complex numbers using a pair of AVX2 registers, for the real and imaginary parts.
struct simd_cf_t {
  __m256 re;
  __m256 im;

  // Sets the registers using a complex constant.
  simd_cf_t& operator=(const cf_t a)
  {
    re = _mm256_set1_ps(a.real());
    im = _mm256_set1_ps(a.imag());
    return *this;
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
                                                 span<const cf_t>        port_weights)
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
    port_weights_simd[i_layer] = port_weights[i_layer];
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