/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/srsvec/compare.h"

#include "simd.h"
#include "srsgnb/support/math_utils.h"
#include <numeric>

using namespace srsgnb;
using namespace srsvec;

std::pair<unsigned, float> srsgnb::srsvec::max_abs_element(span<const cf_t> x)
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