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

#include "srsran/srsvec/prod.h"
#include "simd.h"
#include <cmath>

using namespace srsran;
using namespace srsvec;

static void prod_fff_simd(const float* x, const float* y, float* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_F_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (; i + SRSRAN_SIMD_F_SIZE < len + 1; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_load(x + i);
      simd_f_t b = srsran_simd_f_load(y + i);

      simd_f_t r = srsran_simd_f_mul(a, b);

      srsran_simd_f_store(z + i, r);
    }
  } else {
    for (; i + SRSRAN_SIMD_F_SIZE < len + 1; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_loadu(x + i);
      simd_f_t b = srsran_simd_f_loadu(y + i);

      simd_f_t r = srsran_simd_f_mul(a, b);

      srsran_simd_f_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * y[i];
  }
}

static void prod_ccc_simd(const cf_t* x, const cf_t* y, cf_t* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_CF_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i != i_end; i += SRSRAN_SIMD_CF_SIZE) {
      simd_cf_t a = srsran_simd_cfi_load(x + i);
      simd_cf_t b = srsran_simd_cfi_load(y + i);

      simd_cf_t r = srsran_simd_cf_prod(a, b);

      srsran_simd_cfi_store(z + i, r);
    }
  } else {
    for (std::size_t i_end = (len / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i != i_end; i += SRSRAN_SIMD_CF_SIZE) {
      simd_cf_t a = srsran_simd_cfi_loadu(x + i);
      simd_cf_t b = srsran_simd_cfi_loadu(y + i);

      simd_cf_t r = srsran_simd_cf_prod(a, b);

      srsran_simd_cfi_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * y[i];
  }
}

static void prod_conj_ccc_simd(const cf_t* x, const cf_t* y, cf_t* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_CF_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i != i_end; i += SRSRAN_SIMD_CF_SIZE) {
      simd_cf_t a = srsran_simd_cfi_load(x + i);
      simd_cf_t b = srsran_simd_cfi_load(y + i);

      simd_cf_t r = srsran_simd_cf_conjprod(a, b);

      srsran_simd_cfi_store(z + i, r);
    }
  } else {
    for (std::size_t i_end = (len / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i != i_end; i += SRSRAN_SIMD_CF_SIZE) {
      simd_cf_t a = srsran_simd_cfi_loadu(x + i);
      simd_cf_t b = srsran_simd_cfi_loadu(y + i);

      simd_cf_t r = srsran_simd_cf_conjprod(a, b);

      srsran_simd_cfi_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * std::conj(y[i]);
  }
}

void srsran::srsvec::prod(span<const cf_t> x, span<const cf_t> y, span<cf_t> z)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  prod_ccc_simd(x.data(), y.data(), z.data(), x.size());
}

void srsran::srsvec::prod(span<const float> x, span<const float> y, span<float> z)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  prod_fff_simd(x.data(), y.data(), z.data(), x.size());
}

void srsran::srsvec::prod_conj(span<const cf_t> x, span<const cf_t> y, span<cf_t> z)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  prod_conj_ccc_simd(x.data(), y.data(), z.data(), x.size());
}
