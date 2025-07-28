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

#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/simd.h"
#include "srsran/support/math/math_utils.h"
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

static void prod_sss_simd(const int16_t* x, const int16_t* y, int16_t* z, std::size_t len)
{
  std::size_t i = 0;

#if SRSRAN_SIMD_S_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (; i + SRSRAN_SIMD_S_SIZE < len + 1; i += SRSRAN_SIMD_S_SIZE) {
      simd_s_t a = srsran_simd_s_load(x + i);
      simd_s_t b = srsran_simd_s_load(y + i);

      simd_s_t r = srsran_simd_s_mul(a, b);

      srsran_simd_s_store(z + i, r);
    }
  } else {
    for (; i + SRSRAN_SIMD_S_SIZE < len + 1; i += SRSRAN_SIMD_S_SIZE) {
      simd_s_t a = srsran_simd_s_loadu(x + i);
      simd_s_t b = srsran_simd_s_loadu(y + i);

      simd_s_t r = srsran_simd_s_mul(a, b);

      srsran_simd_s_storeu(z + i, r);
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

static void prod_cexp_simd(cf_t* out, const cf_t* in, float cfo, float initial_phase, unsigned len)
{
  // Phase increment for each sample.
  cf_t osc = std::polar<float>(1.0F, TWOPI * cfo);
  // Current phase, initially with the initial phase.
  cf_t phase = std::polar<float>(1.0F, initial_phase);
  // Current sample index.
  unsigned i = 0;

#if SRSRAN_SIMD_CF_SIZE
  if (len >= SRSRAN_SIMD_CF_SIZE) {
    // Prepare current phase vector with the initial phases.
    std::array<cf_t, SRSRAN_SIMD_CF_SIZE> temp_phase;
    temp_phase[0] = phase;
    for (int k = 1; k != SRSRAN_SIMD_CF_SIZE; ++k) {
      temp_phase[k] = temp_phase[k - 1] * osc;
    }

    // Load the current phase SIMD register with the initial phases;
    simd_cf_t simd_phase = srsran_simd_cfi_loadu(temp_phase.data());

    // Prepare SIMD oscillator phase shift.
    simd_cf_t simd_osc = srsran_simd_cf_set1(std::polar<float>(1.0F, SRSRAN_SIMD_CF_SIZE * TWOPI * cfo));

    // Process in blocks of the SIMD register size.
    for (unsigned end = (len / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i != end; i += SRSRAN_SIMD_CF_SIZE) {
      // Load input.
      simd_cf_t simd_in = srsran_simd_cfi_loadu(&in[i]);

      // Apply current phase to the input.
      simd_cf_t simd_out = srsran_simd_cf_prod(simd_in, simd_phase);

      // Store result.
      srsran_simd_cfi_storeu(&out[i], simd_out);

      // Increment current phase.
      simd_phase = srsran_simd_cf_prod(simd_phase, simd_osc);
    }

    // Store phase SIMD register and update the current phase with the next phase.
    srsran_simd_cfi_storeu(temp_phase.data(), simd_phase);
    phase = temp_phase.front();
  }
#endif

  for (; i != len; ++i) {
    out[i] = in[i] * phase;
    phase *= osc;
  }
}

void srsran::srsvec::prod(span<cf_t> z, span<const cf_t> x, span<const cf_t> y)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  prod_ccc_simd(x.data(), y.data(), z.data(), x.size());
}

void srsran::srsvec::prod(span<float> z, span<const float> x, span<const float> y)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  prod_fff_simd(x.data(), y.data(), z.data(), x.size());
}

void srsran::srsvec::prod(span<int16_t> z, span<const int16_t> x, span<const int16_t> y)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  prod_sss_simd(x.data(), y.data(), z.data(), x.size());
}

void srsran::srsvec::prod_conj(span<cf_t> z, span<const cf_t> x, span<const cf_t> y)
{
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  prod_conj_ccc_simd(x.data(), y.data(), z.data(), x.size());
}

void srsran::srsvec::prod_cexp(span<cf_t> out, span<const cf_t> in, float norm_cfo, float initial_phase)
{
  srsran_srsvec_assert_size(out, in);

  prod_cexp_simd(out.data(), in.data(), norm_cfo, initial_phase, in.size());
}
