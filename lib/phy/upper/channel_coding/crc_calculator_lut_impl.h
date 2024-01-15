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

#pragma once

#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include <array>
#include <cstdint>
#include <map>

namespace srsran {

/// CRC calculator based on look-up tables.
class crc_calculator_lut_impl : public crc_calculator
{
private:
  /// Look up tables for the CRC calculator.
  struct crc_table_s : public std::array<uint32_t, 256> {
    /// Order of the cyclic generator polynomial.
    unsigned order;

    /// String of \c order consecutive ones (used for isolating meaningful bits).
    uint32_t crcmask;

    /// \brief Bitmask describing the generator polynomial.
    ///
    /// Example: <tt>polynom = 0x61</tt> corresponds to the polynomial \f$D^6 + D^5 + 1\f$.
    uint32_t polynom;

    /// Constructs the CRC table for the given order and generator polynomial.
    crc_table_s(unsigned poly, unsigned order_);
  };

  /// Map of CRC tables for the different polynomials.
  static const std::map<crc_generator_poly, crc_table_s> crc_tables;

  /// Current CRC table.
  const crc_table_s& table;
  /// Generator polynomial order.
  const unsigned order;
  /// String of \c order consecutive ones (used for isolating meaningful bits).
  const uint32_t crcmask;
  /// Computed checksum.
  uint32_t crc;
  /// Identifier of the cyclic generator polynomial.
  const crc_generator_poly poly;

public:
  /// Initializes the CRC calculator with the provided cyclic generator polynomial.
  explicit crc_calculator_lut_impl(crc_generator_poly poly_);

  /// Sets the internal CRC.
  void reset(uint32_t crc_ = 0) { crc = crc_; }

  /// Includes the given \c byte into the CRC computation.
  void put_byte(uint8_t byte)
  {
    unsigned idx;
    if (order > 8) {
      // For more than 8 bits
      unsigned ord = order - 8U;
      idx          = ((crc >> (ord)) & 0xffU) ^ byte;
    } else {
      // For 8 bits or less
      unsigned ord = 8U - order;
      idx          = ((crc << (ord)) & 0xffU) ^ byte;
    }

    crc = (crc << 8U) ^ table[idx];
  }

  /// Reverses the \c nbits least significant bits of \c crc.
  void reversecrcbit(unsigned nbits)
  {
    uint64_t m, rmask = 0x1;

    for (m = 0; m < nbits; m++) {
      if ((rmask & crc) == 0x01)
        crc = (crc ^ table.polynom) >> 1;
      else
        crc = crc >> 1;
    }
    crc = (crc & table.crcmask);
  }

  /// Reads the current value of \c crc.
  crc_calculator_checksum_t get_checksum() const { return static_cast<crc_calculator_checksum_t>(crc & crcmask); }

  // See interface for the documentation.
  crc_calculator_checksum_t calculate_byte(span<const uint8_t> input) override;

  // See interface for the documentation.
  crc_calculator_checksum_t calculate_bit(span<const uint8_t> input) override;

  // See interface for the documentation.
  crc_calculator_checksum_t calculate(const bit_buffer& data) override;

  // See interface for the documentation.
  crc_generator_poly get_generator_poly() const override { return poly; }
};

} // namespace srsran
