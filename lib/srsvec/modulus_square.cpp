/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srsvec/modulus_square.h"
#include "srsran/srsvec/simd.h"

using namespace srsran;
using namespace srsvec;

static void modulus_square_simd(float* result, const cf_t* input, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_CF_SIZE
  for (unsigned simd_end = SRSRAN_SIMD_CF_SIZE * (len / SRSRAN_SIMD_CF_SIZE); i != simd_end; i += SRSRAN_SIMD_CF_SIZE) {
    // Load 2 SIMD words of floats.
    simd_f_t simd_input1 = srsran_simd_f_loadu(reinterpret_cast<const float*>(input + i));
    simd_f_t simd_input2 = srsran_simd_f_loadu(reinterpret_cast<const float*>(input + i + SRSRAN_SIMD_CF_SIZE / 2));

    // Calculates the squares.
    simd_f_t simd_mul1 = srsran_simd_f_mul(simd_input1, simd_input1);
    simd_f_t simd_mul2 = srsran_simd_f_mul(simd_input2, simd_input2);

    // Horizontally add the values in pair, it results in adding the squared real and imaginary parts.
    simd_f_t simd_abs2 = srsran_simd_f_hadd(simd_mul1, simd_mul2);

    srsran_simd_f_storeu(result + i, simd_abs2);
  }
#endif // SRSRAN_SIMD_CF_SIZE

  for (; i != len; ++i) {
    float real = std::real(input[i]);
    float imag = std::imag(input[i]);

    result[i] = real * real + imag * imag;
  }
}

void srsran::srsvec::modulus_square(span<float> result, span<const srsran::cf_t> input)
{
  srsran_srsvec_assert_size(result, input);

  modulus_square_simd(result.data(), input.data(), input.size());
}
