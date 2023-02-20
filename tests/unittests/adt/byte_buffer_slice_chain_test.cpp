/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/byte_buffer_slice_chain.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(byte_buffer_slice_chain_test, empty_chain)
{
  byte_buffer_slice_chain buf;

  ASSERT_TRUE(buf.empty());
  ASSERT_EQ(0, buf.length());
  ASSERT_EQ(buf.begin(), buf.end());
  ASSERT_EQ(buf, std::vector<uint8_t>{});

  buf.push_back(byte_buffer{});
  buf.push_front(byte_buffer{});

  ASSERT_TRUE(buf.empty());
  ASSERT_EQ(buf.begin(), buf.end());

  byte_buffer buf_copy = buf.deep_copy();
  ASSERT_TRUE(buf_copy.empty());
  ASSERT_EQ(buf_copy.begin(), buf_copy.end());
}

TEST(byte_buffer_slice_chain_test, chain_payload)
{
  byte_buffer_slice_chain buf;
  ASSERT_TRUE(buf.empty());

  byte_buffer other_buffer{1, 2, 3, 4, 5};
  byte_buffer other_buffer2{6, 7, 8};
  byte_buffer buf_concat = other_buffer.deep_copy();
  buf_concat.append(other_buffer2.deep_copy());
  buf.push_back(other_buffer.copy());
  buf.push_back(other_buffer2.copy());

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

  // Test comparison.
  ASSERT_EQ(buf, buf_concat);

  // Test copy
  byte_buffer buf_copy = buf.deep_copy();
  ASSERT_EQ(buf_copy.length(), buf_concat.length());
  ASSERT_EQ(buf_copy, buf_concat);
}

TEST(byte_buffer_slice_chain_test, push_front)
{
  std::vector<uint8_t>    vec = {1, 2, 3};
  byte_buffer_slice_chain buf;
  byte_buffer             buf2{vec};

  ASSERT_TRUE(buf.empty());

  // Set header using a span of bytes.
  buf.push_front(buf2.deep_copy());
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
  buf.push_front(std::move(buf2));
  ASSERT_EQ(buf, vec);
  ASSERT_TRUE(buf2.empty());

  // Set header by ref-count increment, avoiding deep copy.
  buf.clear();
  buf2 = vec;
  buf.push_front(buf2.copy());
  ASSERT_EQ(buf, vec);
  *buf2.begin() = 5;
  ASSERT_NE(buf, vec);

  // Set header by deep copy.
  buf.clear();
  buf2 = vec;
  buf.push_front(buf2.deep_copy());
  ASSERT_EQ(buf, vec);
  *buf2.begin() = 5;
  ASSERT_EQ(buf, vec);
}

TEST(byte_buffer_slice_chain_test, chain_header_and_payload)
{
  byte_buffer_slice_chain buf;

  byte_buffer header_bytes = {1, 2, 3};
  byte_buffer payload      = {4, 5, 6};

  buf.push_front(header_bytes.deep_copy());
  ASSERT_EQ(header_bytes.length(), buf.length());
  buf.push_back(payload.copy());
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

TEST(byte_buffer_slice_chain_test, payload_lifetime)
{
  byte_buffer_slice_chain buf;

  std::vector<uint8_t> all_bytes;
  {
    byte_buffer header_bytes = {1, 2, 3};
    byte_buffer payload1     = {4, 5, 6};
    byte_buffer payload2     = {7, 8, 9};

    all_bytes.insert(all_bytes.end(), header_bytes.begin(), header_bytes.end());
    all_bytes.insert(all_bytes.end(), payload1.begin(), payload1.end());
    all_bytes.insert(all_bytes.end(), payload2.begin(), payload2.end());

    buf.push_front(header_bytes.copy());
    buf.push_back(std::move(payload1));
    buf.push_back(payload2.copy());
  }
  // Note: header and payload went out of scope, but that shouldnt affect the rlc buffer content.

  ASSERT_EQ(buf, all_bytes);
}

TEST(byte_buffer_slice_chain_test, slice_chain_formatter)
{
  byte_buffer          pdu, pdu2;
  std::vector<uint8_t> bytes = {1, 2, 3, 4, 15, 16, 255};
  pdu.append(bytes);
  pdu2.append(bytes);

  byte_buffer_slice_chain chain;
  chain.push_back(std::move(pdu), 3, 2);
  chain.push_back(std::move(pdu2), 0, 2);

  for (auto& b : chain) {
    ASSERT_TRUE(b > 0);
  }

  std::string result = fmt::format("{}", chain);
  ASSERT_EQ("04 0f 01 02", result);
}
