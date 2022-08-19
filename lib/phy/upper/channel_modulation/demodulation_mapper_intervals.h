/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/support/math_utils.h"

namespace srsgnb {

inline unsigned compute_interval_idx(float x, float interval_width, unsigned nof_intervals)
{
  int nof_intervals_int = static_cast<int>(nof_intervals);
  int idx               = static_cast<int>(std::floor(x / interval_width)) + nof_intervals_int / 2;

  return clamp(idx, 0, nof_intervals_int - 1);
}

} // namespace srsgnb