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

static void modulus_square_simd(float* result, const cbf16_t* input, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_CF_SIZE
  for (unsigned simd_end = SRSRAN_SIMD_CF_SIZE * (len / SRSRAN_SIMD_CF_SIZE); i != simd_end; i += SRSRAN_SIMD_CF_SIZE) {
    // Load input from 16-bit brain float to single precision floating point.
    simd_cf_t simd_input = srsran_simd_cbf16_loadu(input + i);

    // Calculate the modulus squares.
    simd_f_t simd_abs2 = srsran_simd_cf_norm_sq(simd_input);

    // Store result.
    srsran_simd_f_storeu(result + i, simd_abs2);
  }
#endif // SRSRAN_SIMD_CF_SIZE

  for (; i != len; ++i) {
    cf_t  cf_input = to_cf(input[i]);
    float real     = std::real(cf_input);
    float imag     = std::imag(cf_input);

    result[i] = real * real + imag * imag;
  }
}

static void modulus_square_and_add_simd(float* result, const cf_t* input, const float* offset, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_CF_SIZE
  for (unsigned simd_end = SRSRAN_SIMD_CF_SIZE * (len / SRSRAN_SIMD_CF_SIZE); i != simd_end; i += SRSRAN_SIMD_CF_SIZE) {
    // Load 2 SIMD words of floats.
    simd_f_t simd_input1 = srsran_simd_f_loadu(reinterpret_cast<const float*>(input + i));
    simd_f_t simd_input2 = srsran_simd_f_loadu(reinterpret_cast<const float*>(input + i + SRSRAN_SIMD_CF_SIZE / 2));

    // Load offset.
    simd_f_t simd_offset = srsran_simd_f_loadu(&offset[i]);

    // Calculates the squares.
    simd_f_t simd_mul1 = simd_input1 * simd_input1;
    simd_f_t simd_mul2 = simd_input2 * simd_input2;

    // Horizontally add the values in pair, it results in adding the squared real and imaginary parts.
    simd_offset += srsran_simd_f_hadd(simd_mul1, simd_mul2);

    srsran_simd_f_storeu(result + i, simd_offset);
  }
#endif // SRSRAN_SIMD_CF_SIZE

  for (; i != len; ++i) {
    float real = std::real(input[i]);
    float imag = std::imag(input[i]);

    result[i] = real * real + imag * imag + offset[i];
  }
}

static void modulus_square_and_add_simd(float* result, const cbf16_t* input, const float* offset, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_CF_SIZE
  for (unsigned simd_end = SRSRAN_SIMD_CF_SIZE * (len / SRSRAN_SIMD_CF_SIZE); i != simd_end; i += SRSRAN_SIMD_CF_SIZE) {
    // Load input from 16-bit brain float to single precision floating point.
    simd_cf_t simd_input = srsran_simd_cbf16_loadu(input + i);

    // Calculate the modulus squares.
    simd_f_t simd_abs2 = srsran_simd_cf_norm_sq(simd_input);

    // Load offset.
    simd_f_t simd_offset = srsran_simd_f_loadu(&offset[i]);

    // Add the modulus squares to the offset.
    simd_offset += simd_abs2;

    srsran_simd_f_storeu(result + i, simd_offset);
  }
#endif // SRSRAN_SIMD_CF_SIZE

  for (; i != len; ++i) {
    cf_t  cf_input = to_cf(input[i]);
    float real     = std::real(cf_input);
    float imag     = std::imag(cf_input);

    result[i] = real * real + imag * imag + offset[i];
  }
}

void srsran::srsvec::modulus_square(span<float> result, span<const cf_t> input)
{
  srsran_srsvec_assert_size(result, input);

  modulus_square_simd(result.data(), input.data(), input.size());
}

void srsran::srsvec::modulus_square(span<float> result, span<const cbf16_t> input)
{
  srsran_srsvec_assert_size(result, input);

  modulus_square_simd(result.data(), input.data(), input.size());
}

void srsran::srsvec::modulus_square_and_add(span<float> result, span<const cf_t> input, span<const float> offset)
{
  srsran_srsvec_assert_size(result, input);
  srsran_srsvec_assert_size(result, offset);

  modulus_square_and_add_simd(result.data(), input.data(), offset.data(), input.size());
}

void srsran::srsvec::modulus_square_and_add(span<float> result, span<const cbf16_t> input, span<const float> offset)
{
  srsran_srsvec_assert_size(result, input);
  srsran_srsvec_assert_size(result, offset);

  modulus_square_and_add_simd(result.data(), input.data(), offset.data(), input.size());
}
