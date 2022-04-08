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

#ifndef SRSGNB_SRSVEC_XOR_H
#define SRSGNB_SRSVEC_XOR_H

#include "srsgnb/srsvec/types.h"

namespace srsgnb {
namespace srsvec {

void xor_vec(span<const uint8_t> x, span<const uint8_t> y, span<uint8_t> z);

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_XOR_H
