/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SRSVEC_ADD_H_
#define SRSGNB_SRSVEC_ADD_H_

#include "srsgnb/srsvec/types.h"

namespace srsgnb {
namespace srsvec {

void add(span<const cf_t> x, span<const cf_t> y, span<cf_t> z);
void add(span<const float> x, span<const float> y, span<float> z);
void add(span<const int16_t> x, span<const int16_t> y, span<int16_t> z);
void add(span<const int8_t> x, span<const int8_t> y, span<int8_t> z);

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_ADD_H_
