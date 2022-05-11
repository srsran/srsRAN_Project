/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

static std::mt19937 rgen(0);

using namespace srsgnb;

crc_calculator_checksum_t crc_generic_calculator_byte(span<const uint8_t> data, unsigned poly, unsigned order)
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (unsigned char v : data) {
    for (unsigned bit_idx = 0; bit_idx < 8; bit_idx++) {
      uint64_t bit = (static_cast<uint64_t>(v) >> (7UL - bit_idx)) & 1UL;

      remainder = (remainder << 1U) | bit;

      if ((remainder & highbit) != 0) {
        remainder ^= poly;
      }
    }
  }

  for (unsigned bit_idx = 0; bit_idx < order; bit_idx++) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= poly;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}

crc_calculator_checksum_t crc_generic_calculator_bit(const srsgnb::span<uint8_t>& data, unsigned poly, unsigned order)
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (unsigned char bit : data) {
    remainder = (remainder << 1U) | bit;

    if ((remainder & highbit) != 0) {
      remainder ^= poly;
    }
  }

  for (unsigned bit_idx = 0; bit_idx < order; bit_idx++) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= poly;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}

void test_crc_byte(std::size_t nbytes, crc_generator_poly poly, unsigned polynom, unsigned order)
{
  std::uniform_int_distribution<unsigned char> dist(0, UINT8_MAX);

  // Create data buffer
  std::vector<uint8_t> data(nbytes);

  // Fill buffer with random data
  for (unsigned char& v : data) {
    v = dist(rgen);
  }

  // Calculate checksum in library
  crc_calculator_checksum_t checksum = create_crc_calculator(poly)->calculate_byte(data);

  // Calculate ideal CRC24A
  crc_calculator_checksum_t checksum_gold = crc_generic_calculator_byte(data, polynom, order);

  TESTASSERT_EQ(checksum, checksum_gold, "Byte CRC checksum failed.");
}

void test_crc_bit(std::size_t nbits, crc_generator_poly poly, unsigned polynom, unsigned order)
{
  std::uniform_int_distribution<unsigned char> dist(0, 1);

  // Create data buffer
  std::vector<uint8_t> data(nbits);

  // Fill buffer with random data
  for (unsigned char& v : data) {
    v = dist(rgen);
  }

  // Calculate checksum in library
  crc_calculator_checksum_t checksum = create_crc_calculator(poly)->calculate_bit(data);

  // Calculate ideal CRC24A
  crc_calculator_checksum_t checksum_gold = crc_generic_calculator_bit(data, polynom, order);

  TESTASSERT_EQ(checksum, checksum_gold, "Bit CRC checksum failed.");
}

int main()
{
  std::vector<std::size_t> sizes = {257, 997};

  for (std::size_t N : sizes) {
    test_crc_byte(N, crc_generator_poly::CRC24A, 0x1864CFB, 24);
    test_crc_byte(N, crc_generator_poly::CRC24B, 0X1800063, 24);
    test_crc_byte(N, crc_generator_poly::CRC24C, 0X1B2B117, 24);
    test_crc_byte(N, crc_generator_poly::CRC16, 0x11021, 16);
    test_crc_byte(N, crc_generator_poly::CRC11, 0xE21, 11);

    test_crc_bit(N, crc_generator_poly::CRC24A, 0x1864CFB, 24);
    test_crc_bit(N, crc_generator_poly::CRC24B, 0X1800063, 24);
    test_crc_bit(N, crc_generator_poly::CRC24C, 0X1B2B117, 24);
    test_crc_bit(N, crc_generator_poly::CRC16, 0x11021, 16);
    test_crc_bit(N, crc_generator_poly::CRC11, 0xE21, 11);
    //    test_crc_bit(N, crc_generator_poly::CRC6, 0x61, 6);
  }
}