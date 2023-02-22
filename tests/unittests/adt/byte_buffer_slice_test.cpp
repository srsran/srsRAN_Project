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

#include "srsran/adt/byte_buffer.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

/// Creates a vector of specified size.
static std::vector<uint8_t> make_vec(unsigned size)
{
  return test_rgen::random_vector<uint8_t>(size);
}

TEST(byte_buffer_slice, empty)
{
  byte_buffer_slice pkt;

  ASSERT_TRUE(pkt.empty());
  ASSERT_EQ(0, pkt.length());
  ASSERT_EQ(pkt.begin(), pkt.end());
}

TEST(byte_buffer_slice, ctor_arg_span_uint8)
{
  std::vector<uint8_t> vec = make_vec(test_rgen::uniform_int<unsigned>(1, byte_buffer_segment::SEGMENT_SIZE * 4));
  byte_buffer_slice    slice(vec);

  ASSERT_EQ(slice.length(), vec.size());
  // Test operator[].
  for (unsigned i = 0; i < vec.size(); ++i) {
    ASSERT_EQ(vec[i], slice[i]);
  }
  // Test iterator.
  ASSERT_TRUE(std::equal(slice.begin(), slice.end(), vec.begin(), vec.end()));
  unsigned count = 0;
  for (uint8_t b : slice) {
    ASSERT_EQ(vec[count++], b);
  }
  // Test operator==.
  ASSERT_EQ(slice, vec);
}

TEST(byte_buffer_slice, shallow_copy)
{
  std::vector<uint8_t> vec = make_vec(test_rgen::uniform_int<unsigned>(1, byte_buffer_segment::SEGMENT_SIZE * 4));
  byte_buffer          pdu(vec);

  byte_buffer_slice slice{pdu.copy()};

  // Test operator==.
  ASSERT_EQ(slice, pdu);
  ASSERT_EQ(pdu, slice);
  ASSERT_EQ(slice, vec);
  //  ASSERT_EQ(vec, slice); // TODO.

  // slice gets altered because it is a shallow copy.
  pdu[0]++;
  ASSERT_EQ(pdu, slice);
  ASSERT_NE(slice, vec);

  // Test slice doesn't get altered by underlying byte_buffer extension.
  // pdu.append(1);
  // TESTASSERT(slice != pdu);
  // byte_buffer_view v{pdu, 0, pdu.length() - 1};
  // TESTASSERT(slice == v);
  // TODO: Fix.
}

TEST(byte_buffer_slice, deep_slice)
{
  std::vector<uint8_t> vec = make_vec(byte_buffer_segment::SEGMENT_SIZE - byte_buffer_segment::DEFAULT_HEADROOM);
  byte_buffer          pdu{vec};

  byte_buffer_slice slice{pdu.deep_copy()};

  // Test operator[].
  for (unsigned i = 0; i < vec.size(); ++i) {
    ASSERT_EQ(vec[i], slice[i]);
  }

  // Test iterator.
  ASSERT_NE(slice.begin(), slice.end());
  unsigned count = 0;
  for (uint8_t v : slice) {
    ASSERT_EQ(vec[count++], v);
  }
  ASSERT_EQ(vec.size(), count);

  // Test operator==.
  ASSERT_EQ(slice, pdu);
  ASSERT_EQ(pdu, slice);
  ASSERT_EQ(slice, vec);

  // Test slice doesn't get altered by underlying byte_buffer extension.
  pdu.append(1);
  ASSERT_NE(slice, pdu);
  byte_buffer_view v{pdu, 0, pdu.length() - 1};
  ASSERT_EQ(slice, v);

  // Test that slice is a shallow copy.
  *pdu.begin() = 255U;
  ASSERT_NE(slice, pdu);
  ASSERT_EQ(slice, vec);
}

TEST(byte_buffer_slice, move)
{
  std::vector<uint8_t> vec = make_vec(byte_buffer_segment::SEGMENT_SIZE - byte_buffer_segment::DEFAULT_HEADROOM);
  byte_buffer          pdu{vec};

  byte_buffer_slice slice{std::move(pdu)};
  ASSERT_TRUE(pdu.empty());

  // Test operator[].
  for (unsigned i = 0; i < vec.size(); ++i) {
    ASSERT_EQ(vec[i], slice[i]);
  }

  // Test iterator.
  ASSERT_NE(slice.begin(), slice.end());
  unsigned count = 0;
  for (uint8_t v : slice) {
    ASSERT_EQ(vec[count++], v);
  }
  ASSERT_EQ(vec.size(), count);

  // Test operator==.
  ASSERT_EQ(slice, vec);
}

TEST(byte_buffer_slice, formatter)
{
  byte_buffer          pdu;
  std::vector<uint8_t> bytes = {1, 2, 3, 4, 15, 16, 255};
  pdu.append(bytes);
  byte_buffer_slice slice{std::move(pdu), 1, 4};

  std::string result = fmt::format("{}", slice);
  ASSERT_EQ("02 03 04 0f", result);
}
