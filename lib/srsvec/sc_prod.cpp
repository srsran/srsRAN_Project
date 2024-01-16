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

#include "srsran/srsvec/sc_prod.h"

#include "simd.h"

using namespace srsran;
using namespace srsvec;

static void sc_prod_fff_simd(const float* x, float h, float* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_F_SIZE
  simd_f_t b = srsran_simd_f_set1(h);
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(z)) {
    for (unsigned i_end = (len / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_load(&x[i]);

      simd_f_t r = srsran_simd_f_mul(a, b);

      srsran_simd_f_store(&z[i], r);
    }
  } else {
    for (unsigned i_end = (len / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_loadu(&x[i]);

      simd_f_t r = srsran_simd_f_mul(a, b);

      srsran_simd_f_storeu(&z[i], r);
    }
  }
#endif

  for (; i != len; i++) {
    z[i] = x[i] * h;
  }
}

static void sc_prod_ccc_simd(const cf_t* x, cf_t h, cf_t* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_CF_SIZE
  simd_cf_t b = srsran_simd_cf_set1(h);
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(z)) {
    for (; i + SRSRAN_SIMD_CF_SIZE < len + 1; i += SRSRAN_SIMD_F_SIZE) {
      simd_cf_t a = srsran_simd_cfi_load(&x[i]);

      simd_cf_t r = srsran_simd_cf_prod(a, b);

      srsran_simd_cfi_store(&z[i], r);
    }
  } else {
    for (; i + SRSRAN_SIMD_CF_SIZE < len + 1; i += SRSRAN_SIMD_F_SIZE) {
      simd_cf_t a = srsran_simd_cfi_loadu(&x[i]);

      simd_cf_t r = srsran_simd_cf_prod(a, b);

      srsran_simd_cfi_storeu(&z[i], r);
    }
  }
#endif

  for (; i != len; i++) {
    z[i] = x[i] * h;
  }
}

void srsran::srsvec::sc_prod(span<const cf_t> x, cf_t h, span<cf_t> z)
{
  srsran_srsvec_assert_size(x, z);

  sc_prod_ccc_simd(x.data(), h, z.data(), x.size());
}

void srsran::srsvec::sc_prod(span<const cf_t> x, float h, span<cf_t> z)
{
  srsran_srsvec_assert_size(x, z);

  sc_prod_fff_simd((float*)x.data(), h, (float*)z.data(), 2 * x.size());
}

void srsran::srsvec::sc_prod(span<const float> x, float h, span<float> z)
{
  srsran_srsvec_assert_size(x, z);

  sc_prod_fff_simd(x.data(), h, z.data(), x.size());
}
