/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include <numeric>

namespace srsran {
namespace srsvec {

float accumulate(span<const float> x);

inline cf_t accumulate(span<const cf_t> x)
{
  return std::accumulate(x.begin(), x.end(), cf_t());
}

} // namespace srsvec
} // namespace srsran
