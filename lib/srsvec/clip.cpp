/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
