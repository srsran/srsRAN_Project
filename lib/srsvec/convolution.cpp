
/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srsvec/convolution.h"
#include "simd.h"
using namespace srsran;
using namespace srsvec;

namespace srsran {
namespace srsvec {
namespace detail {

void multiplicate_and_accumulate(span<cf_t> out, span<const cf_t> x, cf_t y)
{
  unsigned  i        = 0;
  simd_cf_t y_vector = srsran_simd_cf_set1(y);
  simd_cf_t x_vector, out_vector, result_vector;

  for (unsigned i_end = (x.size() / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
    x_vector      = srsran_simd_cfi_loadu(&x[i]);
    out_vector    = srsran_simd_cfi_loadu(&out[i]);
    result_vector = srsran_simd_cf_prod(x_vector, y_vector);
    result_vector = srsran_simd_cf_add(out_vector, result_vector);
    srsran_simd_cfi_storeu(&out[i], result_vector);
  }

  for (unsigned i_end = x.size(); i != i_end; ++i) {
    out[i] += x[i] * y;
  }
}

void multiplicate_and_accumulate(span<float> out, span<const float> x, float y)
{
  unsigned i        = 0;
  simd_f_t y_vector = srsran_simd_f_set1(y);
  simd_f_t x_vector, out_vector, result_vector;

  for (unsigned i_end = (x.size() / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
    x_vector      = srsran_simd_f_loadu(&x[i]);
    out_vector    = srsran_simd_f_loadu(&out[i]);
    result_vector = srsran_simd_f_fma(out_vector, x_vector, y_vector);
    srsran_simd_f_storeu(&out[i], result_vector);
  }

  for (unsigned i_end = x.size(); i != i_end; ++i) {
    out[i] += x[i] * y;
  }
}

void multiplicate_and_accumulate(span<cf_t> out, span<const cf_t> x, float y)
{
  simd_f_t y_simd = srsran_simd_f_set1(y);

  span<const float> x_float(reinterpret_cast<const float*>(x.data()), 2 * x.size());
  span<float>       out_float(reinterpret_cast<float*>(out.data()), 2 * out.size());

  unsigned i = 0;
  for (unsigned i_end = (x.size() / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i != i_end; i += SRSRAN_SIMD_F_SIZE) {
    simd_f_t x_vals   = srsran_simd_f_loadu(&x_float[i]);
    simd_f_t result   = srsran_simd_f_mul(x_vals, y_simd);
    simd_f_t out_vals = srsran_simd_f_loadu(&out_float[i]);
    out_vals          = srsran_simd_f_add(out_vals, result);
    srsran_simd_f_storeu(&out_float[i], out_vals);
  }
  for (unsigned i_end = x_float.size(); i != i_end; ++i) {
    out_float[i] += x_float[i] * y;
  }
}

void multiplicate_and_accumulate(span<cf_t> out, span<const float> x, cf_t y)
{
  unsigned i = 0;
  for (unsigned i_end = x.size(); i != i_end; ++i) {
    out[i] += x[i] * y;
  }
}

} // namespace detail
} // namespace srsvec
} // namespace srsran
