/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SRSVEC_PROD_H_
#define SRSGNB_SRSVEC_PROD_H_

#include "srsgnb/srsvec/types.h"

namespace srsgnb {
namespace srsvec {

void prod(span<const cf_t> x, span<const cf_t> y, span<cf_t> z);
void prod(span<const cf_t> x, span<const float> y, span<cf_t> z);
void prod(span<const float> x, span<const cf_t> y, span<cf_t> z);
void prod(span<const float> x, span<const float> y, span<float> z);

void prod_conj(span<const cf_t> x, span<const cf_t> y, span<cf_t> z);

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_PROD_H_
