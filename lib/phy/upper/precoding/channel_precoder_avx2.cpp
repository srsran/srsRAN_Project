/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "channel_precoder_avx2.h"
#include <immintrin.h>

using namespace srsran;

// Size of an AVX2 register in number of 32-bit floats.
static constexpr unsigned AVX2_F_SIZE = 8;

// Representation of a set of complex numbers using a pair of AVX2 registers, for the real and imaginary parts.
struct simd_cf_t {
  __m256 re;
  __m256 im;
};

// Loads complex data from a buffer into a pair of AVX2 registers.
static simd_cf_t load_unaligned(const cf_t* ptr)
{
  __m256 in1 = _mm256_permute_ps(_mm256_loadu_ps((float*)(ptr)), 0b11011000);
  __m256 in2 = _mm256_permute_ps(_mm256_loadu_ps((float*)(ptr + 4)), 0b11011000);

  return {_mm256_unpacklo_ps(in1, in2), _mm256_unpackhi_ps(in1, in2)};
}

// Stores data from a pair of AVX2 registers into a buffer of complex data.
static void store_unaligned(cf_t* ptr, const simd_cf_t& re_out_register)
{
  __m256 out1 = _mm256_permute_ps(re_out_register.re, 0b11011000);
  __m256 out2 = _mm256_permute_ps(re_out_register.im, 0b11011000);
  _mm256_storeu_ps((float*)(ptr), _mm256_unpacklo_ps(out1, out2));
  _mm256_storeu_ps((float*)(ptr + 4), _mm256_unpackhi_ps(out1, out2));
}

// Scales the input RE with a precoding coefficient and adds the result to the value of the output register.
static void multiply_weight_add(simd_cf_t& re_out, const simd_cf_t& re_in, cf_t weight)
{
  simd_cf_t weight_simd{_mm256_set1_ps(weight.real()), _mm256_set1_ps(weight.imag())};

  re_out.re += _mm256_fmsub_ps(re_in.re, weight_simd.re, _mm256_mul_ps(re_in.im, weight_simd.im));
  re_out.im += _mm256_fmadd_ps(re_in.re, weight_simd.im, _mm256_mul_ps(re_in.im, weight_simd.re));
}

// Scales the input RE with a precoding coefficient and adds the result to the value of the output register.
static simd_cf_t multiply_weight(const simd_cf_t& re_in, cf_t weight)
{
  simd_cf_t weight_simd{_mm256_set1_ps(weight.real()), _mm256_set1_ps(weight.imag())};

  return {_mm256_fmsub_ps(re_in.re, weight_simd.re, _mm256_mul_ps(re_in.im, weight_simd.im)),
          _mm256_fmadd_ps(re_in.re, weight_simd.im, _mm256_mul_ps(re_in.im, weight_simd.re))};
}

void channel_precoder_avx2::apply_precoding_port(span<srsran::cf_t>              port_re,
                                                 const srsran::re_buffer_reader& input_re,
                                                 span<const srsran::cf_t>        port_weights)
{
  unsigned nof_re     = input_re.get_nof_re();
  unsigned nof_layers = input_re.get_nof_slices();

  // Create a list of all the input layer RE views.
  std::array<span<const cf_t>, precoding_constants::MAX_NOF_LAYERS> layer_re_view_list;
  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    layer_re_view_list[i_layer] = input_re.get_slice(i_layer);
  }

  unsigned i_re = 0;

  for (unsigned max_re = (nof_re / AVX2_F_SIZE) * AVX2_F_SIZE; i_re != max_re; i_re += AVX2_F_SIZE) {
    // Load layer 0 RE.
    simd_cf_t re_in = load_unaligned(&(layer_re_view_list[0][i_re]));

    // Multiply the input RE by the precoding coefficient.
    simd_cf_t re_out = multiply_weight(re_in, port_weights[0]);

    for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
      // Load layer RE.
      re_in = load_unaligned(&(layer_re_view_list[i_layer][i_re]));

      // Multiply the input RE by the coefficient and add to the contributions of other layers.
      multiply_weight_add(re_out, re_in, port_weights[i_layer]);
    }

    // Store.
    store_unaligned(&port_re[i_re], re_out);
  }

  for (; i_re != nof_re; ++i_re) {
    port_re[i_re] = layer_re_view_list[0][i_re] * port_weights[0];

    for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
      port_re[i_re] += layer_re_view_list[i_layer][i_re] * port_weights[i_layer];
    }
  }
}