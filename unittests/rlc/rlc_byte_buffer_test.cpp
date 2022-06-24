/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/rlc/rlc_byte_buffer.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_empty_rlc_buffer()
{
  rlc_byte_buffer buf;

  TESTASSERT(buf.empty());
  TESTASSERT_EQ(0, buf.length());
  TESTASSERT(buf.begin() == buf.end());
  TESTASSERT(buf == std::vector<uint8_t>{});

  buf = rlc_byte_buffer{byte_buffer{}};

  TESTASSERT(buf.empty());
  TESTASSERT(buf.begin() == buf.end());

  buf.set_payload(byte_buffer{});

  TESTASSERT(buf.empty());
  TESTASSERT(buf.begin() == buf.end());
}

void test_rlc_buffer_payload_assignment()
{
  rlc_byte_buffer buf;
  TESTASSERT(buf.empty());

  byte_buffer other_buffer{1, 2, 3, 4, 5};
  buf.set_payload(other_buffer);

  // Test length/empty methods.
  TESTASSERT(not buf.empty());
  TESTASSERT_EQ(other_buffer.length(), buf.length());

  // Test comparison.
  TESTASSERT(buf == other_buffer);

  // Test operator[].
  for (unsigned i = 0; i < other_buffer.length(); ++i) {
    TESTASSERT_EQ(other_buffer[i], buf[i]);
  }

  // Test iterator.
  TESTASSERT(buf.begin() != buf.end());
  unsigned count = 0;
  for (uint8_t v : buf) {
    TESTASSERT_EQ(other_buffer[count++], v);
  }
  TESTASSERT_EQ(other_buffer.length(), count);
}

void test_rlc_buffer_prepend()
{
  rlc_byte_buffer buf;
  TESTASSERT(buf.empty());

  std::vector<uint8_t> bytes = {1, 2, 3};
  buf.prepend_header(bytes);

  // Test length/empty methods.
  TESTASSERT(not buf.empty());
  TESTASSERT_EQ(3, buf.length());

  // Test comparison.
  TESTASSERT(buf == bytes);

  // Test operator[].
  for (unsigned i = 0; i < bytes.size(); ++i) {
    TESTASSERT_EQ(bytes[i], buf[i]);
  }

  // Test iterator.
  TESTASSERT(buf.begin() != buf.end());
  unsigned count = 0;
  for (uint8_t v : buf) {
    TESTASSERT_EQ(bytes[count++], v);
  }
  TESTASSERT_EQ(bytes.size(), count);
}

void test_rlc_buffer_header_and_payload()
{
  rlc_byte_buffer buf;

  std::vector<uint8_t> header_bytes = {1, 2, 3};
  byte_buffer          payload      = {4, 5, 6};

  buf.set_header(header_bytes);
  TESTASSERT_EQ(header_bytes.size(), buf.length());
  buf.set_payload(payload);
  TESTASSERT_EQ(header_bytes.size() + payload.length(), buf.length());

  // Test comparison.
  std::vector<uint8_t> all_bytes = header_bytes;
  all_bytes.insert(all_bytes.end(), payload.begin(), payload.end());
  TESTASSERT(buf != payload);
  TESTASSERT(buf == all_bytes);
  TESTASSERT(buf.header_view() == header_bytes);
  TESTASSERT(buf.payload_view() == payload);

  // Test operator[].
  for (unsigned i = 0; i < all_bytes.size(); ++i) {
    TESTASSERT_EQ(all_bytes[i], buf[i]);
  }

  // Test iterator.
  TESTASSERT(buf.begin() != buf.end());
  unsigned count = 0;
  for (uint8_t v : buf) {
    TESTASSERT_EQ(all_bytes[count++], v);
  }
  TESTASSERT_EQ(all_bytes.size(), count);
}

void test_rlc_buffer_payload_lifetime()
{
  rlc_byte_buffer buf;

  std::vector<uint8_t> all_bytes;
  {
    std::vector<uint8_t> header_bytes = {1, 2, 3};
    byte_buffer          payload      = {4, 5, 6};

    buf.set_header(header_bytes);
    buf.set_payload(payload);

    all_bytes = header_bytes;
    all_bytes.insert(all_bytes.end(), payload.begin(), payload.end());
  }
  // Note: header and payload went out of scope, but that shouldnt affect the rlc buffer content.

  TESTASSERT(not buf.payload_view().empty());
  TESTASSERT(buf == all_bytes);
}

int main()
{
  test_empty_rlc_buffer();
  test_rlc_buffer_payload_assignment();
  test_rlc_buffer_prepend();
  test_rlc_buffer_header_and_payload();
  test_rlc_buffer_payload_lifetime();
}