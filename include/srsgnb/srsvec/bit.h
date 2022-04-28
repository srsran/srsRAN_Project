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

/// \brief Unpacks bytes into bits.
/// \param[out] unpacked View of the unpacked bits.
/// \param[in] packed View of the packed bits.
/// \remark The number of unpacked elements must be equal to eight times the number packed elements.
void bit_unpack(span<uint8_t> unpacked, span<const uint8_t> packed);

/// \brief Packs a number of bits into a integer value. Updating bits with the remainder of the input bits.
/// \param[in,out] bits View of unpacked bits.
/// \param[in] nof_bits Indicates the number of bits.
/// \return An integer containing the packed bits.
/// \remark The number of elements in bits must be greater than or equal to the number of bits.
unsigned bit_pack(span<const uint8_t>& bits, unsigned nof_bits);

/// \brief Packs a number of bits into bytes.
/// \param[out] packed View of packed bits.
/// \param[in] unpacked View of unpacked bits.
/// \remark The number of unpacked elements must be equal to eight times the number packed elements.
void bit_pack(span<uint8_t> packed, span<const uint8_t> unpacked);

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_BIT_H_
