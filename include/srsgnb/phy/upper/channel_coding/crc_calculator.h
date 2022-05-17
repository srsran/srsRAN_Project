/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_CRC_CALCULATOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_CRC_CALCULATOR_H

#include "srsgnb/adt/span.h"
#include <memory>

namespace srsgnb {

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

  ///\brief Returns the identifier of the generator polynomial.
  virtual crc_generator_poly get_generator_poly() const = 0;
};

std::unique_ptr<crc_calculator> create_crc_calculator(crc_generator_poly poly);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_CRC_CALCULATOR_H
