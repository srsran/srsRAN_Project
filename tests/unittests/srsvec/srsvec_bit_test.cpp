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

#include "srsran/srsvec/aligned_vec.h"
#include "srsran/srsvec/bit.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);

using namespace srsran;

namespace {

using SrsvecBitParams = unsigned;

class SrsvecBitFixture : public ::testing::TestWithParam<SrsvecBitParams>
{
protected:
  unsigned size;
  unsigned nbits;
  unsigned offset;

  static void SetUpTestSuite() {}

  void SetUp() override
  {
    // Get test parameters.
    auto params = GetParam();
    size        = params;
    nbits       = size * 8;
    offset      = size;
  }
};

TEST_P(SrsvecBitFixture, SrsvecBitTestUnpack)
{
  std::uniform_int_distribution<unsigned> dist(0, (1U << size) - 1U);

  // Create random value to unpack
  unsigned value = dist(rgen);

  // Create destination
  srsvec::aligned_vec<uint8_t> unpacked(size);

  // Unpack
  span<uint8_t> bit_buf = srsvec::bit_unpack(unpacked, value, size);

  // Make sure the allocate dbvector size remains the same while all bits are taken in bit_buff
  ASSERT_EQ(unpacked.size(), size);
  ASSERT_TRUE(bit_buf.empty());

  // Assert each bit
  for (unsigned i = 0; i != size; i++) {
    uint8_t gold = (value >> (size - 1 - i)) & 1U;
    ASSERT_EQ(gold, unpacked[i]);
  }
}

TEST_P(SrsvecBitFixture, SrsvecBitTestUnpackVector)
{
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
  ASSERT_EQ(span<const uint8_t>(expected), span<const uint8_t>(unpacked));
}

TEST_P(SrsvecBitFixture, SrsvecBitTestPack)
{
  std::uniform_int_distribution<uint8_t> dist(0, 1U);

  // Create unpacked data
  srsvec::aligned_vec<uint8_t> unpacked(size);
  for (uint8_t& value : unpacked) {
    value = dist(rgen);
  }

  // Pack
  span<const uint8_t> bit_buf = unpacked;
  unsigned            value   = srsvec::bit_pack(bit_buf, size);

  // Make sure the allocate dbvector size remains the same while all bits are taken in bit_buff
  ASSERT_EQ(unpacked.size(), size);
  ASSERT_TRUE(bit_buf.empty());

  // Assert each bit
  for (unsigned i = 0; i != size; ++i) {
    uint8_t gold = (value >> (size - 1 - i)) & 1U;
    ASSERT_EQ(gold, unpacked[i]);
  }
}

TEST_P(SrsvecBitFixture, SrsvecBitTestPackVector)
{
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
    ASSERT_EQ(gold, unpacked[i]);
  }
}

TEST_P(SrsvecBitFixture, SrsvecBitTestPackFullVector)
{
  std::uniform_int_distribution<unsigned> dist(0, 1U);

  // Create unpacked data
  srsvec::aligned_vec<uint8_t> unpacked(size);
  for (uint8_t& value : unpacked) {
    value = dist(rgen);
  }

  unsigned packed = srsvec::bit_pack(unpacked);

  for (unsigned i = 0; i < size; i++) {
    uint8_t gold = (packed >> (size - 1 - i)) & 1U;
    ASSERT_EQ(gold, unpacked[i]);
  }
}

TEST_P(SrsvecBitFixture, SrsvecBitTestPackOffsetVector)
{
  std::uniform_int_distribution<unsigned> dist(0, 1U);

  // Create unpacked data
  srsvec::aligned_vec<uint8_t> unpacked(nbits);
  for (uint8_t& value : unpacked) {
    value = dist(rgen);
  }

  // Create destination
  dynamic_bit_buffer packed(nbits + offset);

  srsran::srsvec::bit_pack(packed, offset, unpacked);

  for (unsigned i = 0; i < (nbits - offset); i++) {
    uint8_t expected_bit = unpacked[i];
    uint8_t actual_bit   = packed.extract(offset + i, 1);
    ASSERT_EQ(expected_bit, actual_bit);
  }
}

