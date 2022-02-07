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

#ifndef SRSGNB_SRSVEC_BIT_H_
#define SRSGNB_SRSVEC_BIT_H_

#include "srsgnb/adt/bit_buffer.h"
#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

namespace srsvec {

void bit_unpack(unsigned value, bit_buffer& bits, unsigned nof_bits);
void bit_unpack(byte_buffer& packed, bit_buffer& unpacked);

unsigned bit_pack(bit_buffer& bits, unsigned nof_bits);
void     bit_pack(bit_buffer& unpacked, byte_buffer& packed);

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_BIT_H_
