/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "crc_calculator_impl.h"

using namespace srsgnb;

const std::map<crc_generator_poly, crc_calculator_impl::crc_table_s> crc_calculator_impl::crc_tables = {
    {crc_generator_poly::CRC24A, crc_calculator_impl::crc_table_s(0x1864CFB, 24)},
    {crc_generator_poly::CRC24B, crc_calculator_impl::crc_table_s(0X1800063, 24)},
    {crc_generator_poly::CRC24C, crc_calculator_impl::crc_table_s(0X1B2B117, 24)},
    {crc_generator_poly::CRC16, crc_calculator_impl::crc_table_s(0x11021, 16)},
    {crc_generator_poly::CRC11, crc_calculator_impl::crc_table_s(0xE21, 11)},
    {crc_generator_poly::CRC6, crc_calculator_impl::crc_table_s(0x61, 6)}};

crc_calculator_impl::crc_table_s::crc_table_s(unsigned polynom, unsigned order_) :
  order(order_), crcmask(((((uint64_t)1UL << (order - 1UL)) - 1UL) << 1UL) | 1UL)
{
  unsigned pad        = (order < 8) ? (8 - order) : 0;
  unsigned ord        = order + pad - 8;
  uint64_t crchighbit = (uint64_t)1 << (order - 1);

  for (unsigned i = 0; i != 256; ++i) {
    uint64_t remainder = ((uint64_t)i) << ord;
    for (unsigned j = 0; j != 8; ++j) {
      bool bit = (remainder & crchighbit) != 0;
      remainder <<= 1U;
      if (bit) {
        remainder ^= polynom;
      }
    }
    (*this)[i] = (remainder >> pad) & crcmask;
  }
}

crc_calculator_impl::crc_calculator_impl(crc_generator_poly poly) :
  table(crc_tables.at(poly)), order(table.order), crcmask(table.crcmask), crc(0)
{
  // Do nothing
}

void crc_calculator_impl::put_byte(unsigned char byte)
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

crc_calculator_checksum_t crc_calculator_impl::get_checksum() const
{
  return static_cast<crc_calculator_checksum_t>(crc & crcmask);
}

crc_calculator_checksum_t srsgnb::crc_calculator_impl::calculate(const srsgnb::byte_buffer& input)
{
  // For each byte
  for (unsigned char byte : input) {
    put_byte(byte);
  }

  return get_checksum();
}

std::unique_ptr<crc_calculator> srsgnb::create_crc_calculator(crc_generator_poly poly)
{
  return std::make_unique<crc_calculator_impl>(poly);
}