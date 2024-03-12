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

#include "srsran/srsvec/add.h"
#include "simd.h"

using namespace srsran;
using namespace srsvec;

static void add_fff_simd(const float* x, const float* y, float* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_F_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_load(x + i);
      simd_f_t b = srsran_simd_f_load(y + i);

      simd_f_t r = srsran_simd_f_add(a, b);

      srsran_simd_f_store(z + i, r);
    }
  } else {
    for (std::size_t i_end = (len / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_loadu(x + i);
      simd_f_t b = srsran_simd_f_loadu(y + i);

      simd_f_t r = srsran_simd_f_add(a, b);

      srsran_simd_f_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] + y[i];
  }
}

static void add_sss_simd(const int16_t* x, const int16_t* y, int16_t* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_S_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / SRSRAN_SIMD_S_SIZE) * SRSRAN_SIMD_S_SIZE; i != i_end; i += SRSRAN_SIMD_S_SIZE) {
      simd_s_t a = srsran_simd_s_load(x + i);
      simd_s_t b = srsran_simd_s_load(y + i);

      simd_s_t r = srsran_simd_s_add(a, b);

      srsran_simd_s_store(z + i, r);
    }
  } else {
    for (std::size_t i_end = (len / SRSRAN_SIMD_S_SIZE) * SRSRAN_SIMD_S_SIZE; i != i_end; i += SRSRAN_SIMD_S_SIZE) {
      simd_s_t a = srsran_simd_s_loadu(x + i);
      simd_s_t b = srsran_simd_s_loadu(y + i);

      simd_s_t r = srsran_simd_s_add(a, b);

      srsran_simd_s_storeu(z + i, r);
    }
  }
#endif /* SRSRAN_SIMD_S_SIZE */

  for (; i != len; ++i) {
    z[i] = x[i] + y[i];
  }
}

static void add_bbb_simd(const int8_t* x, const int8_t* y, int8_t* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_S_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / SRSRAN_SIMD_B_SIZE) * SRSRAN_SIMD_B_SIZE; i != i_end; i += SRSRAN_SIMD_B_SIZE) {
      simd_b_t a = srsran_simd_b_load(x + i);
      simd_b_t b = srsran_simd_b_load(y + i);

      simd_b_t r = srsran_simd_b_add(a, b);

      srsran_simd_b_store(z + i, r);
    }
  } else {
    for (std::size_t i_end = (len / SRSRAN_SIMD_B_SIZE) * SRSRAN_SIMD_B_SIZE; i != i_end; i += SRSRAN_SIMD_B_SIZE) {
      simd_b_t a = srsran_simd_b_loadu(x + i);
      simd_b_t b = srsran_simd_b_loadu(y + i);

      simd_b_t r = srsran_simd_b_add(a, b);

      srsran_simd_b_storeu(z + i, r);
    }
  }
#endif /* SRSRAN_SIMD_S_SIZE */

  for (; i != len; ++i) {
    z[i] = x[i] + y[i];
  }
}

void srsran::srsvec::add(span<const cf_t> x, span<const cf_t> y, span<cf_t> z)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  add_fff_simd(reinterpret_cast<const float*>(x.data()),
               reinterpret_cast<const float*>(y.data()),
               reinterpret_cast<float*>(z.data()),
               2 * z.size());
}

void srsran::srsvec::add(span<const float> x, span<const float> y, span<float> z)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  add_fff_simd(x.data(), y.data(), z.data(), z.size());
}

void srsran::srsvec::add(span<const int16_t> x, span<const int16_t> y, span<int16_t> z)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  add_sss_simd(x.data(), y.data(), z.data(), z.size());
}

void srsran::srsvec::add(span<const int8_t> x, span<const int8_t> y, span<int8_t> z)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  add_bbb_simd(x.data(), y.data(), z.data(), z.size());
}
