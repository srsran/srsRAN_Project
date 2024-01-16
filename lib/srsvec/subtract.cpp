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

#include "srsran/srsvec/subtract.h"
#include "simd.h"

using namespace srsran;
using namespace srsvec;

static void subtract_fff_simd(float* z, const float* x, const float* y, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_F_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_load(&x[i]);
      simd_f_t b = srsran_simd_f_load(&y[i]);

      simd_f_t r = srsran_simd_f_sub(a, b);

      srsran_simd_f_store(&z[i], r);
    }
  } else {
    for (std::size_t i_end = (len / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_loadu(&x[i]);
      simd_f_t b = srsran_simd_f_loadu(&y[i]);

      simd_f_t r = srsran_simd_f_sub(a, b);

      srsran_simd_f_storeu(&z[i], r);
    }
  }
#endif

  for (; i != len; i++) {
    z[i] = x[i] - y[i];
  }
}

static void subtract_sss_simd(int16_t* z, const int16_t* x, const int16_t* y, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_S_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / SRSRAN_SIMD_S_SIZE) * SRSRAN_SIMD_S_SIZE; i != i_end; i += SRSRAN_SIMD_S_SIZE) {
      simd_s_t a = srsran_simd_s_load(&x[i]);
      simd_s_t b = srsran_simd_s_load(&y[i]);

      simd_s_t r = srsran_simd_s_sub(a, b);

      srsran_simd_s_store(&z[i], r);
    }
  } else {
    for (std::size_t i_end = (len / SRSRAN_SIMD_S_SIZE) * SRSRAN_SIMD_S_SIZE; i != i_end; i += SRSRAN_SIMD_S_SIZE) {
      simd_s_t a = srsran_simd_s_loadu(&x[i]);
      simd_s_t b = srsran_simd_s_loadu(&y[i]);

      simd_s_t r = srsran_simd_s_sub(a, b);

      srsran_simd_s_storeu(&z[i], r);
    }
  }
#endif /* SRSRAN_SIMD_S_SIZE */

  for (; i != len; i++) {
    z[i] = x[i] - y[i];
  }
}

static void subtract_bbb_simd(int8_t* z, const int8_t* x, const int8_t* y, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_S_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / SRSRAN_SIMD_B_SIZE) * SRSRAN_SIMD_B_SIZE; i != i_end; i += SRSRAN_SIMD_B_SIZE) {
      simd_b_t a = srsran_simd_b_load(&x[i]);
      simd_b_t b = srsran_simd_b_load(&y[i]);

      simd_b_t r = srsran_simd_b_sub(a, b);

      srsran_simd_b_store(&z[i], r);
    }
  } else {
    for (std::size_t i_end = (len / SRSRAN_SIMD_B_SIZE) * SRSRAN_SIMD_B_SIZE; i != i_end; i += SRSRAN_SIMD_B_SIZE) {
      simd_b_t a = srsran_simd_b_loadu(&x[i]);
      simd_b_t b = srsran_simd_b_loadu(&y[i]);

      simd_b_t r = srsran_simd_b_sub(a, b);

      srsran_simd_b_storeu(&z[i], r);
    }
  }
#endif /* SRSRAN_SIMD_S_SIZE */

  for (; i != len; i++) {
    z[i] = x[i] - y[i];
  }
}

void srsran::srsvec::subtract(span<cf_t> z, span<const cf_t> x, span<const cf_t> y)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  subtract_fff_simd(reinterpret_cast<float*>(z.data()),
                    reinterpret_cast<const float*>(x.data()),
                    reinterpret_cast<const float*>(y.data()),
                    2 * z.size());
}

void srsran::srsvec::subtract(span<float> z, span<const float> x, span<const float> y)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  subtract_fff_simd(z.data(), x.data(), y.data(), z.size());
}

void srsran::srsvec::subtract(span<int16_t> z, span<const int16_t> x, span<const int16_t> y)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  subtract_sss_simd(z.data(), x.data(), y.data(), z.size());
}

void srsran::srsvec::subtract(span<int8_t> z, span<const int8_t> x, span<const int8_t> y)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  subtract_bbb_simd(z.data(), x.data(), y.data(), z.size());
}