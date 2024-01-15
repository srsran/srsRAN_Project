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

#include "srsran/adt/span.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;

void test_bit_encoder()
{
  byte_buffer bytes;
  bit_encoder enc(bytes);

  // TEST: Empty buffer.

  TESTASSERT_EQ(0, enc.nof_bytes());
  TESTASSERT_EQ(0, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());

  enc.align_bytes_zero();
  TESTASSERT_EQ(0, enc.nof_bytes());
  TESTASSERT_EQ(0, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());

  enc.pack(0, 0);
  TESTASSERT_EQ(0, enc.nof_bytes());
  TESTASSERT_EQ(0, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());

  enc.pack_bytes(byte_buffer{});
  TESTASSERT_EQ(0, enc.nof_bytes());
  TESTASSERT_EQ(0, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());

  // TEST: bit packing.

  // byte_buffer:  [101_____]
  // Written bits: [101]
  enc.pack(0b101, 3);
  TESTASSERT_EQ(1, enc.nof_bytes());
  TESTASSERT_EQ(3, enc.nof_bits());
  TESTASSERT_EQ(0b10100000, *bytes.begin());
  TESTASSERT_EQ(3, enc.next_bit_offset());

  // byte_buffer:  [10101___]
  // Written bits:    [01]
  enc.pack(0b1, 2);
  TESTASSERT_EQ(1, enc.nof_bytes());
  TESTASSERT_EQ(5, enc.nof_bits());
  TESTASSERT_EQ(0b10101000, *bytes.begin());
  TESTASSERT_EQ(5, enc.next_bit_offset());

  // TEST: byte packing.

  // byte_buffer:  [10101000][00001  000][00010  000][00011___]
  // Written bits:      [000  00001][000  00010][000  00011]
  byte_buffer vec = {0b1, 0b10, 0b11};
  enc.pack_bytes(vec);
  TESTASSERT_EQ(4, enc.nof_bytes());
  TESTASSERT_EQ(5 + 3 * 8, enc.nof_bits());
  TESTASSERT_EQ(5, enc.next_bit_offset());
  byte_buffer vec2 = {0b10101000, 0b00001000, 0b00010000, 0b00011000};
  TESTASSERT(bytes == vec2);

  // TEST: alignment padding.
  // byte_buffer:  [10101000][00001000][00010000][00011000]
  // Written bits:                                    [000]
  enc.align_bytes_zero();
  TESTASSERT_EQ(4, enc.nof_bytes());
  TESTASSERT_EQ(4 * 8, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());
  TESTASSERT(bytes == vec2);

  // byte_buffer:  [10101000][00001000][00010000][00011000][00000000]
  // Written bits:                                         [00000000]
  enc.pack(0, 8);
  // No bits written.
  enc.align_bytes_zero();
  TESTASSERT_EQ(5, enc.nof_bytes());
  TESTASSERT_EQ(5 * 8, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());

  // TEST: fmt formatting of aligned bits
  fmt::print("encoded bits: {}\n", enc);
  std::string s            = fmt::format("{}", enc);
  std::string expected_str = "10101000 00001000 00010000 00011000 00000000";
  TESTASSERT_EQ(expected_str, s);

  // TEST: fmt formatting of unaligned bits
  // byte_buffer:  [10101000][00001000][00010000][00011000][00000000][10______]
  // Written bits:                                                   [10]
  enc.pack(0b10, 2);
  fmt::print("encoded bits: {}\n", enc);
  s            = fmt::format("{}", enc);
  expected_str = "10101000 00001000 00010000 00011000 00000000 10";
  TESTASSERT_EQ(expected_str, s);

  // TEST: encode full all 64 bit of a uint64_t
  uint64_t large = 0xc000000000000001;
  enc.pack(large, 64);

  const uint8_t packed_vec[] = {
      0b10101000,
      0b00001000,
      0b00010000,
      0b00011000,
      0b00000000,
      0b10110000, // combined byte having MSBs of 0xc0
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x40, // two MSBs contain LSBs of 0x01
  };

  TESTASSERT(bytes == byte_buffer{packed_vec});
}

void test_bit_encoder_bool()
{
  byte_buffer bytes;
  bit_encoder enc(bytes);
  uint8_t     dummy          = 0;
  bool        bit1           = true;
  bool        bit0           = false;
  byte_buffer expected_bytes = {0x02};

  enc.pack(dummy, 6);
  enc.pack(bit1, 1);
  enc.pack(bit0, 1);

  TESTASSERT(expected_bytes == bytes);
}

