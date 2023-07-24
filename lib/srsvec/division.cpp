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

#include "srsran/srsvec/division.h"
#include "simd.h"

using namespace srsran;

static void divide_fff_simd(float* result, const float* num, const float* den, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_F_SIZE
  for (; i + SRSRAN_SIMD_F_SIZE < len + 1; i += SRSRAN_SIMD_F_SIZE) {
    simd_f_t num_simd = srsran_simd_f_loadu(&num[i]);
    simd_f_t den_simd = srsran_simd_f_loadu(&den[i]);

    simd_f_t r = srsran_simd_f_mul(num_simd, srsran_simd_f_rcp(den_simd));

    srsran_simd_f_storeu(&result[i], r);
  }
#endif

  for (; i != len; ++i) {
    if (!std::isnormal(num[i]) || !std::isnormal(den[i])) {
      result[i] = 0.0F;
      continue;
    }

    result[i] = num[i] / den[i];
  }
}

void srsran::srsvec::divide(span<float> result, span<const float> numerator, span<const float> denominator)
{
  srsran_srsvec_assert_size(result, numerator);
  srsran_srsvec_assert_size(result, denominator);

  divide_fff_simd(result.data(), numerator.data(), denominator.data(), result.size());
}