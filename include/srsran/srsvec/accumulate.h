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
#include "srsran/srsvec/detail/traits.h"
#include "srsran/support/srsran_assert.h"
#include <numeric>

namespace srsran {
namespace srsvec {

float accumulate(span<const float> x);

} // namespace srsvec
} // namespace srsran
