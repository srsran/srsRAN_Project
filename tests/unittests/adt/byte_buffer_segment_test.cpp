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

#include "srsran/adt/detail/byte_buffer_segment.h"
#include "srsran/adt/detail/byte_buffer_segment_list.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace detail;

///////////////////////// byte_buffer_segment //////////////////////////////

TEST(byte_buffer_segment_test, default_ctor)
{
  byte_buffer_segment segment;
  ASSERT_TRUE(segment.empty());
  ASSERT_EQ(segment.length(), 0);
  ASSERT_EQ(segment.capacity(), 0);
  ASSERT_EQ(segment.headroom(), 0);
  ASSERT_EQ(segment.tailroom(), 0);
  ASSERT_EQ(segment.begin(), segment.end());
  ASSERT_EQ(segment, byte_buffer_segment{});
}

TEST(byte_buffer_segment_test, segment_with_no_data)
{
  std::vector<uint8_t> buffer(1024);
  byte_buffer_segment  segment{buffer, 16}, segment2{buffer, 15};
  ASSERT_TRUE(segment.empty());
  ASSERT_EQ(0, segment.length());
  ASSERT_EQ(segment.capacity(), buffer.size());
  ASSERT_EQ(16, segment.headroom());
  ASSERT_EQ(segment.capacity(), segment.tailroom() + segment.headroom());
  ASSERT_EQ(segment.begin(), segment.end());
  ASSERT_EQ(segment, segment2)
      << "The fact that the segments have different headroom/tailroom space should not affect operator==";
}

TEST(byte_buffer_segment_test, append_bytes_on_empty_buffer)
{
  std::vector<uint8_t>       buffer(1024);
  byte_buffer_segment        segment{buffer, 16};
  const std::vector<uint8_t> bytes = test_rgen::random_vector<uint8_t>(6);

  segment.append(bytes);
  ASSERT_EQ(bytes.size(), segment.length());
  ASSERT_EQ(segment, bytes);
  ASSERT_TRUE(std::equal(segment.begin(), segment.end(), bytes.begin(), bytes.end()));
  ASSERT_EQ(segment[5], bytes[5]);
}

TEST(byte_buffer_segment_test, prepend_bytes_on_empty_buffer)
{
  std::vector<uint8_t> buffer(1024);
  byte_buffer_segment  segment{buffer, 16};

  const std::vector<uint8_t> bytes = test_rgen::random_vector<uint8_t>(6);
  segment.prepend(bytes);
  ASSERT_EQ(bytes.size(), segment.length());
  ASSERT_EQ(segment, bytes);
  ASSERT_TRUE(std::equal(segment.begin(), segment.end(), bytes.begin(), bytes.end()));
}

TEST(byte_buffer_segment_test, multiple_appends)
{
  std::vector<uint8_t> buffer(1024);
  byte_buffer_segment  segment{buffer, 16};
  std::vector<uint8_t> tot_bytes;

  for (unsigned i = 0; i != 5; ++i) {
    const std::vector<uint8_t> bytes = test_rgen::random_vector<uint8_t>(6);
    tot_bytes.insert(tot_bytes.end(), bytes.begin(), bytes.end());
    segment.append(bytes);
  }
  ASSERT_EQ(segment, tot_bytes);
}

TEST(byte_buffer_segment_test, trim_head)
{
  std::vector<uint8_t>       buffer(1024);
  byte_buffer_segment        segment{buffer, 16};
  const std::vector<uint8_t> bytes = test_rgen::random_vector<uint8_t>(6);
  segment.append(bytes);

  const unsigned n = test_rgen::uniform_int<unsigned>(1, bytes.size());
  segment.trim_head(n);
  ASSERT_EQ(segment, span<const uint8_t>{bytes}.subspan(n, bytes.size() - n));
  ASSERT_TRUE(std::equal(segment.begin(), segment.end(), bytes.begin() + n, bytes.end()));
}

TEST(byte_buffer_segment_test, trim_tail)
{
  std::vector<uint8_t>       buffer(1024);
  byte_buffer_segment        segment{buffer, 16};
  const std::vector<uint8_t> bytes = test_rgen::random_vector<uint8_t>(6);
  segment.append(bytes);

  const unsigned n = test_rgen::uniform_int<unsigned>(1, bytes.size());
  segment.trim_tail(n);
  ASSERT_EQ(segment, span<const uint8_t>{bytes}.subspan(0, bytes.size() - n));
  ASSERT_TRUE(std::equal(segment.begin(), segment.end(), bytes.begin(), bytes.end() - n));
}

