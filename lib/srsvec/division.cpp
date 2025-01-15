/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srsvec/division.h"
#include "srsran/srsvec/simd.h"

using namespace srsran;
using namespace srsvec;

static void divide_fff_simd(float* result, const float* num, const float* den, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_F_SIZE
  for (; i + SRSRAN_SIMD_F_SIZE < len + 1; i += SRSRAN_SIMD_F_SIZE) {
    simd_f_t num_simd = srsran_simd_f_loadu(num + i);
    simd_f_t den_simd = srsran_simd_f_loadu(den + i);

    simd_f_t r = srsran_simd_f_mul(num_simd, srsran_simd_f_rcp(den_simd));

    srsran_simd_f_storeu(result + i, r);
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