TEST_P(SrsvecBitFixture, SrsvecBitTestCopyOffsetVector)
{
  std::uniform_int_distribution<unsigned> dist_byte(0, 255U);

  // Read offsets in bits.
  std::vector<unsigned> read_offsets = {0, 5, 8, 15};

  for (auto read_offset : read_offsets) {
    // Determine source buffer dimensions.
    unsigned source_nof_bytes = size + divide_ceil(read_offset, 8);
    unsigned source_nof_bits  = source_nof_bytes * 8;

    // Create source and destination buffers.
    std::vector<uint8_t> source(source_nof_bytes);
    dynamic_bit_buffer   destination(source_nof_bits - read_offset);

    // Fill source buffer with random bits.
    for (auto& byte : source) {
      byte = dist_byte(rgen);
    }

    // Copy from source to destination.
    srsvec::copy_offset(destination, source, read_offset);

    // Assert each bit.
    for (unsigned i_bit = 0; i_bit != (source_nof_bits - read_offset); ++i_bit) {
      // Source byte to extract.
      unsigned i_byte = (i_bit + read_offset) / 8;

      // Extract the source byte and mask the desired bit.
      uint8_t source_bit = (source[i_byte] >> (7U - ((i_bit + read_offset) % 8U))) & 1U;

      // Destination buffer bit where the source bit should have been copied.
      uint8_t copy_bit = destination.extract(i_bit, 1);

      ASSERT_EQ(source_bit, copy_bit);
    }
  }
}

TEST_P(SrsvecBitFixture, SrsvecBitTestCopyOffsetBitBuffers)
{
  std::uniform_int_distribution<unsigned> dist(0, 1U);

  // Read and write offsets. All combinations will be tested.
  std::vector<unsigned> read_offsets  = {0, 5, 8, 15};
  std::vector<unsigned> write_offsets = {0, 5, 8, 15};

  for (auto read_offset : read_offsets) {
    for (auto write_offset : write_offsets) {
      // Determine buffer dimensions.
      unsigned source_size = nbits + read_offset;
      unsigned dest_size   = nbits + write_offset;

      // Create source and destination buffers.
      dynamic_bit_buffer source(source_size);
      dynamic_bit_buffer destination(dest_size);

      // Fill source buffer with random bits.
      for (unsigned i_bit = 0; i_bit != source_size; ++i_bit) {
        source.insert(dist(rgen), i_bit, 1);
      }

      // Copy from source to destination.
      srsvec::copy_offset(destination, write_offset, source, read_offset, nbits);

      // Assert each bit.
      for (unsigned i_bit = 0; i_bit != nbits; ++i_bit) {
        // Source bit.
        uint8_t source_bit = source.extract(i_bit + read_offset, 1);

        // Destination buffer bit where the source bit should have been copied.
        uint8_t copy_bit = destination.extract(i_bit + write_offset, 1);

        ASSERT_EQ(source_bit, copy_bit);
      }
    }
  }
}

TEST_P(SrsvecBitFixture, SrsvecBitTestUnpackVectorWithRemainder)
{
  std::uniform_int_distribution<unsigned> dist(0, UINT8_MAX);

  // Create random value to unpack
  dynamic_bit_buffer packed(15);
  for (uint8_t& value : packed.get_buffer()) {
    value = dist(rgen);
  }

  // Create destination
  srsvec::aligned_vec<uint8_t> unpacked(15);

  // Generate expected values.
  srsvec::aligned_vec<uint8_t> expected(15);
  std::generate(expected.begin(), expected.end(), [&, index = 0]() mutable { return packed.extract(index++, 1); });

  // Unpack
  srsvec::bit_unpack(unpacked, packed);

  // Assert each bit
  ASSERT_EQ(span<const uint8_t>(expected), span<const uint8_t>(unpacked));
}

TEST_P(SrsvecBitFixture, SrsvecBitTestUnpackVectorOffset)
{
  // std::uniform_int_distribution<unsigned> dist(0, (1U << size) - 1U);
  std::uniform_int_distribution<unsigned> dist(0, 255U);

  // Create random value to unpack
  dynamic_bit_buffer packed(nbits);
  for (uint8_t& value : packed.get_buffer()) {
    value = dist(rgen);
  }

  // Create destination
  srsvec::aligned_vec<uint8_t> unpacked(nbits - offset);

  // Generate expected values.
  srsvec::aligned_vec<uint8_t> expected(nbits - offset);
  std::generate(expected.begin(), expected.end(), [&, index = offset]() mutable { return packed.extract(index++, 1); });

  // Unpack
  srsvec::bit_unpack(unpacked, packed, offset);

  // Assert each bit
  ASSERT_EQ(span<const uint8_t>(expected), span<const uint8_t>(unpacked));
}

INSTANTIATE_TEST_SUITE_P(SrsvecBitTest, SrsvecBitFixture, ::testing::Values(1, 5, 7, 19, 23, 32));

} // namespace