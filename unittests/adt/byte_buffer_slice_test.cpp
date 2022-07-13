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

using namespace srsgnb;

std::vector<uint8_t> make_small_vec()
{
  return {1, 2, 3, 4, 5, 6};
}

std::vector<uint8_t> make_big_vec()
{
  std::vector<uint8_t> vec(byte_buffer_segment::capacity() - byte_buffer_segment::DEFAULT_HEADROOM);
  for (size_t i = 0; i < vec.size(); ++i) {
    vec[i] = i;
  }
  return vec;
}

void test_empty_slice()
{
  byte_buffer_slice pkt;

  TESTASSERT(pkt.empty());
  TESTASSERT_EQ(0, pkt.length());
  TESTASSERT(pkt.begin() == pkt.end());
}

void test_shallow_slice()
{
  std::vector<uint8_t> vec = make_big_vec();
  byte_buffer          pdu{vec};

  byte_buffer_slice slice{pdu.copy()};

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
  // pdu.append(1);
  // TESTASSERT(slice != pdu);
  // byte_buffer_view v{pdu, 0, pdu.length() - 1};
  // TESTASSERT(slice == v);
  // TODO: Fix.

  // Test that slice is a shallow copy.
  *pdu.begin() = 255U;
  TESTASSERT(slice == pdu);
  TESTASSERT(slice != vec);
}

void test_deep_slice()
{
  std::vector<uint8_t> vec = make_big_vec();
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

void test_move_slice()
{
  std::vector<uint8_t> vec = make_big_vec();
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

void test_slice_formatter()
{
  byte_buffer          pdu;
  std::vector<uint8_t> bytes = {1, 2, 3, 4, 15, 16, 255};
  pdu.append(bytes);
  byte_buffer_slice slice{std::move(pdu), 1, 4};

  std::string result = fmt::format("{}", slice);
  TESTASSERT_EQ("02 03 04 0f", result);
}

int main()
{
  test_empty_slice();
  test_shallow_slice();
  test_deep_slice();
  test_move_slice();
  test_slice_formatter();
}
