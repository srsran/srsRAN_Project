/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/srsvec/aligned_vec.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);

using namespace srsran;

void test_unpack(unsigned N)
{
  std::uniform_int_distribution<unsigned> dist(0, (1U << N) - 1U);

  // Create random value to unpack
  unsigned value = dist(rgen);

  // Create destination
  srsvec::aligned_vec<uint8_t> unpacked(N);

  // Unpack
  span<uint8_t> bit_buf = srsvec::bit_unpack(unpacked, value, N);

  // Make sure the allocate dbvector size remains the same while all bits are taken in bit_buff
  TESTASSERT_EQ(unpacked.size(), N);
  TESTASSERT(bit_buf.empty());

  // Assert each bit
  for (unsigned i = 0; i != N; i++) {
    uint8_t gold = (value >> (N - 1 - i)) & 1U;
    TESTASSERT_EQ(gold, unpacked[i]);
  }
}

void test_unpack_vector(unsigned N)
{
  unsigned                                nbytes = N;
  unsigned                                nbits  = nbytes * 8;
  std::uniform_int_distribution<unsigned> dist(0, UINT8_MAX);

  // Create random value to unpack
  dynamic_bit_buffer packed(nbits);
  for (uint8_t& value : packed.get_buffer()) {
    value = dist(rgen);
  }

  // Create destination
  srsvec::aligned_vec<uint8_t> unpacked(nbits);

  // Generate expected values.
  srsvec::aligned_vec<uint8_t> expected(nbits);
  std::generate(expected.begin(), expected.end(), [&, index = 0]() mutable { return packed.extract(index++, 1); });

  // Unpack
  srsvec::bit_unpack(unpacked, packed);

  // Assert each bit
  TESTASSERT_EQ(span<const uint8_t>(expected), span<const uint8_t>(unpacked));
}

void test_pack(unsigned N)
{
  std::uniform_int_distribution<uint8_t> dist(0, 1U);

  // Create unpacked data
  srsvec::aligned_vec<uint8_t> unpacked(N);
  for (uint8_t& value : unpacked) {
    value = dist(rgen);
  }

  // Pack
  span<const uint8_t> bit_buf = unpacked;
  unsigned            value   = srsvec::bit_pack(bit_buf, N);

  // Make sure the allocate dbvector size remains the same while all bits are taken in bit_buff
  TESTASSERT_EQ(unpacked.size(), N);
  TESTASSERT(bit_buf.empty());

  // Assert each bit
  for (unsigned i = 0; i != N; ++i) {
    uint8_t gold = (value >> (N - 1 - i)) & 1U;
    TESTASSERT_EQ(gold, unpacked[i]);
  }
}

void test_pack_vector(unsigned N)
{
  unsigned                                nbytes = N;
  unsigned                                nbits  = nbytes * 8;
  std::uniform_int_distribution<unsigned> dist(0, 1U);

  // Create unpacked data
  srsvec::aligned_vec<uint8_t> unpacked(nbits);
  for (uint8_t& value : unpacked) {
    value = dist(rgen);
  }

  // Create destination
  dynamic_bit_buffer packed(nbits);

  // Unpack
  srsvec::bit_pack(packed, unpacked);

  // Assert each bit
  for (unsigned i = 0; i != nbits; i++) {
    uint8_t gold = packed.extract(i, 1);
    TESTASSERT_EQ(gold, unpacked[i]);
  }
}

void test_copy_offset_vector(unsigned nof_bytes)
{
  unsigned nof_bits = nof_bytes * 8;

  std::uniform_int_distribution<unsigned> dist_byte(0, 255U);

  // Read offsets in bits.
  std::vector<unsigned> read_offsets = {0, 5, 8, 15};

  for (auto read_offset : read_offsets) {
    // Determine source buffer dimensions.
    unsigned source_nof_bytes = nof_bytes + divide_ceil(read_offset, 8);

    // Create source and destination buffers.
    std::vector<uint8_t> source(source_nof_bytes);
    dynamic_bit_buffer   destination(nof_bits);

    // Fill source buffer with random bits.
    for (auto& byte : source) {
      byte = dist_byte(rgen);
    }

    // Copy from source to destination.
    srsvec::copy_offset(destination, source, read_offset);

    // Assert each bit.
    for (unsigned i_bit = 0; i_bit != nof_bits; ++i_bit) {
      // Source byte to extract.
      unsigned i_byte = (i_bit + read_offset) / 8;

      // Extract the source byte and mask the desired bit.
      uint8_t source_bit = (source[i_byte] >> (7U - ((i_bit + read_offset) % 8U))) & 1U;

      // Destination buffer bit where the source bit should have been copied.
      uint8_t copy_bit = destination.extract(i_bit, 1);

      TESTASSERT_EQ(source_bit, copy_bit);
    }
  }
}

void test_copy_offset_bit_buffers(unsigned nof_bytes)
{
  unsigned nof_bits = nof_bytes * 8;

  std::uniform_int_distribution<unsigned> dist(0, 1U);

  // Read and write offsets. All combinations will be tested.
  std::vector<unsigned> read_offsets  = {0, 5, 8, 15};
  std::vector<unsigned> write_offsets = {0, 5, 8, 15};

  for (auto read_offset : read_offsets) {
    for (auto write_offset : write_offsets) {
      // Determine buffer dimensions.
      unsigned source_size = nof_bits + read_offset;
      unsigned dest_size   = nof_bits + write_offset;

      // Create source and destination buffers.
      dynamic_bit_buffer source(source_size);
      dynamic_bit_buffer destination(dest_size);

      // Fill source buffer with random bits.
      for (unsigned i_bit = 0; i_bit != source_size; ++i_bit) {
        source.insert(dist(rgen), i_bit, 1);
      }

      // Copy from source to destination.
      srsvec::copy_offset(destination, write_offset, source, read_offset, nof_bits);

      // Assert each bit.
      for (unsigned i_bit = 0; i_bit != nof_bits; ++i_bit) {
        // Source bit.
        uint8_t source_bit = source.extract(i_bit + read_offset, 1);

        // Destination buffer bit where the source bit should have been copied.
        uint8_t copy_bit = destination.extract(i_bit + write_offset, 1);

        TESTASSERT_EQ(source_bit, copy_bit);
      }
    }
  }
}

int main()
{
  std::vector<unsigned> sizes = {1, 5, 7, 19, 23, 32};

  for (unsigned N : sizes) {
    test_unpack(N);
    test_unpack_vector(N);
    test_pack(N);
    test_pack_vector(N);
    test_copy_offset_vector(N);
    test_copy_offset_bit_buffers(N);
  }
}