TEST(byte_buffer_segment_test, copy_ctor)
{
  std::vector<uint8_t>       buffer(1024);
  byte_buffer_segment        segment{buffer, 16};
  const std::vector<uint8_t> bytes = test_rgen::random_vector<uint8_t>(6);
  segment.append(bytes);

  byte_buffer_segment segment2 = segment;
  ASSERT_EQ(segment2, segment);
  ASSERT_TRUE(std::equal(segment2.begin(), segment2.end(), bytes.begin(), bytes.end()));

  segment.trim_tail(1);
  ASSERT_NE(segment.length(), segment2.length());
  ASSERT_NE(segment, segment2);
  segment.append(10);
  ASSERT_EQ(segment, segment2);
  segment[3] = 240;
  ASSERT_EQ(segment, segment2);
}

TEST(byte_buffer_segment_test, move_ctor)
{
  std::vector<uint8_t>       buffer(1024);
  byte_buffer_segment        segment{buffer, 32};
  const std::vector<uint8_t> bytes = test_rgen::random_vector<uint8_t>(6);
  segment.append(bytes);

  byte_buffer_segment segment2 = std::move(segment);
  ASSERT_EQ(segment, segment2);
  ASSERT_TRUE(std::equal(segment2.begin(), segment2.end(), bytes.begin(), bytes.end()));
}

TEST(byte_buffer_segment_list, default_ctor)
{
  detail::byte_buffer_segment_list list;

  ASSERT_TRUE(list.empty());
}

TEST(byte_buffer_segment_list, push_back)
{
  detail::byte_buffer_segment_list         list;
  std::vector<uint8_t>                     buffer(128), buffer2(128);
  detail::byte_buffer_segment_list::node_t node{buffer, 32}, node2{buffer2, 32};

  list.push_back(node);
  ASSERT_FALSE(list.empty());
  ASSERT_EQ(&node, &list.front());
  ASSERT_EQ(&node, &list.back());

  list.push_back(node2);
  ASSERT_FALSE(list.empty());
  ASSERT_EQ(&node, &list.front());
  ASSERT_EQ(&node2, &list.back());
}

TEST(byte_buffer_segment_list, push_front)
{
  detail::byte_buffer_segment_list         list;
  std::vector<uint8_t>                     buffer(128), buffer2(128);
  detail::byte_buffer_segment_list::node_t node{buffer, 32}, node2{buffer2, 32};

  list.push_front(node);
  ASSERT_FALSE(list.empty());
  ASSERT_EQ(&node, &list.front());
  ASSERT_EQ(&node, &list.back());

  list.push_front(node2);
  ASSERT_FALSE(list.empty());
  ASSERT_EQ(&node2, &list.front());
  ASSERT_EQ(&node, &list.back());
}

TEST(byte_buffer_segment_list, pop_back)
{
  detail::byte_buffer_segment_list         list;
  std::vector<uint8_t>                     buffer(128), buffer2(128);
  detail::byte_buffer_segment_list::node_t node{buffer, 32}, node2{buffer2, 32};
  list.push_back(node);
  list.push_back(node2);

  list.pop_back();
  ASSERT_FALSE(list.empty());
  ASSERT_EQ(&node, &list.front());
  ASSERT_EQ(&node, &list.back());

  list.pop_back();
  ASSERT_TRUE(list.empty());
}

TEST(byte_buffer_segment_list, pop_front)
{
  detail::byte_buffer_segment_list         list;
  std::vector<uint8_t>                     buffer(128), buffer2(128);
  detail::byte_buffer_segment_list::node_t node{buffer, 32}, node2{buffer2, 32};
  list.push_back(node);
  list.push_back(node2);

  list.pop_front();
  ASSERT_FALSE(list.empty());
  ASSERT_EQ(&node2, &list.front());
  ASSERT_EQ(&node2, &list.back());

  list.pop_front();
  ASSERT_TRUE(list.empty());
}

TEST(byte_buffer_segment_list_span_iterator, multi_segment_comparison)
{
  detail::byte_buffer_segment_list         list;
  std::vector<uint8_t>                     buffer(128), buffer2(128);
  detail::byte_buffer_segment_list::node_t node{buffer, 32}, node2{buffer2, 32};
  list.push_back(node);
  list.push_back(node2);
  std::vector<uint8_t> bytes1 = test_rgen::random_vector<uint8_t>(32);
  std::vector<uint8_t> bytes2 = test_rgen::random_vector<uint8_t>(32);
  node.append(bytes1);
  node2.append(bytes2);

  unsigned start_idx = test_rgen::uniform_int<unsigned>(0, 5);
  unsigned length    = bytes1.size() + bytes2.size() - start_idx - test_rgen::uniform_int<unsigned>(0, 5);
  byte_buffer_segment_span_range range{&list.front(), start_idx, length};

  auto it    = range.begin();
  auto span1 = span<uint8_t>{bytes1}.subspan(start_idx, bytes1.size() - start_idx);
  ASSERT_EQ(*it, span1);
  ++it;
  auto span2 = span<uint8_t>{bytes2}.subspan(0, length - span1.size());
  ASSERT_EQ(*it, span2);
  ++it;
  ASSERT_EQ(it, range.end());
}
