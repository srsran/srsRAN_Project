/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/srslog/bundled/fmt/ostream.h"
#include "srsgnb/support/bit_encoding.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

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

  enc.pack(0b101, 3);
  TESTASSERT_EQ(1, enc.nof_bytes());
  TESTASSERT_EQ(3, enc.nof_bits());
  TESTASSERT_EQ(0b10100000, *bytes.begin());
  TESTASSERT_EQ(3, enc.next_bit_offset());

  enc.pack(0b1, 2);
  TESTASSERT_EQ(1, enc.nof_bytes());
  TESTASSERT_EQ(5, enc.nof_bits());
  TESTASSERT_EQ(0b10101000, *bytes.begin());
  TESTASSERT_EQ(5, enc.next_bit_offset());

  // TEST: byte packing.

  byte_buffer vec = {0b1, 0b10, 0b11};
  enc.pack_bytes(vec);
  TESTASSERT_EQ(4, enc.nof_bytes());
  TESTASSERT_EQ(5 + 3 * 8, enc.nof_bits());
  TESTASSERT_EQ(5, enc.next_bit_offset());
  byte_buffer vec2 = {0b10101000, 0b00001000, 0b00010000, 0b00011000};
  TESTASSERT(bytes == vec2);

  // TEST: alignment padding.
  enc.align_bytes_zero();
  TESTASSERT_EQ(4, enc.nof_bytes());
  TESTASSERT_EQ(4 * 8, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());
  TESTASSERT(bytes == vec2);

  enc.pack(0, 8);
  enc.align_bytes_zero();
  TESTASSERT_EQ(5, enc.nof_bytes());
  TESTASSERT_EQ(5 * 8, enc.nof_bits());
  TESTASSERT_EQ(0, enc.next_bit_offset());

  // TEST: print
  fmt::print("encoded bits: {}\n", enc);

  enc.pack(0b10, 2);
  fmt::print("encoded bits: {}\n", enc);
}

int main()
{
  test_bit_encoder();
}