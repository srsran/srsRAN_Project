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

#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/support/srsran_test.h"
#include <getopt.h>
#include <random>

static std::mt19937 rgen(0);

using namespace srsran;

crc_calculator_checksum_t crc_generic_calculator_byte(span<const uint8_t> data, unsigned poly, unsigned order)
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (uint8_t v : data) {
    for (unsigned bit_idx = 0; bit_idx != 8; ++bit_idx) {
      uint64_t bit = (static_cast<uint64_t>(v) >> (7UL - bit_idx)) & 1UL;

      remainder = (remainder << 1U) | bit;

      if ((remainder & highbit) != 0) {
        remainder ^= poly;
      }
    }
  }

  for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= poly;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}

crc_calculator_checksum_t crc_generic_calculator_bit(const srsran::span<uint8_t>& data, unsigned poly, unsigned order)
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (uint8_t bit : data) {
    remainder = (remainder << 1U) | bit;

    if ((remainder & highbit) != 0) {
      remainder ^= poly;
    }
  }

  for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= poly;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}

crc_calculator_checksum_t crc_generic_calculator(const bit_buffer& data, unsigned poly, unsigned order)
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (unsigned i_bit = 0, i_bit_end = data.size(); i_bit != i_bit_end; ++i_bit) {
    remainder = (remainder << 1U) | data.extract<uint64_t>(i_bit, 1);

    if ((remainder & highbit) != 0) {
      remainder ^= poly;
    }
  }

  for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= poly;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}

void test_crc_byte(crc_calculator_factory& factory,
                   std::size_t             nbytes,
                   crc_generator_poly      poly,
                   unsigned                polynom,
                   unsigned                order)
{
  std::uniform_int_distribution<uint8_t> dist(0, UINT8_MAX);

  // Create data buffer.
  std::vector<uint8_t> data(nbytes);

  // Fill buffer with random data.
  for (uint8_t& v : data) {
    v = static_cast<uint8_t>(dist(rgen) & 0xff);
  }

  std::unique_ptr<crc_calculator> crc_calculator = factory.create(poly);
  TESTASSERT(crc_calculator);

  // Calculate checksum in library.
  crc_calculator_checksum_t checksum = crc_calculator->calculate_byte(data);

  // Calculate ideal CRC24A.
  crc_calculator_checksum_t checksum_gold = crc_generic_calculator_byte(data, polynom, order);

  TESTASSERT_EQ(checksum, checksum_gold, "Byte CRC checksum failed {:06X}!={:06X}.", checksum, checksum_gold);
}

void test_crc_bit(crc_calculator_factory& factory,
                  std::size_t             nbits,
                  crc_generator_poly      poly,
                  unsigned                polynom,
                  unsigned                order)
{
  std::uniform_int_distribution<uint8_t> dist(0, 1);

  // Create data buffer.
  std::vector<uint8_t> data(nbits);

  // Fill buffer with random data.
  for (uint8_t& v : data) {
    v = dist(rgen);
  }

  std::unique_ptr<crc_calculator> crc_calculator = factory.create(poly);
  TESTASSERT(crc_calculator);

  // Calculate checksum in library.
  crc_calculator_checksum_t checksum = crc_calculator->calculate_bit(data);

  // Calculate ideal CRC24A.
  crc_calculator_checksum_t checksum_gold = crc_generic_calculator_bit(data, polynom, order);

  TESTASSERT_EQ(checksum, checksum_gold, "Bit CRC checksum failed.");
}

void test_crc_bit_buffer(crc_calculator_factory& factory,
                         std::size_t             nbits,
                         crc_generator_poly      poly,
                         unsigned                polynom,
                         unsigned                order)
{
  // Create data buffer.
  dynamic_bit_buffer data(nbits);

  // Fill buffer with random data.
  for (unsigned i_byte = 0, i_byte_end = nbits / 8; i_byte != i_byte_end; ++i_byte) {
    data.set_byte(rgen() & mask_lsb_ones<unsigned>(8), i_byte);
  }
  if (nbits % 8 != 0) {
    data.insert((rgen() & mask_lsb_ones<unsigned>(nbits % 8)), 8 * (nbits / 8), nbits % 8);
  }

  std::unique_ptr<crc_calculator> crc_calculator = factory.create(poly);
  TESTASSERT(crc_calculator);

  // Calculate checksum in library.
  crc_calculator_checksum_t checksum = crc_calculator->calculate(data);

  // Calculate ideal CRC24A.
  crc_calculator_checksum_t checksum_gold = crc_generic_calculator(data, polynom, order);

  TESTASSERT_EQ(checksum, checksum_gold, "Bit buffer CRC checksum failed {:06X}!={:06X}.", checksum, checksum_gold);
}

static std::string preferred_type = "auto";

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-F preferred algo] [-R repetitions]\n", prog);
  fmt::print("\t-F Select CRC calculator preferred algorithm [Default {}]\n", preferred_type);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "F:R:sh")) != -1) {
    switch (opt) {
      case 'F':
        preferred_type = std::string(optarg);
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  std::shared_ptr<crc_calculator_factory> factory = create_crc_calculator_factory_sw(preferred_type);
  TESTASSERT(factory);

  std::vector<std::size_t> sizes = {8, 16, 32, 257, 997, 6012};

  for (std::size_t N : sizes) {
    test_crc_byte(*factory, N, crc_generator_poly::CRC24A, 0x1864cfb, 24);
    test_crc_byte(*factory, N, crc_generator_poly::CRC24B, 0X1800063, 24);
    test_crc_byte(*factory, N, crc_generator_poly::CRC24C, 0X1B2B117, 24);
    test_crc_byte(*factory, N, crc_generator_poly::CRC16, 0x11021, 16);
    test_crc_byte(*factory, N, crc_generator_poly::CRC11, 0xe21, 11);
    test_crc_byte(*factory, N, crc_generator_poly::CRC6, 0x61, 6);

    test_crc_bit(*factory, N, crc_generator_poly::CRC24A, 0x1864cfb, 24);
    test_crc_bit(*factory, N, crc_generator_poly::CRC24B, 0X1800063, 24);
    test_crc_bit(*factory, N, crc_generator_poly::CRC24C, 0X1B2B117, 24);
    test_crc_bit(*factory, N, crc_generator_poly::CRC16, 0x11021, 16);
    test_crc_bit(*factory, N, crc_generator_poly::CRC11, 0xe21, 11);
    test_crc_bit(*factory, N, crc_generator_poly::CRC6, 0x61, 6);

    test_crc_bit_buffer(*factory, N, crc_generator_poly::CRC24A, 0x1864cfb, 24);
    test_crc_bit_buffer(*factory, N, crc_generator_poly::CRC24B, 0X1800063, 24);
    test_crc_bit_buffer(*factory, N, crc_generator_poly::CRC24C, 0X1B2B117, 24);
    test_crc_bit_buffer(*factory, N, crc_generator_poly::CRC16, 0x11021, 16);
    test_crc_bit_buffer(*factory, N, crc_generator_poly::CRC11, 0xe21, 11);
    test_crc_bit_buffer(*factory, N, crc_generator_poly::CRC6, 0x61, 6);
  }
}