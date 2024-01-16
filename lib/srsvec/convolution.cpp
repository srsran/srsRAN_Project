
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

#include "srsran/srsvec/convolution.h"
#include "simd.h"

using namespace srsran;
using namespace srsvec;

namespace srsran {
namespace srsvec {
namespace detail {

void multiply_and_accumulate(span<float> out, span<const float> x, span<const float> y)
{
  unsigned y_mid     = y.size() / 2;
  unsigned out_start = y_mid - (y.size() % 2 == 0 ? 1 : 0);

  for (unsigned m = 0, m_end = y.size(); m != m_end; ++m) {
    int y_index = y.size() - 1 - m;

    unsigned x_start = m;
    unsigned x_end   = std::min(x.size(), x.size() - y.size() + m + 1);

    span<const float> x_chunk = x.subspan(x_start, x_end - x_start);

    unsigned i = 0;

    for (unsigned i_end = x_chunk.size(); i != i_end; ++i) {
      out[i + out_start] += x_chunk[i] * y[y_index];
    }
  }
}

void multiply_and_accumulate(span<cf_t> out, span<const cf_t> x, span<const float> y)
{
  unsigned y_mid     = y.size() / 2;
  unsigned out_start = (y_mid - (y.size() % 2 == 0 ? 1 : 0)) * 2;

  span<float> out_float(reinterpret_cast<float*>(out.data()), 2 * out.size());

  span<const float> x_float(reinterpret_cast<const float*>(x.data()), 2 * x.size());
  unsigned          i_x = 0;
#if SRSRAN_SIMD_F_SIZE
  unsigned out_end = (x.size() - y.size()) * 2;
  for (unsigned i_x_end = (out_end / SRSRAN_SIMD_F_SIZE) * SRSRAN_SIMD_F_SIZE; i_x != i_x_end;
       i_x += SRSRAN_SIMD_F_SIZE) {
    simd_f_t result = srsran_simd_f_zero();
    auto     y_it   = y.rbegin();
    for (unsigned i_y = 0, i_y_end = y.size(); i_y != i_y_end; ++i_y) {
      simd_f_t y_vector = srsran_simd_f_set1(*y_it++);
      simd_f_t x_vals   = srsran_simd_f_loadu(&x_float[i_x + (i_y * 2)]);
      simd_f_t partial  = srsran_simd_f_mul(x_vals, y_vector);
      result            = srsran_simd_f_add(result, partial);
    }
    srsran_simd_f_storeu(&out_float[out_start + i_x], result);
  }
#endif
  unsigned extra = out_start + i_x;
  for (unsigned i_y = 0, i_y_end = y.size(); i_y != i_y_end; ++i_y) {
    unsigned y_index = y.size() - 1 - i_y;
    unsigned x_start = (extra - out_start) + (i_y * 2);
    unsigned x_end   = (std::min(x.size(), x.size() - y.size() + i_y + 1)) * 2;
    unsigned count   = 0;
    i_x              = x_start;
    for (unsigned i_end = x_end; i_x != i_end; ++i_x) {
      out_float[extra + count] += x_float[i_x] * y[y_index];
      ++count;
    }
  }
}

void multiply_and_accumulate(span<cf_t> out, span<const float> x, span<const cf_t> y)
{
  unsigned y_mid     = y.size() / 2;
  unsigned out_start = y_mid - (y.size() % 2 == 0 ? 1 : 0);

  for (unsigned m = 0, m_end = y.size(); m != m_end; ++m) {
    int y_index = y.size() - 1 - m;

    unsigned x_start = m;
    unsigned x_end   = std::min(x.size(), x.size() - y.size() + m + 1);

    span<const float> x_chunk = x.subspan(x_start, x_end - x_start);

    unsigned i = 0;

    for (unsigned i_end = x_chunk.size(); i != i_end; ++i) {
      out[i + out_start] += x_chunk[i] * y[y_index];
    }
  }
}

} // namespace detail
} // namespace srsvec
} // namespace srsran
