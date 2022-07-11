/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SRSVEC_BIT_H
#define SRSGNB_SRSVEC_BIT_H

#include "srsgnb/adt/span.h"
#include <cstdint>

namespace srsgnb {
namespace srsvec {

/// \brief Unpacks an unsigned integer value into bits.
///
/// The first bit in the sequence corresponds to the bit of index <tt> nof_bits - 1 </tt> of \c value. The LSB of \c
/// value is mapped to the last element in the sequence.
///
/// \param[out] bits Unpacked bits sequence.
/// \param[in] value Provides the value to unpack.
/// \param[in] nof_bits Indicates the number of bits to unpack.
/// \return A view of the remaining (behind \c nof_bits) unpacked bits of \c value.
span<uint8_t> bit_unpack(span<uint8_t> bits, unsigned value, unsigned nof_bits);

/// \brief Unpacks bytes into bits.
/// \param[out] unpacked View of the unpacked bits.
/// \param[in] packed View of the packed bits.
/// \remark The number of unpacked elements must be equal to eight times the number of packed elements.
void bit_unpack(span<uint8_t> unpacked, span<const uint8_t> packed);

/// \brief Packs a number of bits into an integer value.
/// \param[in,out] bits View of unpacked bits.
/// \param[in] nof_bits Indicates the number of bits.
/// \return An integer containing the packed bits.
/// \remark The number of elements in bits must be greater than or equal to \c nof_bits.
/// \remark \c nof_bits must not exceed 32 bits.
/// \remark After execution, \c bits will contain only the unused bits.
unsigned bit_pack(span<const uint8_t>& bits, unsigned nof_bits);

/// \brief Packs a number of bits into bytes.
/// \param[out] packed View of packed bits.
/// \param[in] unpacked View of unpacked bits.
/// \remark The number of unpacked elements must be equal to eight times the number packed elements.
void bit_pack(span<uint8_t> packed, span<const uint8_t> unpacked);

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_BIT_H
