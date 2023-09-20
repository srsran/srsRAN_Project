/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/bit_buffer.h"
#include "srsran/adt/span.h"

namespace srsran {

/// CRC cyclic generator polynomials identifiers.
enum class crc_generator_poly {
  CRC24A,
  CRC24B,
  CRC24C,
  CRC16,
  CRC11,
  CRC6,
  // etc...
};

inline constexpr unsigned get_crc_size(crc_generator_poly poly)
{
  switch (poly) {
    case crc_generator_poly::CRC24A:
    case crc_generator_poly::CRC24B:
    case crc_generator_poly::CRC24C:
      return 24;
    case crc_generator_poly::CRC16:
      return 16;
    case crc_generator_poly::CRC11:
      return 11;
    case crc_generator_poly::CRC6:
    default:
      return 6;
  }
}

/// Checksum type.
using crc_calculator_checksum_t = unsigned;

/// \brief Calculates the CRC, as per TS38.212 Section 5.1. Interface.
class crc_calculator
{
public:
  virtual ~crc_calculator() = default;

  ///\brief Calculates the checksum from a byte buffer (8 packed bits in every input byte).
  ///\param[in] data Provides the byte buffer.
  ///\return The resulting CRC checksum.
  virtual crc_calculator_checksum_t calculate_byte(span<const uint8_t> data) = 0;

  ///\brief Calculates the checksum from a bit buffer (1 bit in for every input byte).
  ///\param[in] data Provides the byte buffer.
  ///\return The resulting CRC checksum.
  virtual crc_calculator_checksum_t calculate_bit(span<const uint8_t> data) = 0;

  ///\brief Calculates the checksum from a bit buffer.
  ///\param[in] data Input data.
  ///\return The resulting CRC checksum.
  virtual crc_calculator_checksum_t calculate(const bit_buffer& data) = 0;

  ///\brief Returns the identifier of the generator polynomial.
  virtual crc_generator_poly get_generator_poly() const = 0;
};

} // namespace srsran
