/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "crc_calculator_lut_impl.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/support/math_utils.h"

#if HAVE_SSE
#include <immintrin.h>
#endif // HAVE_SSE

using namespace srsran;

const std::map<crc_generator_poly, crc_calculator_lut_impl::crc_table_s> crc_calculator_lut_impl::crc_tables = {
    {crc_generator_poly::CRC24A, crc_calculator_lut_impl::crc_table_s(0x1864cfb, 24)},
    {crc_generator_poly::CRC24B, crc_calculator_lut_impl::crc_table_s(0X1800063, 24)},
    {crc_generator_poly::CRC24C, crc_calculator_lut_impl::crc_table_s(0X1B2B117, 24)},
    {crc_generator_poly::CRC16, crc_calculator_lut_impl::crc_table_s(0x11021, 16)},
    {crc_generator_poly::CRC11, crc_calculator_lut_impl::crc_table_s(0xe21, 11)},
    {crc_generator_poly::CRC6, crc_calculator_lut_impl::crc_table_s(0x61, 6)}};

crc_calculator_lut_impl::crc_table_s::crc_table_s(unsigned polynom_, unsigned order_) :
  order(order_), crcmask(((((uint64_t)1UL << (order - 1UL)) - 1UL) << 1UL) | 1UL), polynom(polynom_)
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

crc_calculator_lut_impl::crc_calculator_lut_impl(crc_generator_poly poly_) :
  table(crc_tables.at(poly_)), order(table.order), crcmask(table.crcmask), crc(0), poly(poly_)
{
  // Do nothing
}

crc_calculator_checksum_t srsran::crc_calculator_lut_impl::calculate_byte(span<const uint8_t> input)
{
  reset();

  // For each byte
  for (unsigned char byte : input) {
    put_byte(byte);
  }

  return get_checksum();
}

crc_calculator_checksum_t crc_calculator_lut_impl::calculate_bit(srsran::span<const uint8_t> input)
{
  reset();

  int a = 0;

  // Pack bits into bytes.
  unsigned nbytes = input.size() / 8;
  unsigned res8   = input.size() % 8;
  if (res8 > 0) {
    a = 1;
  }

  // Calculate CRC.
  for (unsigned i = 0; i < nbytes; i++) {
    // Get pack a byte from 8 bits.
    span<const uint8_t> pter = input.subspan(8U * i, 8);
#ifdef HAVE_SSE
    // Get 8 Bit
    __m64 mask = _mm_cmpgt_pi8(*(reinterpret_cast<const __m64*>(pter.data())), _mm_set1_pi8(0));

    // Get mask and reverse.
    uint8_t byte = reverse_byte(static_cast<uint8_t>(_mm_movemask_pi8(mask)));
#else  // HAVE_SSE
    uint8_t byte = (uint8_t)(srsvec::bit_pack(pter, 8) & 0xff);
#endif // HAVE_SSE
    put_byte(byte);
  }

  for (unsigned i = nbytes; i < nbytes + a; i++) {
    uint8_t byte = 0x00;
    for (unsigned k = 0; k < res8; k++) {
      byte |= (uint8_t)((input[i * 8 + k]) << (7U - k));
    }
    put_byte(byte);
  }

  crc = (uint32_t)get_checksum();

  // Reverse CRC res8 positions
  if (a == 1) {
    reversecrcbit(8 - res8);
  }

  return get_checksum();
}
crc_calculator_checksum_t crc_calculator_lut_impl::calculate(const bit_buffer& input)
{
  reset();

  // Pack bits into bytes.
  unsigned nbytes = input.size() / 8;
  unsigned res8   = input.size() % 8;

  // Extract and calculate CRC for each byte.
  for (unsigned i_byte = 0; i_byte != nbytes; ++i_byte) {
    uint8_t byte = input.get_byte(i_byte);
    put_byte(byte);
  }

  // Process remainder bits.
  if (res8 > 0) {
    uint8_t remainder = input.extract(nbytes * 8, res8);
    put_byte(remainder << (8 - res8));
  }

  crc = (uint32_t)get_checksum();

  if (res8 > 0) {
    // Reverse CRC res8 positions
    reversecrcbit(8 - res8);
  }

  return get_checksum();
}
