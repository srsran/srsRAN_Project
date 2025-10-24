/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/srsvec/simd.h"

using namespace srsran;
using namespace srsvec;

static void sc_prod_fff_simd(const float* x, float h, float* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_F_SIZE
  simd_f_t b = srsran_simd_f_set1(h);
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(z)) {
    for (unsigned i_end = (len / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_load(x + i);

      simd_f_t r = srsran_simd_f_mul(a, b);

      srsran_simd_f_store(z + i, r);
    }
  } else {
    for (unsigned i_end = (len / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_loadu(x + i);

      simd_f_t r = srsran_simd_f_mul(a, b);

      srsran_simd_f_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
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
      simd_cf_t a = srsran_simd_cfi_load(x + i);

      simd_cf_t r = srsran_simd_cf_prod(a, b);

      srsran_simd_cfi_store(z + i, r);
    }
  } else {
    for (; i + SRSRAN_SIMD_CF_SIZE < len + 1; i += SRSRAN_SIMD_F_SIZE) {
      simd_cf_t a = srsran_simd_cfi_loadu(x + i);

      simd_cf_t r = srsran_simd_cf_prod(a, b);

      srsran_simd_cfi_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * h;
  }
}

static void sc_prod_ccc_simd(const cbf16_t* x, cf_t h, cbf16_t* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_CF_SIZE
  simd_cf_t b = srsran_simd_cf_set1(h);
  for (unsigned i_end = (len / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i != i_end; i += SRSRAN_SIMD_CF_SIZE) {
    simd_cf_t a = srsran_simd_cbf16_loadu(x + i);

    simd_cf_t r = srsran_simd_cf_prod(a, b);

    srsran_simd_cbf16_storeu(z + i, r);
  }
#endif

  for (; i != len; ++i) {
    z[i] = to_cbf16(to_cf(x[i]) * h);
  }
}

static void sc_prod_sss_simd(const int16_t* x, int16_t h, int16_t* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_S_SIZE
  simd_s_t b = srsran_simd_s_set1(h);
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(z)) {
    for (unsigned i_end = (len / SRSRAN_SIMD_S_SIZE) * SRSRAN_SIMD_S_SIZE; i != i_end; i += SRSRAN_SIMD_S_SIZE) {
      simd_s_t a = srsran_simd_s_load(x + i);

      simd_s_t r = srsran_simd_s_mul(a, b);

      srsran_simd_s_store(z + i, r);
    }
  } else {
    for (unsigned i_end = (len / SRSRAN_SIMD_S_SIZE) * SRSRAN_SIMD_S_SIZE; i != i_end; i += SRSRAN_SIMD_S_SIZE) {
      simd_s_t a = srsran_simd_s_loadu(x + i);

      simd_s_t r = srsran_simd_s_mul(a, b);

      srsran_simd_s_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * h;
  }
}

static void sc_prod_cfc_simd(const cf_t* x, float h, cbf16_t* z, unsigned len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_CF_SIZE

  simd_f_t b = srsran_simd_f_set1(h);

  for (unsigned end = (len / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i != end; i += SRSRAN_SIMD_CF_SIZE) {
    // Load interleaved complex float values into SIMD registers.
    simd_cf_t a = srsran_simd_cfi_loadu(x + i);

    // Multiply with the scaling factor.
    simd_cf_t r = srsran_simd_cf_mul(a, b);

    // Convert complex float to complex brain float and store the result back to memory.
    srsran_simd_cbf16_storeu(z + i, r);
  }
#endif // SRSRAN_SIMD_CF_SIZE

  for (; i != len; ++i) {
    z[i] = to_cbf16(x[i] * h);
  }
}

void srsran::srsvec::sc_prod(span<cf_t> z, span<const cf_t> x, cf_t h)
{
  srsran_srsvec_assert_size(x, z);

  sc_prod_ccc_simd(x.data(), h, z.data(), x.size());
}

void srsran::srsvec::sc_prod(span<cf_t> z, span<const cf_t> x, float h)
{
  srsran_srsvec_assert_size(x, z);

  sc_prod_fff_simd(reinterpret_cast<const float*>(x.data()), h, reinterpret_cast<float*>(z.data()), 2 * x.size());
}

void srsran::srsvec::sc_prod(span<float> z, span<const float> x, float h)
{
  srsran_srsvec_assert_size(x, z);

  sc_prod_fff_simd(x.data(), h, z.data(), x.size());
}

void srsran::srsvec::sc_prod(span<cbf16_t> z, span<const cbf16_t> x, cf_t h)
{
  srsran_srsvec_assert_size(x, z);

  sc_prod_ccc_simd(x.data(), h, z.data(), x.size());
}

void srsran::srsvec::sc_prod(span<int16_t> z, span<const int16_t> x, int16_t h)
{
  srsran_srsvec_assert_size(x, z);

  sc_prod_sss_simd(x.data(), h, z.data(), x.size());
}

void srsran::srsvec::sc_prod(span<cbf16_t> z, span<const cf_t> x, float h)
{
  srsran_srsvec_assert_size(x, z);
  sc_prod_cfc_simd(x.data(), h, z.data(), x.size());
}
