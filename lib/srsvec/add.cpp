/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/srsvec/add.h"

#include "simd.h"

using namespace srsgnb;
using namespace srsvec;

static void add_fff_simd(const float* x, const float* y, float* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_F_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (; i + SRSRAN_SIMD_F_SIZE < len + 1; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_load(&x[i]);
      simd_f_t b = srsran_simd_f_load(&y[i]);

      simd_f_t r = srsran_simd_f_add(a, b);

      srsran_simd_f_store(&z[i], r);
    }
  } else {
    for (; i + SRSRAN_SIMD_F_SIZE < len + 1; i += SRSRAN_SIMD_F_SIZE) {
      simd_f_t a = srsran_simd_f_loadu(&x[i]);
      simd_f_t b = srsran_simd_f_loadu(&y[i]);

      simd_f_t r = srsran_simd_f_add(a, b);

      srsran_simd_f_storeu(&z[i], r);
    }
  }
#endif

  for (; i != len; i++) {
    z[i] = x[i] + y[i];
  }
}

static void add_sss_simd(const int16_t* x, const int16_t* y, int16_t* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_S_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (; i + SRSRAN_SIMD_S_SIZE < len + 1; i += SRSRAN_SIMD_S_SIZE) {
      simd_s_t a = srsran_simd_s_load(&x[i]);
      simd_s_t b = srsran_simd_s_load(&y[i]);

      simd_s_t r = srsran_simd_s_add(a, b);

      srsran_simd_s_store(&z[i], r);
    }
  } else {
    for (; i + SRSRAN_SIMD_S_SIZE < len + 1; i += SRSRAN_SIMD_S_SIZE) {
      simd_s_t a = srsran_simd_s_loadu(&x[i]);
      simd_s_t b = srsran_simd_s_loadu(&y[i]);

      simd_s_t r = srsran_simd_s_add(a, b);

      srsran_simd_s_storeu(&z[i], r);
    }
  }
#endif /* SRSRAN_SIMD_S_SIZE */

  for (; i != len; i++) {
    z[i] = x[i] + y[i];
  }
}

static void add_bbb_simd(const int8_t* x, const int8_t* y, int8_t* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_S_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (; i + SRSRAN_SIMD_B_SIZE < len + 1; i += SRSRAN_SIMD_B_SIZE) {
      simd_b_t a = srsran_simd_b_load(&x[i]);
      simd_b_t b = srsran_simd_b_load(&y[i]);

      simd_b_t r = srsran_simd_b_add(a, b);

      srsran_simd_b_store(&z[i], r);
    }
  } else {
    for (; i + SRSRAN_SIMD_B_SIZE < len + 1; i += SRSRAN_SIMD_B_SIZE) {
      simd_b_t a = srsran_simd_b_loadu(&x[i]);
      simd_b_t b = srsran_simd_b_loadu(&y[i]);

      simd_b_t r = srsran_simd_b_add(a, b);

      srsran_simd_b_storeu(&z[i], r);
    }
  }
#endif /* SRSRAN_SIMD_S_SIZE */

  for (; i != len; i++) {
    z[i] = x[i] + y[i];
  }
}

void srsgnb::srsvec::add(span<const cf_t> x, span<const cf_t> y, span<cf_t> z)
{
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

  add_fff_simd((float*)x.data(), (float*)y.data(), (float*)z.data(), 2 * z.size());
}

void srsgnb::srsvec::add(span<const float> x, span<const float> y, span<float> z)
{
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

  add_fff_simd(x.data(), y.data(), z.data(), z.size());
}

void srsgnb::srsvec::add(span<const int16_t> x, span<const int16_t> y, span<int16_t> z)
{
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

  add_sss_simd(x.data(), y.data(), z.data(), z.size());
}

void srsgnb::srsvec::add(span<const int8_t> x, span<const int8_t> y, span<int8_t> z)
{
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

  add_bbb_simd(x.data(), y.data(), z.data(), z.size());
}