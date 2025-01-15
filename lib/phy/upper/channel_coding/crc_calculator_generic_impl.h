/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  crc_calculator_checksum_t calculate_byte(span<const uint8_t> input) const override;

  // See interface for the documentation.
  crc_calculator_checksum_t calculate_bit(span<const uint8_t> input) const override;

  // See interface for the documentation.
  crc_calculator_checksum_t calculate(const bit_buffer& data) const override;

  // See interface for the documentation.
  crc_generator_poly get_generator_poly() const override { return poly; }
};

} // namespace srsran
