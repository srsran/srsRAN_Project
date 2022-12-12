/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "crc_calculator_lut_impl.h"
#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"

namespace srsgnb {

/// \brief CRC calculator implementation based on the \e PCLMULQDQ instruction.
///
/// Port of the \e soft-crc implementation copyrighted by Intel Corporation and licensed under BSD.
/// Source: https://github.com/intel/soft-crc
///
/// "Fast CRC Computation for Generic Polynomials Using PCLMULQDQ Instruction"
/// URL: http://download.intel.com/design/intarch/papers/323102.pdf
class crc_calculator_clmul_impl : public crc_calculator
{
public:
  /// Default constructor from a polynomial.
  crc_calculator_clmul_impl(crc_generator_poly poly_) : poly(poly_), crc_calc_lut(poly) {}

  // See interface for documentation.
  crc_calculator_checksum_t calculate_byte(span<const uint8_t> data) override;

  // See interface for documentation.
  crc_calculator_checksum_t calculate_bit(span<const uint8_t> data) override;

  // See interface for documentation.
  crc_calculator_checksum_t calculate(const bit_buffer& data) override;

  // See interface for documentation.
  crc_generator_poly get_generator_poly() const override { return poly; };

private:
  /// Polynomial selection.
  crc_generator_poly poly;
  /// CRC calculator based on look-up table for unsupported methods and polynomials.
  crc_calculator_lut_impl crc_calc_lut;
};

} // namespace srsgnb