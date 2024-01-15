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

#include "crc_calculator_generic_impl.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

crc_calculator_generic_impl::crc_calculator_generic_impl(crc_generator_poly poly_) : poly(poly_)
{
  switch (poly) {
    case crc_generator_poly::CRC24A:
      order   = 24;
      polynom = 0x1864cfb;
      break;
    case crc_generator_poly::CRC24B:
      order   = 24;
      polynom = 0X1800063;
      break;
    case crc_generator_poly::CRC24C:
      order   = 24;
      polynom = 0X1B2B117;
      break;
    case crc_generator_poly::CRC16:
      order   = 16;
      polynom = 0x11021;
      break;
    case crc_generator_poly::CRC11:
      order   = 11;
      polynom = 0xe21;
      break;
    case crc_generator_poly::CRC6:
      order   = 6;
      polynom = 0x61;
      break;
  }
}

crc_calculator_checksum_t srsran::crc_calculator_generic_impl::calculate_byte(span<const uint8_t> input)
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (uint8_t v : input) {
    for (unsigned bit_idx = 0; bit_idx != 8; ++bit_idx) {
      uint64_t bit = (static_cast<uint64_t>(v) >> (7UL - bit_idx)) & 1UL;

      remainder = (remainder << 1U) | bit;

      if ((remainder & highbit) != 0) {
        remainder ^= polynom;
      }
    }
  }

  for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= polynom;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}

crc_calculator_checksum_t crc_calculator_generic_impl::calculate_bit(srsran::span<const uint8_t> input)
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (uint8_t bit : input) {
    remainder = (remainder << 1U) | bit;

    if ((remainder & highbit) != 0) {
      remainder ^= polynom;
    }
  }

  for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= polynom;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}

crc_calculator_checksum_t crc_calculator_generic_impl::calculate(const bit_buffer& input)
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (unsigned i_bit = 0, i_bit_end = input.size(); i_bit != i_bit_end; ++i_bit) {
    remainder = (remainder << 1U) | input.extract<uint64_t>(i_bit, 1);

    if ((remainder & highbit) != 0) {
      remainder ^= polynom;
    }
  }

  for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= polynom;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}
