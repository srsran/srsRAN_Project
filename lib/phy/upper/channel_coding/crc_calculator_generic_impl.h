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

#include "srsran/phy/upper/channel_coding/crc_calculator.h"

namespace srsran {

/// CRC calculator generic implementation.
class crc_calculator_generic_impl : public crc_calculator
{
private:
  /// Generator polynomial order.
  unsigned order;
  /// String of \c order consecutive ones (used for isolating meaningful bits).
  uint32_t polynom;
  /// Identifier of the cyclic generator polynomial.
  const crc_generator_poly poly;

public:
  /// Initializes the CRC calculator with the provided cyclic generator polynomial.
  explicit crc_calculator_generic_impl(crc_generator_poly poly_);

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
