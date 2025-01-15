/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "crc_calculator_generic_impl.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/math/math_utils.h"

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

crc_calculator_checksum_t srsran::crc_calculator_generic_impl::calculate_byte(span<const uint8_t> input) const
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

crc_calculator_checksum_t crc_calculator_generic_impl::calculate_bit(srsran::span<const uint8_t> input) const
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

crc_calculator_checksum_t crc_calculator_generic_impl::calculate(const bit_buffer& input) const
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
