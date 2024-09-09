/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "srsran/srsvec/accumulate.h"
#include "srsran/srsvec/simd.h"
#include "srsran/support/srsran_assert.h"
#include <numeric>

using namespace srsran;
using namespace srsvec;

static float accumulate_f_simd(const float* x, unsigned len)
{
  float    result = 0;
  unsigned i      = 0;

#if SRSRAN_SIMD_F_SIZE
  if (len >= SRSRAN_SIMD_F_SIZE) {
    simd_f_t simd_result = srsran_simd_f_loadu(x + i);
    i += SRSRAN_SIMD_F_SIZE;

    for (unsigned simd_end = SRSRAN_SIMD_F_SIZE * (len / SRSRAN_SIMD_F_SIZE); i != simd_end; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t simd_x = srsran_simd_f_loadu(x + i);

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
