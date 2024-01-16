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

#include "srsran/srsvec/clip.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

unsigned srsvec::clip(span<float> y, span<const float> x, const float threshold)
{
  srsran_srsvec_assert_size(x, y);

  unsigned nof_clipped_samples = 0;
  unsigned len                 = x.size();

  // Clip the output signal.
  for (unsigned i = 0; i != len; ++i) {
    float val = x[i];
    if (val > threshold) {
      val = threshold;
      ++nof_clipped_samples;
    } else if (val < -threshold) {
      val = -threshold;
      ++nof_clipped_samples;
    }
    y[i] = val;
  }

  return nof_clipped_samples;
}

unsigned srsvec::clip_iq(span<cf_t> y, span<const cf_t> x, const float threshold)
{
  span<const float> x_fp = span<const float>(reinterpret_cast<const float*>(x.data()), 2 * x.size());
  span<float>       y_fp = span<float>(reinterpret_cast<float*>(y.data()), 2 * x.size());
  return srsvec::clip(y_fp, x_fp, threshold);
}

unsigned srsvec::clip_magnitude(span<cf_t> y, span<const cf_t> x, const float threshold)
{
  srsran_srsvec_assert_size(x, y);

  unsigned nof_clipped_samples = 0;
  unsigned len                 = x.size();

  // Clip the output signal.
  for (unsigned i = 0; i != len; ++i) {
    cf_t  val   = x[i];
    float x_abs = std::sqrt(abs_sq(val));
    if (x_abs > threshold) {
      val *= threshold / x_abs;
      ++nof_clipped_samples;
    }
    y[i] = val;
  }

  return nof_clipped_samples;
}
