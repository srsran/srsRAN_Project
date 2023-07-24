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
#include "srsran/srsvec/accumulate.h"
#include "simd.h"

using namespace srsran;
using namespace srsvec;

static float accumulate_f_simd(const float* x, unsigned len)
{
  float result = 0;

  unsigned i = 0;
#if SRSRAN_SIMD_F_SIZE
  if (len >= SRSRAN_SIMD_F_SIZE) {
    simd_f_t simd_result = srsran_simd_f_loadu(&x[i]);
    i += SRSRAN_SIMD_F_SIZE;

    for (unsigned simd_end = SRSRAN_SIMD_F_SIZE * (len / SRSRAN_SIMD_F_SIZE); i != simd_end; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t simd_x = srsran_simd_f_loadu(&x[i]);

      simd_result = srsran_simd_f_add(simd_x, simd_result);
    }

    alignas(SIMD_BYTE_ALIGN) std::array<float, SRSRAN_SIMD_F_SIZE> simd_vector_sum;
    srsran_simd_f_store(simd_vector_sum.data(), simd_result);
    result = std::accumulate(simd_vector_sum.begin(), simd_vector_sum.end(), result);
  }
#endif // SRSRAN_SIMD_F_SIZE

  for (; i != len; ++i) {
    result += x[i];
  }

  return result;
}

float srsran::srsvec::accumulate(span<const float> x)
{
  return accumulate_f_simd(x.data(), x.size());
}