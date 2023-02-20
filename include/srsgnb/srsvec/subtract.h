/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/srsvec/types.h"

namespace srsran {
namespace srsvec {

void subtract(span<cf_t> z, span<const cf_t> x, span<const cf_t> y);
void subtract(span<float> z, span<const float> x, span<const float> y);
void subtract(span<int16_t> z, span<const int16_t> x, span<const int16_t> y);
void subtract(span<int8_t> z, span<const int8_t> x, span<const int8_t> y);

} // namespace srsvec
} // namespace srsran
