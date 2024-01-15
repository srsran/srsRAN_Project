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

#include "srsran/srsvec/modulus_square.h"
#include "simd.h"

using namespace srsran;

static void modulus_square_simd(float* result, const cf_t* input, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_CF_SIZE
  for (unsigned simd_end = SRSRAN_SIMD_CF_SIZE * (len / SRSRAN_SIMD_CF_SIZE); i != simd_end; i += SRSRAN_SIMD_CF_SIZE) {
    // Load 2 SIMD words of floats.
    simd_f_t simd_input1 = srsran_simd_f_loadu((float*)&input[i]);
    simd_f_t simd_input2 = srsran_simd_f_loadu((float*)&input[i + SRSRAN_SIMD_CF_SIZE / 2]);

    // Calculates the squares.
    simd_f_t simd_mul1 = srsran_simd_f_mul(simd_input1, simd_input1);
    simd_f_t simd_mul2 = srsran_simd_f_mul(simd_input2, simd_input2);

    // Horizontally add the values in pair, it results in adding the squared real and imaginary parts.
    simd_f_t simd_abs2 = srsran_simd_f_hadd(simd_mul1, simd_mul2);

    srsran_simd_f_storeu(&result[i], simd_abs2);
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