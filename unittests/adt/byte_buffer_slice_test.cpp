/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

/// Creates a vector of specified size.
std::vector<uint8_t> make_vec(unsigned size)
{
  std::vector<uint8_t> vec(size);
  for (size_t i = 0; i < vec.size(); ++i) {
    vec[i] = get_random_uint(0, 255);
  }
  return vec;
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
  std::vector<uint8_t> vec = make_vec(get_random_uint(1, byte_buffer_segment::SEGMENT_SIZE * 4));
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
  std::vector<uint8_t> vec = make_vec(get_random_uint(1, byte_buffer_segment::SEGMENT_SIZE * 4));
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
    TESTASSERT_EQ(vec[i], slice[i]);
  }

  // Test iterator.
  TESTASSERT(slice.begin() != slice.end());
  unsigned count = 0;
  for (uint8_t v : slice) {
    TESTASSERT_EQ(vec[count++], v);
  }
  TESTASSERT_EQ(vec.size(), count);

  // Test operator==.
  TESTASSERT(slice == pdu);
  TESTASSERT(pdu == slice);
  TESTASSERT(slice == vec);

  // Test slice doesn't get altered by underlying byte_buffer extension.
  pdu.append(1);
  TESTASSERT(slice != pdu);
  byte_buffer_view v{pdu, 0, pdu.length() - 1};
  TESTASSERT(slice == v);

  // Test that slice is a shallow copy.
  *pdu.begin() = 255U;
  TESTASSERT(slice != pdu);
  TESTASSERT(slice == vec);
}

TEST(byte_buffer_slice, move)
{
  std::vector<uint8_t> vec = make_vec(byte_buffer_segment::SEGMENT_SIZE - byte_buffer_segment::DEFAULT_HEADROOM);
  byte_buffer          pdu{vec};

  byte_buffer_slice slice{std::move(pdu)};
  TESTASSERT(pdu.empty());

  // Test operator[].
  for (unsigned i = 0; i < vec.size(); ++i) {
    TESTASSERT_EQ(vec[i], slice[i]);
  }

  // Test iterator.
  TESTASSERT(slice.begin() != slice.end());
  unsigned count = 0;
  for (uint8_t v : slice) {
    TESTASSERT_EQ(vec[count++], v);
  }
  TESTASSERT_EQ(vec.size(), count);

  // Test operator==.
  TESTASSERT(slice == vec);
}

TEST(byte_buffer_slice, formatter)
{
  byte_buffer          pdu;
  std::vector<uint8_t> bytes = {1, 2, 3, 4, 15, 16, 255};
  pdu.append(bytes);
  byte_buffer_slice slice{std::move(pdu), 1, 4};

  std::string result = fmt::format("{}", slice);
  TESTASSERT_EQ("02 03 04 0f", result);
}
