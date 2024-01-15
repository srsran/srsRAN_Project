/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/// \file
/// \brief Bit manipulation function declarations.

#pragma once

#include "srsran/adt/bit_buffer.h"
#include "srsran/adt/span.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include <cstdint>

namespace srsran {
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

/// \brief Unpacks a bit buffer into bits.
/// \param[out] unpacked View of the unpacked bits.
/// \param[in] packed    Bit buffer to unpack.
/// \remark The number of unpacked elements must be equal to the packed number of bits.
void bit_unpack(span<uint8_t> unpacked, const bit_buffer& packed);

/// \brief Unpacks a bit buffer into bits.
/// \param[out] unpacked View of the unpacked bits.
/// \param[in]  packed   Bit buffer to unpack.
/// \param[in]  offset   Packed initial bit index.
void bit_unpack(span<uint8_t> unpacked, const bit_buffer& packed, unsigned offset);

/// \brief Packs a number of bits into an integer value.
/// \param[in,out] bits View of unpacked bits.
/// \param[in] nof_bits Indicates the number of bits.
/// \return An integer containing the packed bits.
/// \remark The number of elements in bits must be greater than or equal to \c nof_bits.
/// \remark \c nof_bits must not exceed 32 bits.
/// \remark After execution, \c bits will contain only the unused bits.
unsigned bit_pack(span<const uint8_t>& bits, unsigned nof_bits);

/// \brief Packs the input bits into an integer value.
///
/// The first bit in the sequence corresponds to the bit of index <tt> bits.size() - 1 </tt> of the returned value. The
/// last value in the sequence corresponds to LSB of the returned value.
///
/// \param[in] bits View of unpacked bits.
/// \return An integer containing the packed bits.
/// \remark The number of elements must not exceed 32 bits.
unsigned bit_pack(span<const uint8_t> bits);

/// \brief Packs a number of bits into a bit buffer.
/// \param[out] packed   Destination bit buffer.
/// \param[in]  unpacked View of unpacked bits.
/// \remark The number of unpacked elements must be equal to the maximum number of bits supported by the bit buffer.
void bit_pack(bit_buffer& packed, span<const uint8_t> unpacked);

/// \brief Packs a number of bits into a bit buffer.
/// \param[out] packed   Destination bit buffer.
/// \param[in]  offset   Packed initial bit index.
/// \param[in]  unpacked View of unpacked bits.
/// \remark The number of unpacked elements must be smaller than or equal to the maximum number of bits supported by the
/// bit buffer minus \c offset.
void bit_pack(bit_buffer& packed, unsigned offset, span<const uint8_t> unpacked);

/// \brief Copies \c output.size() bits from \c input, starting at \c startpos, into \c output.
/// \param[out] output   Destination of the copy.
/// \param[in]  input    Data source to copy.
/// \param[in]  startpos Bit position index to start the copy.
void copy_offset(bit_buffer& output, span<const uint8_t> input, unsigned startpos);

/// \brief Copies bits from one buffer to another.
///
/// Copies \c nof_bits bits from \c input, starting at \c in_offset, into \c output, starting at \c out_offset.
/// \param[out] output   Destination of the copy.
/// \param[in]  out_offset Output bit position index to start the copy.
/// \param[in]  input    Data source to copy.
/// \param[in]  in_offset Input bit position index to start the copy.
/// \param[in]  nof_bits Number of bits to copy.
void copy_offset(bit_buffer&       output,
                 unsigned          out_offset,
                 const bit_buffer& input,
                 unsigned          in_offset,
                 unsigned          nof_bits);

} // namespace srsvec
} // namespace srsran
