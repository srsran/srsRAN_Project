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

#include "srsgnb/adt/span.h"
#include <cstdint>

namespace srsgnb {

namespace srsvec {

/// \brief Unpacks an unsigned integer value into bits. The MSB is mapped to the first bit.
/// \param[out] bits Unpacked bits.
/// \param[in] value Provides the value to unpack.
/// \param[in] nof_bits Indicates the number of bits to unpack.
/// \return A view of the remainder of unpacked bits.
span<uint8_t> bit_unpack(span<uint8_t> bits, unsigned value, unsigned nof_bits);

void bit_unpack(span<const uint8_t> packed, span<uint8_t> unpacked);

unsigned bit_pack(span<const uint8_t>& bits, unsigned nof_bits);

void bit_pack(span<const uint8_t> unpacked, span<uint8_t> packed);

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_BIT_H_
