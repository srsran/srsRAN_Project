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

#include "crc_calculator_lut_impl.h"
#include "srsran/phy/upper/channel_coding/crc_calculator.h"

namespace srsran {

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

} // namespace srsran