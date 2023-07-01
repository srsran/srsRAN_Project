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

using namespace srsran;

static_assert(std::is_same<byte_buffer_view2::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer_view2::iterator::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer_view2::const_iterator::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer_view2::iterator::reference, uint8_t&>::value, "Invalid reference type");
static_assert(std::is_same<byte_buffer_view2::const_iterator::reference, const uint8_t&>::value,
              "Invalid reference type");
static_assert(std::is_same<byte_buffer_view2::const_iterator::pointer, const uint8_t*>::value, "Invalid pointer type");
static_assert(is_byte_buffer_range<byte_buffer_view2>::value, "Invalid metafunction is_byte_buffer_range");

TEST(byte_buffer_view2, default_ctor)
{
  byte_buffer_view2 view;
  ASSERT_TRUE(view.empty());
  ASSERT_EQ(view.length(), 0);
  ASSERT_EQ(view.begin(), view.end());
  ASSERT_EQ(view, byte_buffer_view2{});
  ASSERT_EQ(view.view(0, 0), view);
  ASSERT_EQ(view.segments().begin(), view.segments().end());
}