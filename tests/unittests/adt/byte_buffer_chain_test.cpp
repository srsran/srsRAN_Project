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

#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

static_assert(std::is_same<byte_buffer_chain::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer_chain::iterator::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer_chain::const_iterator::value_type, uint8_t>::value, "Invalid valid_type");

TEST(byte_buffer_chain_test, empty_container_in_valid_state)
{
  byte_buffer_chain chain;
  ASSERT_TRUE(chain.empty());
  ASSERT_EQ(chain.length(), 0U);
  ASSERT_EQ(chain.nof_slices(), 0);
  ASSERT_TRUE(chain.slices().empty());
  ASSERT_EQ(chain.begin(), chain.end());
  ASSERT_EQ(std::distance(chain.begin(), chain.end()), 0);
  ASSERT_EQ(chain, std::vector<uint8_t>{});

  chain.clear();
  ASSERT_EQ(chain.length(), 0U);
  ASSERT_EQ(chain.begin(), chain.end());

  chain.append(byte_buffer{});
  ASSERT_EQ(chain.length(), 0U);
  ASSERT_EQ(chain.begin(), chain.end());

  chain.prepend(byte_buffer{});
  ASSERT_EQ(chain.length(), 0U);
  ASSERT_EQ(chain.begin(), chain.end());
  ASSERT_TRUE(chain.slices().empty());
}

TEST(byte_buffer_chain_test, append_byte_buffer)
{
  byte_buffer_chain buf;

  byte_buffer other_buffer{1, 2, 3, 4, 5};
  byte_buffer other_buffer2{6, 7, 8};
  byte_buffer buf_concat = other_buffer.deep_copy();
  buf_concat.append(other_buffer2.deep_copy());
  buf.append(other_buffer.copy());
  buf.append(other_buffer2.copy());

  // Test length/empty methods.
  ASSERT_FALSE(buf.empty());
  ASSERT_EQ(buf_concat.length(), buf.length());

  // Test operator[].
  for (unsigned i = 0; i < buf_concat.length(); ++i) {
    ASSERT_EQ(buf_concat[i], buf[i]);
  }

  // Test iterator.
  ASSERT_NE(buf.begin(), buf.end());
  unsigned count = 0;
  for (uint8_t v : buf) {
    ASSERT_EQ(buf_concat[(count++)], v);
  }
  ASSERT_EQ(buf_concat.length(), count);
  ASSERT_EQ(std::distance(buf.begin(), buf.end()), buf.length());

  // Test comparison.
  ASSERT_EQ(buf, buf_concat);

  // Test copy
  byte_buffer buf_copy = buf.deep_copy();
  ASSERT_EQ(buf_copy.length(), buf_concat.length());
  ASSERT_EQ(buf_copy, buf_concat);
}

TEST(byte_buffer_chain_test, prepend_buffer)
{
  std::vector<uint8_t> vec = {1, 2, 3};
  byte_buffer_chain    buf;
  byte_buffer          buf2{vec};

  ASSERT_TRUE(buf.empty());

  // Set header using a span of bytes.
  buf.prepend(buf2.deep_copy());
  ASSERT_FALSE(buf.empty());
  ASSERT_EQ(3, buf.length());
  ASSERT_EQ(buf, vec);
  for (unsigned i = 0; i < vec.size(); ++i) {
    ASSERT_EQ(vec[i], buf[i]);
  }
  ASSERT_NE(buf.begin(), buf.end());
  unsigned count = 0;
  for (uint8_t v : buf) {
    ASSERT_EQ(vec[count++], v);
  }
  ASSERT_EQ(vec.size(), count);

  // Set header avoiding ref-count increment and avoiding deep copy.
  buf.clear();
  buf.prepend(std::move(buf2));
  ASSERT_EQ(buf, vec);
  ASSERT_TRUE(buf2.empty());

  // Set header by ref-count increment, avoiding deep copy.
  buf.clear();
  buf2 = vec;
  buf.prepend(buf2.copy());
  ASSERT_EQ(buf, vec);
  *buf2.begin() = 5;
  ASSERT_NE(buf, vec);

  // Set header by deep copy.
  buf.clear();
  buf2 = vec;
  buf.prepend(buf2.deep_copy());
  ASSERT_EQ(buf, vec);
  *buf2.begin() = 5;
  ASSERT_EQ(buf, vec);
}

TEST(byte_buffer_chain_test, prepend_header_and_append_payload)
{
  byte_buffer_chain buf;

  byte_buffer header_bytes = {1, 2, 3};
  byte_buffer payload      = {4, 5, 6};

  buf.prepend(header_bytes.deep_copy());
  ASSERT_EQ(header_bytes.length(), buf.length());
  buf.append(payload.copy());
  ASSERT_EQ(header_bytes.length() + payload.length(), buf.length());

  // Test comparison.
  std::vector<uint8_t> all_bytes(header_bytes.begin(), header_bytes.end());
  all_bytes.insert(all_bytes.end(), payload.begin(), payload.end());
  ASSERT_NE(buf, payload);
  ASSERT_EQ(buf, all_bytes);

  // Test operator[].
  for (unsigned i = 0; i < all_bytes.size(); ++i) {
    ASSERT_EQ(all_bytes[i], buf[i]);
  }

  // Test iterator.
  ASSERT_NE(buf.begin(), buf.end());
  unsigned count = 0;
  for (uint8_t v : buf) {
    ASSERT_EQ(all_bytes[count++], v);
  }
  ASSERT_EQ(all_bytes.size(), count);
}

TEST(byte_buffer_chain_test, payload_lifetime)
{
  byte_buffer_chain buf;

  std::vector<uint8_t> all_bytes;
  {
    byte_buffer header_bytes = {1, 2, 3};
    byte_buffer payload1     = {4, 5, 6};
    byte_buffer payload2     = {7, 8, 9};

    all_bytes.insert(all_bytes.end(), header_bytes.begin(), header_bytes.end());
    all_bytes.insert(all_bytes.end(), payload1.begin(), payload1.end());
    all_bytes.insert(all_bytes.end(), payload2.begin(), payload2.end());

    buf.prepend(header_bytes.copy());
    buf.append(std::move(payload1));
    buf.append(payload2.copy());
  }
  // Note: header and payload went out of scope, but that shouldnt affect the rlc buffer content.

  ASSERT_EQ(buf, all_bytes);
}

TEST(byte_buffer_chain_test, slice_chain_formatter)
{
  byte_buffer          pdu, pdu2;
  std::vector<uint8_t> bytes = {1, 2, 3, 4, 15, 16, 255};
  pdu.append(bytes);
  pdu2.append(bytes);

  byte_buffer_chain chain;
  chain.append(byte_buffer_slice{std::move(pdu), 3, 2});
  chain.append(byte_buffer_slice{std::move(pdu2), 0, 2});

  for (auto& b : chain) {
    ASSERT_TRUE(b > 0);
  }

  std::string result = fmt::format("{}", chain);
  ASSERT_EQ("04 0f 01 02", result);
}
