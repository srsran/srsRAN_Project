/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/srsvec/clip.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

unsigned srsvec::clip(span<const cf_t> x, float threshold, span<cf_t> y)
{
  srsgnb_srsvec_assert_size(x, y);

  uint  nof_clipped_samples = 0;
  uint  len                 = x.size();
  float x_abs               = 0.0F;

  // Copy the input samples at the output.
  srsvec::copy(y, x);

  // Clip the output signal.
  for (unsigned i = 0; i != len; ++i) {
    x_abs = std::sqrt(abs_sq(x[i]));
    if (x_abs > threshold) {
      y[i] *= threshold / x_abs;
      ++nof_clipped_samples;
    }
  }

  return nof_clipped_samples;
}
