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

#include "srsran/srsvec/compare.h"

#include "simd.h"
#include "srsran/support/math_utils.h"
#include <numeric>

using namespace srsran;
using namespace srsvec;

const char* srsran::srsvec::detail::find(span<const char> input, char value)
{
  // Input index.
  unsigned index = 0;

#ifdef HAVE_AVX2
  // Advances the input index to either the first SIMD word that contains value or the last index rounded to 32.
  for (unsigned simd_index_end = 32 * (input.size() / 32); index != simd_index_end; index += 32) {
    // Load 32 consecutive words starting at index.
    __m256i simd_input = _mm256_loadu_si256((__m256i*)&input[index]);
    // Compare the 32 words with the value.
    __m256i simd_eq_filler_bit = _mm256_cmpeq_epi8(_mm256_set1_epi8(value), simd_input);
    // Get the MSB of each word.
    unsigned mask = _mm256_movemask_epi8(simd_eq_filler_bit);
    // If it is not zero, it means at least one of the words in the SIMD register is equal to value.
    if (mask > 0) {
      break;
    }
  }
#endif // HAVE_AVX2

#ifdef HAVE_NEON
  bool found = false;
  // Advances the input index to either the first SIMD word that contains value or the last index rounded to 16.
  for (unsigned simd_index_end = 16 * (input.size() / 16); index != simd_index_end; index += 16) {
    // Load 16 consecutive words starting at index.
    int8x16_t simd_input = vld1q_s8(reinterpret_cast<const int8_t*>(&input[index]));
    // Compare the 16 words with the value.
    uint8x16_t mask_u8 = vceqq_s8(vdupq_n_s8((int8_t)value), simd_input);
    uint8_t    mask    = vmaxvq_u8(mask_u8);
    if (mask != 0) {
      found = true;
      break;
    }
  }
  // Advances the input index to either the first SIMD word that contains value or the last index rounded to 8.
  for (unsigned simd_index_end = 8 * (input.size() / 8); !found && index != simd_index_end; index += 8) {
    // Load 8 consecutive words starting at index.
    int8x8_t simd_input = vld1_s8(reinterpret_cast<const int8_t*>(&input[index]));
    // Compare the 8 words with the value.
    uint8x8_t mask_u8 = vceq_s8(vdup_n_s8((int8_t)value), simd_input);
    uint8_t   mask    = vmaxv_u8(mask_u8);
    if (mask != 0) {
      break;
    }
  }
#endif // HAVE_NEON

  // Keeps iterating from the current index to the end.
  for (; index != input.size(); ++index) {
    // Early return if a word is equal to value.
    if (input[index] == value) {
      return &input[index];
    }
  }

  // There is no word equal to value if the execution reached here.
  return input.end();
}

std::pair<unsigned, float> srsran::srsvec::max_abs_element(span<const cf_t> x)
{
  unsigned max_index = 0;
  float    max_abs2  = 0.0F;

  unsigned i = 0, len = x.size();

#if SRSRAN_SIMD_CF_SIZE
  // Prepare range of indexes in SIMD register.
  alignas(SIMD_BYTE_ALIGN) std::array<int32_t, SRSRAN_SIMD_CF_SIZE> simd_vector_max_indexes;
  std::iota(simd_vector_max_indexes.begin(), simd_vector_max_indexes.end(), 0);
  simd_i_t simd_indexes = srsran_simd_i_load(simd_vector_max_indexes.data());

  simd_i_t simd_inc         = srsran_simd_i_set1(SRSRAN_SIMD_CF_SIZE);
  simd_i_t simd_max_indexes = srsran_simd_i_set1(0);

  simd_f_t simd_max_values = srsran_simd_f_set1(-INFINITY);

  for (unsigned simd_end = SRSRAN_SIMD_CF_SIZE * (len / SRSRAN_SIMD_CF_SIZE); i != simd_end; i += SRSRAN_SIMD_CF_SIZE) {
    // Load 2 SIMD words of floats.
    simd_f_t simd_x1 = srsran_simd_f_loadu((float*)&x[i]);
    simd_f_t simd_x2 = srsran_simd_f_loadu((float*)&x[i + SRSRAN_SIMD_CF_SIZE / 2]);

    // Calculates the squares.
    simd_f_t simd_mul1 = srsran_simd_f_mul(simd_x1, simd_x1);
    simd_f_t simd_mul2 = srsran_simd_f_mul(simd_x2, simd_x2);

    // Horizontally add the values in pair, it results in adding the squared real and imaginary parts.
    simd_f_t simd_abs2 = srsran_simd_f_hadd(simd_mul1, simd_mul2);

    // Get SIMD selector mask.
    simd_sel_t res = srsran_simd_f_max(simd_abs2, simd_max_values);

    // Select the indexes and values for the maximum.
    simd_max_indexes = srsran_simd_i_select(simd_max_indexes, simd_indexes, res);
    simd_max_values  = srsran_simd_f_select(simd_max_values, simd_abs2, res);

    // Increment indexes.
    simd_indexes = srsran_simd_i_add(simd_indexes, simd_inc);
  }

  // Store the indexes and values from SIMD registers into arrays.
  alignas(SIMD_BYTE_ALIGN) std::array<float, SRSRAN_SIMD_I_SIZE> simd_vector_max_values;
  srsran_simd_i_store(simd_vector_max_indexes.data(), simd_max_indexes);
  srsran_simd_f_store(simd_vector_max_values.data(), simd_max_values);

  // Find maximum value within the vectors.
  float*   it             = std::max_element(simd_vector_max_values.begin(), simd_vector_max_values.end());
  unsigned simd_max_index = static_cast<unsigned>(it - simd_vector_max_values.begin());
  max_index               = simd_vector_max_indexes[simd_max_index];
  max_abs2                = simd_vector_max_values[simd_max_index];
#endif // SRSRAN_SIMD_CF_SIZE

  for (; i != len; ++i) {
    float abs2 = abs_sq(x[i]);
    if (abs2 > max_abs2) {
      max_index = i;
      max_abs2  = abs2;
    }
  }

  return {max_index, max_abs2};
}