void test_bit_encoder_uint64_aligned()
{
  byte_buffer bytes;
  bit_encoder enc(bytes);
  uint64_t    val64          = 0xc00f00000000f001;
  byte_buffer expected_bytes = {0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x01};

  TESTASSERT_EQ(0, enc.nof_bytes());
  TESTASSERT_EQ(0, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());

  enc.pack(val64, 64);
  TESTASSERT_EQ(8, enc.nof_bytes());
  TESTASSERT_EQ(64, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());
  TESTASSERT(expected_bytes == bytes);
}

void test_bit_encoder_uint64_offset()
{
  byte_buffer bytes;
  bit_encoder enc(bytes);
  uint8_t     val1           = 1;
  uint64_t    val64          = 0xc00f00000000f001;
  byte_buffer expected_bytes = {0xe0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x78, 0x00, 0x80};

  TESTASSERT_EQ(0, enc.nof_bytes());
  TESTASSERT_EQ(0, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());

  enc.pack(val1, 1);
  TESTASSERT_EQ(1, enc.nof_bytes());
  TESTASSERT_EQ(1, enc.nof_bits());
  TESTASSERT_EQ(1, enc.next_bit_offset());

  enc.pack(val64, 64);
  TESTASSERT_EQ(9, enc.nof_bytes());
  TESTASSERT_EQ(65, enc.nof_bits());
  TESTASSERT_EQ(1, enc.next_bit_offset());
  TESTASSERT(expected_bytes == bytes);
}

void test_bit_decoder_empty_buffer()
{
  byte_buffer          bytes;
  bit_decoder          dec(bytes);
  uint32_t             val;
  std::vector<uint8_t> vec;

  TESTASSERT_EQ(0, dec.nof_bytes());
  TESTASSERT_EQ(0, dec.nof_bits());
  TESTASSERT_EQ(0, dec.data().length());
  TESTASSERT_EQ(0, dec.next_bit_offset());

  TESTASSERT(dec.advance_bits(0));
  TESTASSERT_EQ(0, dec.nof_bytes());
  TESTASSERT_EQ(0, dec.nof_bits());

  dec.align_bytes();
  TESTASSERT_EQ(0, dec.nof_bytes());
  TESTASSERT_EQ(0, dec.nof_bits());

  val = 1;
  TESTASSERT(dec.unpack(val, 0));
  TESTASSERT_EQ(0, dec.nof_bytes());
  TESTASSERT_EQ(0, dec.nof_bits());
  TESTASSERT_EQ(val, 0);

  TESTASSERT(dec.unpack_bytes(vec));
  TESTASSERT_EQ(0, dec.nof_bytes());
  TESTASSERT_EQ(0, dec.nof_bits());
  TESTASSERT_EQ(0, vec.size());
  TESTASSERT_EQ(0, dec.next_bit_offset());
}

