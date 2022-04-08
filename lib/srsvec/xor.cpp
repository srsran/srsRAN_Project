/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/srsvec/xor.h"

#define HAVE_SSE
#include "simd.h"

using namespace srsgnb;
using namespace srsvec;

void xor_bbb_simd(const uint8_t* x, const uint8_t* y, uint8_t* z, const int len)
{
  int i = 0;
#if SRSRAN_SIMD_B_SIZE
  if (SRSRAN_IS_ALIGNED(x) && SRSRAN_IS_ALIGNED(y) && SRSRAN_IS_ALIGNED(z)) {
    for (; i < len - SRSRAN_SIMD_B_SIZE + 1; i += SRSRAN_SIMD_B_SIZE) {
      simd_b_t a = srsran_simd_b_load((int8_t*)&x[i]);
      simd_b_t b = srsran_simd_b_load((int8_t*)&y[i]);

      simd_b_t r = srsran_simd_b_xor(a, b);

      srsran_simd_b_store((int8_t*)&z[i], r);
    }
  } else {
    for (; i < len - SRSRAN_SIMD_B_SIZE + 1; i += SRSRAN_SIMD_B_SIZE) {
      simd_b_t a = srsran_simd_b_loadu((int8_t*)&x[i]);
      simd_b_t b = srsran_simd_b_loadu((int8_t*)&y[i]);

      simd_b_t r = srsran_simd_b_xor(a, b);

      srsran_simd_b_storeu((int8_t*)&z[i], r);
    }
  }
#endif /* SRSRAN_SIMD_B_SIZE */

  for (; i < len; i++) {
    z[i] = x[i] ^ y[i];
  }
}

void srsgnb::srsvec::xor_vec(srsgnb::span<const uint8_t> x, srsgnb::span<const uint8_t> y, srsgnb::span<uint8_t> z)
{
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

  xor_bbb_simd(x.data(), y.data(), z.data(), z.size());
}