std::pair<unsigned, float> srsran::srsvec::max_element(span<const float> x)
{
  unsigned max_index = 0;
  float    max_x     = 0.0F;

  unsigned i = 0, len = x.size();

#if SRSRAN_SIMD_CF_SIZE
  // Prepare range of indexes in SIMD register.
  alignas(SIMD_BYTE_ALIGN) std::array<int32_t, SRSRAN_SIMD_CF_SIZE> simd_vector_max_indexes = {};
  std::iota(simd_vector_max_indexes.begin(), simd_vector_max_indexes.end(), 0);
  simd_i_t simd_indexes = srsran_simd_i_load(simd_vector_max_indexes.data());

  simd_i_t simd_inc         = srsran_simd_i_set1(SRSRAN_SIMD_CF_SIZE);
  simd_i_t simd_max_indexes = srsran_simd_i_set1(0);

  simd_f_t simd_max_values = srsran_simd_f_set1(-INFINITY);

  for (unsigned simd_end = SRSRAN_SIMD_CF_SIZE * (len / SRSRAN_SIMD_CF_SIZE); i != simd_end; i += SRSRAN_SIMD_CF_SIZE) {
    // Load SIMD word of floats.
    simd_f_t simd_x = srsran_simd_f_loadu((float*)&x[i]);

    // Get SIMD selector mask.
    simd_sel_t res = srsran_simd_f_max(simd_x, simd_max_values);

    // Select the indexes and values for the maximum.
    simd_max_indexes = srsran_simd_i_select(simd_max_indexes, simd_indexes, res);
    simd_max_values  = srsran_simd_f_select(simd_max_values, simd_x, res);

    // Increment indexes.
    simd_indexes = srsran_simd_i_add(simd_indexes, simd_inc);
  }

  // Store the indexes and values from SIMD registers into arrays.
  alignas(SIMD_BYTE_ALIGN) std::array<float, SRSRAN_SIMD_I_SIZE> simd_vector_max_values = {};
  srsran_simd_i_store(simd_vector_max_indexes.data(), simd_max_indexes);
  srsran_simd_f_store(simd_vector_max_values.data(), simd_max_values);

  // Find maximum value within the vectors.
  float*   it             = std::max_element(simd_vector_max_values.begin(), simd_vector_max_values.end());
  unsigned simd_max_index = static_cast<unsigned>(it - simd_vector_max_values.begin());
  max_index               = simd_vector_max_indexes[simd_max_index];
  max_x                   = simd_vector_max_values[simd_max_index];
#endif // SRSRAN_SIMD_CF_SIZE

  for (; i != len; ++i) {
    if (x[i] > max_x) {
      max_index = i;
      max_x     = x[i];
    }
  }

  return {max_index, max_x};
}