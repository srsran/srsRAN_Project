/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/byte_buffer2.h"
#include <gtest/gtest.h>
#include <list>

using namespace srsran;

static_assert(std::is_same<byte_buffer_view2::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer_view2::iterator::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer_view2::const_iterator::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer_view2::iterator::reference, uint8_t&>::value, "Invalid reference type");
static_assert(std::is_same<byte_buffer_view2::const_iterator::reference, const uint8_t&>::value,
              "Invalid reference type");
static_assert(std::is_same<byte_buffer_view2::const_iterator::pointer, const uint8_t*>::value, "Invalid pointer type");
static_assert(is_byte_buffer_range<byte_buffer_view2>::value, "Invalid metafunction is_byte_buffer_range");

// Performs several checks that ensure the valid state of the byte_buffer length.
// Note1: the byte_buffer::iterator operator- sums the lengths of the byte_buffer individual segments.
// Note2: std::distance counts the number of iterations between begin() and end().
#define ASSERT_EQ_LEN(buffer, len)                                                                                     \
  ASSERT_EQ(buffer.empty(), (len) == 0);                                                                               \
  ASSERT_EQ(buffer.length(), (len));                                                                                   \
  ASSERT_EQ(buffer.end() - buffer.begin(), (len));                                                                     \
  ASSERT_EQ(std::distance(buffer.begin(), buffer.end()), (len));

// Ensures commutativity of byte_buffer::operator==, and consistency when compared to std::equal(...).
#define ASSERT_EQ_BUFFER(buffer1, buffer2)                                                                             \
  ASSERT_EQ(buffer1, buffer2);                                                                                         \
  std::equal(buffer1.begin(), buffer1.end(), buffer2.begin(), buffer2.end());                                          \
  ASSERT_EQ(buffer2, buffer1)

///////////////////////// byte_buffer_test //////////////////////////////

TEST(byte_buffer2_test, empty_byte_buffer_in_valid_state)
{
  byte_buffer2 pdu;
  ASSERT_EQ_LEN(pdu, 0);
  ASSERT_EQ(pdu, std::vector<uint8_t>{});
  ASSERT_EQ(pdu, std::list<uint8_t>{}) << "Comparison with empty non-span type failed";
  ASSERT_EQ(pdu.segments().begin(), pdu.segments().end());
  ASSERT_TRUE(pdu.is_contiguous());
  pdu.resize(0);
  ASSERT_EQ_LEN(pdu, 0);
  pdu.clear();
  ASSERT_EQ_LEN(pdu, 0);
  ASSERT_EQ(pdu, pdu.deep_copy());
  ASSERT_EQ(pdu, pdu.copy());
  pdu.append(std::vector<uint8_t>{});
  ASSERT_EQ_LEN(pdu, 0);
  ASSERT_EQ(pdu.segments().begin(), pdu.segments().end());
}

///////////////////////// byte_buffer_view_test //////////////////////////////

TEST(byte_buffer_view2_test, empty_byte_buffer_view_is_in_valid_state)
{
  byte_buffer_view2 view;
  ASSERT_EQ_LEN(view, 0);
  ASSERT_EQ(view.begin(), view.end());
  ASSERT_EQ(view, byte_buffer_view2{});
  ASSERT_EQ(view.view(0, 0), view);
  ASSERT_EQ(view.segments().begin(), view.segments().end());
  ASSERT_EQ(view, std::vector<uint8_t>{});

  byte_buffer2 pdu;
  view = pdu;
  ASSERT_EQ_LEN(view, 0);
}
