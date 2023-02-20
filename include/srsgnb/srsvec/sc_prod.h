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

namespace srsgnb {
namespace srsvec {

void sc_prod(span<const cf_t> x, cf_t h, span<cf_t> z);
void sc_prod(span<const cf_t> x, float h, span<cf_t> z);
void sc_prod(span<const float> x, float h, span<float> z);

} // namespace srsvec
} // namespace srsgnb