void test_bit_decoder()
{
  byte_buffer          bytes = {0b1, 0b10, 0b11, 0b100};
  bit_decoder          dec(bytes);
  uint32_t             val;
  std::vector<uint8_t> vec;

  TESTASSERT_EQ(0, dec.nof_bytes());
  TESTASSERT_EQ(0, dec.nof_bits());
  TESTASSERT(bytes == dec.data());

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:   [00]
  TESTASSERT(dec.unpack(val, 2));
  TESTASSERT_EQ(1, dec.nof_bytes());
  TESTASSERT_EQ(2, dec.nof_bits());
  TESTASSERT_EQ(2, dec.next_bit_offset());
  TESTASSERT_EQ(val, 0);

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:     [000001]
  TESTASSERT(dec.unpack(val, 6));
  TESTASSERT_EQ(1, dec.nof_bytes());
  TESTASSERT_EQ(8, dec.nof_bits());
  TESTASSERT_EQ(0, dec.next_bit_offset());
  TESTASSERT_EQ(val, 0b1);

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:             [0]
  TESTASSERT(dec.unpack(val, 1));
  TESTASSERT_EQ(2, dec.nof_bytes());
  TESTASSERT_EQ(9, dec.nof_bits());
  TESTASSERT_EQ(1, dec.next_bit_offset());
  TESTASSERT_EQ(val, 0);

  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:              [0000010  0]
  vec.resize(1);
  TESTASSERT(dec.unpack_bytes(vec));
  TESTASSERT_EQ(3, dec.nof_bytes());
  TESTASSERT_EQ(9 + 8, dec.nof_bits());
  TESTASSERT_EQ(1, dec.next_bit_offset());
  TESTASSERT_EQ(0b100, vec[0]);

  // byte_buffer:   [00000001][00000010][00000011][00000100]
  // Advanced bits:                       ---------^
  dec.align_bytes();
  TESTASSERT_EQ(3, dec.nof_bytes());
  TESTASSERT_EQ(3 * 8, dec.nof_bits());
  TESTASSERT_EQ(0, dec.next_bit_offset());

  // TEST: fmt formatting of aligned bits.
  fmt::print("decoded bits: {}\n", dec);
  std::string s            = fmt::format("{}", dec);
  std::string expected_str = "00000001 00000010 00000011";
  TESTASSERT_EQ(expected_str, s);

  // TEST: fmt formatting of unaligned bits.
  // byte_buffer: [00000001][00000010][00000011][00000100]
  // Read bits:                                 [00]
  TESTASSERT(dec.unpack(val, 2));
  fmt::print("decoded bits: {}\n", dec);
  s            = fmt::format("{}", dec);
  expected_str = "00000001 00000010 00000011 00";
  TESTASSERT_EQ(expected_str, s);

  // TEST: unpack beyond limits
  TESTASSERT_EQ(3 * 8 + 2, dec.nof_bits());
  TESTASSERT(not dec.unpack(val, 8));
  TESTASSERT_EQ(4 * 8, dec.nof_bits());

  TESTASSERT(not dec.unpack_bytes(vec));
  TESTASSERT_EQ(4 * 8, dec.nof_bits());

  TESTASSERT(not dec.advance_bits(1));
  TESTASSERT_EQ(4 * 8, dec.nof_bits());
}

void test_bit_decoder_bool()
{
  byte_buffer bytes = {0x02};
  bit_decoder dec(bytes);
  uint8_t     dummy;
  bool        bit1, bit0;

  TESTASSERT(dec.unpack(dummy, 6));
  TESTASSERT(dec.unpack(bit1, 1));
  TESTASSERT(dec.unpack(bit0, 1));

  TESTASSERT_EQ(bit1, true);
  TESTASSERT_EQ(bit0, false);
}

void test_bit_decoder_uint64_aligned()
{
  byte_buffer bytes = {0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x01};
  bit_decoder dec(bytes);
  uint64_t    val;

  TESTASSERT_EQ(0, dec.nof_bytes());
  TESTASSERT_EQ(0, dec.nof_bits());
  TESTASSERT(bytes == dec.data());

  TESTASSERT(dec.unpack(val, 64));
  TESTASSERT_EQ(8, dec.nof_bytes());
  TESTASSERT_EQ(64, dec.nof_bits());
  TESTASSERT_EQ(0, dec.next_bit_offset());
  TESTASSERT_EQ(val, 0xc00f00000000f001);
}

void test_bit_decoder_uint64_offset()
{
  byte_buffer bytes = {0xe0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x78, 0x00, 0x80};
  bit_decoder dec(bytes);
  uint64_t    val;

  TESTASSERT_EQ(0, dec.nof_bytes());
  TESTASSERT_EQ(0, dec.nof_bits());
  TESTASSERT(bytes == dec.data());

  TESTASSERT(dec.unpack(val, 1));
  TESTASSERT_EQ(1, dec.nof_bytes());
  TESTASSERT_EQ(1, dec.nof_bits());
  TESTASSERT_EQ(1, dec.next_bit_offset());
  TESTASSERT_EQ(val, 1);

  TESTASSERT(dec.unpack(val, 64));
  TESTASSERT_EQ(9, dec.nof_bytes());
  TESTASSERT_EQ(65, dec.nof_bits());
  TESTASSERT_EQ(1, dec.next_bit_offset());
  TESTASSERT_EQ(val, 0xc00f00000000f001);
}

int main()
{
  test_bit_encoder();
  test_bit_encoder_bool();
  test_bit_encoder_uint64_aligned();
  test_bit_encoder_uint64_offset();
  test_bit_decoder_empty_buffer();
  test_bit_decoder();
  test_bit_decoder_bool();
  test_bit_decoder_uint64_aligned();
  test_bit_decoder_uint64_offset();
}
