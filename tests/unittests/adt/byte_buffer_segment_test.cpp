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
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

std::random_device rd;
std::mt19937       g(rd());

static unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

/// Creates a small vector of bytes that fits in one segment.
static std::vector<uint8_t> make_small_vec()
{
  return {1, 2, 3, 4, 5, 6};
}

///////////////////////// byte_buffer_segment //////////////////////////////

TEST(buffer_segment_test, default_init_segment_is_empty_and_has_headroom)
{
  byte_buffer_segment segment;
  ASSERT_EQ(segment.begin(), segment.end());
  ASSERT_EQ(0, segment.length());
  ASSERT_EQ((size_t)byte_buffer_segment::SEGMENT_SIZE, segment.headroom() + segment.tailroom());
  ASSERT_EQ((size_t)byte_buffer_segment::DEFAULT_HEADROOM, segment.headroom());
  ASSERT_EQ(segment.next(), nullptr);
}

TEST(buffer_segment_test, append_bytes_on_empty_buffer)
{
  byte_buffer_segment segment;

  const std::vector<uint8_t> bytes = make_small_vec();
  segment.append(bytes);
  ASSERT_EQ(bytes.size(), segment.length());
  ASSERT_EQ(segment, bytes);
  ASSERT_TRUE(std::equal(segment.begin(), segment.end(), bytes.begin(), bytes.end()));
}

TEST(buffer_segment_test, prepend_bytes_on_empty_buffer)
{
  byte_buffer_segment segment;

  const std::vector<uint8_t> bytes = make_small_vec();
  segment.prepend(bytes);
  ASSERT_EQ(segment, bytes);
  ASSERT_TRUE(std::equal(segment.begin(), segment.end(), bytes.begin(), bytes.end()));
}

TEST(buffer_segment_test, multiple_appends)
{
  byte_buffer_segment  segment;
  std::vector<uint8_t> tot_bytes;

  for (unsigned i = 0; i != 5; ++i) {
    const std::vector<uint8_t> bytes = make_small_vec();
    tot_bytes.insert(tot_bytes.end(), bytes.begin(), bytes.end());
    segment.append(bytes);
  }
  ASSERT_TRUE(segment == tot_bytes);
}

TEST(buffer_segment_test, trim_head)
{
  byte_buffer_segment        segment;
  const std::vector<uint8_t> bytes = make_small_vec();
  segment.append(bytes);

  unsigned n = get_random_uint(1, 6);
  segment.trim_head(n);
  ASSERT_EQ(segment, span<const uint8_t>{bytes}.subspan(n, bytes.size() - n));
  ASSERT_TRUE(std::equal(segment.begin(), segment.end(), bytes.begin() + n, bytes.end()));
}

TEST(buffer_segment_test, trim_tail)
{
  byte_buffer_segment        segment;
  const std::vector<uint8_t> bytes = make_small_vec();
  segment.append(bytes);

  unsigned n = get_random_uint(1, 6);
  segment.trim_tail(n);
  ASSERT_EQ(segment, span<const uint8_t>{bytes}.subspan(0, bytes.size() - n));
  ASSERT_TRUE(std::equal(segment.begin(), segment.end(), bytes.begin(), bytes.end() - n));
}

TEST(buffer_segment_test, copy_ctor)
{
  byte_buffer_segment        segment;
  const std::vector<uint8_t> bytes = make_small_vec();
  segment.append(bytes);

  byte_buffer_segment segment2 = segment;
  ASSERT_EQ(segment2, segment);
  segment.trim_tail(1);
  segment.append(10);
  ASSERT_NE(segment, segment2) << "byte_buffer_segment copy ctor must make deep copy";
  ASSERT_TRUE(std::equal(segment2.begin(), segment2.end(), bytes.begin(), bytes.end()));
}

TEST(buffer_segment_test, move_ctor)
{
  byte_buffer_segment        segment;
  const std::vector<uint8_t> bytes = make_small_vec();
  segment.append(bytes);

  byte_buffer_segment segment2 = std::move(segment);
  segment.resize(0);
  segment.append(10);
  ASSERT_NE(segment, segment2) << "byte_buffer_segment move ctor must make deep copy";
  ASSERT_TRUE(std::equal(segment2.begin(), segment2.end(), bytes.begin(), bytes.end()));
}
