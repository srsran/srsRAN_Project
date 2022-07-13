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
#include <list>

using namespace srsgnb;

static_assert(std::is_same<byte_buffer::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer::iterator::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer::const_iterator::value_type, uint8_t>::value, "Invalid valid_type");
static_assert(std::is_same<byte_buffer::iterator::reference, uint8_t&>::value, "Invalid reference type");
static_assert(std::is_same<byte_buffer::const_iterator::reference, const uint8_t&>::value, "Invalid reference type");
static_assert(std::is_same<byte_buffer::const_iterator::pointer, const uint8_t*>::value, "Invalid pointer type");

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

void test_buffer_segment()
{
  constexpr static size_t segment_size          = byte_buffer_segment::SEGMENT_SIZE;
  constexpr static size_t segment_headroom_init = byte_buffer_segment::DEFAULT_HEADROOM;
  byte_buffer_segment     segment;
  TESTASSERT(segment.begin() == segment.end());
  TESTASSERT_EQ(0, segment.length());
  TESTASSERT_EQ(segment_size, segment.headroom() + segment.tailroom());
  TESTASSERT_EQ(segment_headroom_init, segment.headroom());
  TESTASSERT(segment.next() == nullptr);

  // append
  auto bytes = make_small_vec();
  segment.append(bytes);
  TESTASSERT(segment == bytes);
  TESTASSERT(std::equal(segment.begin(), segment.end(), bytes.begin(), bytes.end()));

  // prepend
  segment.prepend(bytes);
  auto bytes2 = bytes;
  bytes2.insert(bytes2.end(), bytes.begin(), bytes.end());
  TESTASSERT(segment == bytes2);
  TESTASSERT(std::equal(segment.begin(), segment.end(), bytes2.begin(), bytes2.end()));

  // trim front
  segment.trim_head(4);
  TESTASSERT(segment == span<uint8_t>{bytes2}.subspan(4, bytes2.size() - 4));
  TESTASSERT(std::equal(segment.begin(), segment.end(), bytes2.begin() + 4, bytes2.end()));

  // trim back
  segment.trim_tail(2);
  TESTASSERT(segment == span<uint8_t>{bytes2}.subspan(4, bytes2.size() - 6));
  TESTASSERT(std::equal(segment.begin(), segment.end(), bytes2.begin() + 4, bytes2.end() - 2));

  // comparison
  byte_buffer_segment segment2 = segment;
  TESTASSERT(segment == segment2);
  segment2.trim_tail(1);
  TESTASSERT(segment != segment2);
}

void test_byte_buffer_append()
{
  byte_buffer pdu;
  TESTASSERT(pdu.empty());
  TESTASSERT_EQ(0, pdu.length());

  std::vector<uint8_t> bytes = make_small_vec();
  pdu.append(bytes);
  TESTASSERT_EQ(pdu.length(), bytes.size());

  // create a new segment during the append.
  auto bytes2 = make_big_vec();
  pdu.append(bytes2);
  TESTASSERT_EQ(pdu.length(), bytes2.size() + bytes.size());

  // append two byte_buffers
  byte_buffer pdu2{bytes};
  TESTASSERT_EQ(pdu2.length(), bytes.size());
  pdu2.append(pdu);
  TESTASSERT_EQ(pdu.length() + bytes.size(), pdu2.length());
}

void test_byte_buffer_prepend()
{
  byte_buffer          pdu;
  std::vector<uint8_t> bytes = make_small_vec();

  pdu.prepend(bytes);
  TESTASSERT_EQ(pdu.length(), bytes.size());
  TESTASSERT(pdu == bytes);

  auto bytes2 = make_big_vec();
  pdu.prepend(bytes2);
  TESTASSERT_EQ(pdu.length(), bytes.size() + bytes2.size());

  auto bytes_concat = bytes2;
  bytes_concat.insert(bytes_concat.end(), bytes.begin(), bytes.end());
  TESTASSERT(bytes_concat == pdu);
}

void test_byte_buffer_clear()
{
  byte_buffer pdu;
  pdu.append(make_small_vec());

  TESTASSERT(not pdu.empty());
  TESTASSERT(pdu.length() > 0);
  pdu.clear();
  TESTASSERT(pdu.empty());
  TESTASSERT(pdu.length() == 0);

  pdu.append(make_big_vec());
  pdu.append(make_big_vec());

  TESTASSERT(not pdu.empty());
  pdu.clear();
  TESTASSERT(pdu.empty());
}

void test_byte_buffer_compare()
{
  byte_buffer          pdu, pdu2, pdu3, pdu4;
  std::vector<uint8_t> bytes  = {1, 2, 3, 4, 5, 6};
  std::vector<uint8_t> bytes2 = {1, 2, 3, 4, 5};
  std::vector<uint8_t> bytes3 = {2, 2, 3, 4, 5, 6};

  pdu.append(bytes);

  TESTASSERT(pdu == bytes);
  TESTASSERT(bytes == pdu);
  TESTASSERT(bytes2 != pdu);
  TESTASSERT(bytes3 != pdu);

  pdu2.append(bytes);
  pdu3.append(bytes2);
  pdu4.append(bytes3);

  TESTASSERT(pdu == pdu2);
  TESTASSERT(pdu != pdu3);
  TESTASSERT(pdu != pdu4);
  TESTASSERT(pdu2 != pdu4);
  TESTASSERT(pdu3 != pdu4);

  // create a new segment during the append.
  bytes2.resize(byte_buffer_segment::SEGMENT_SIZE);
  for (size_t i = 0; i < bytes2.size(); ++i) {
    bytes2[i] = i;
  }
  pdu.append(bytes2);
  bytes.insert(bytes.end(), bytes2.begin(), bytes2.end());
  TESTASSERT(pdu == bytes);

  // create a new segment during the prepend.
  pdu.prepend(bytes2);
  bytes.insert(bytes.begin(), bytes2.begin(), bytes2.end());
  TESTASSERT(pdu == bytes);
}

void test_byte_buffer_iterator()
{
  byte_buffer pdu;

  std::vector<uint8_t> bytes = make_small_vec();
  pdu.append(bytes);

  // iterator
  size_t i = 0;
  for (byte_buffer::iterator it = pdu.begin(); it != pdu.end(); ++it, ++i) {
    TESTASSERT_EQ(bytes[i], *it);
  }
  TESTASSERT_EQ(bytes.size(), i);

  // const iterator
  i = 0;
  for (byte_buffer::const_iterator it = pdu.cbegin(); it != pdu.cend(); ++it, ++i) {
    TESTASSERT_EQ(bytes[i], *it);
  }
  TESTASSERT_EQ(bytes.size(), i);

  // distance
  TESTASSERT_EQ(bytes.size(), pdu.length());
  TESTASSERT_EQ(bytes.size(), (size_t)(pdu.end() - pdu.begin()));
  TESTASSERT_EQ(bytes.size() - 2, (size_t)(pdu.end() - (pdu.begin() + 2)));

  // multiple segments
  std::vector<uint8_t> bytes2 = make_big_vec();
  pdu.append(bytes2);
  std::vector<uint8_t> bytes_concat = bytes;
  bytes_concat.insert(bytes_concat.end(), bytes2.begin(), bytes2.end());

  // iterator
  i = 0;
  for (byte_buffer::iterator it = pdu.begin(); it != pdu.end(); ++it, ++i) {
    TESTASSERT_EQ(bytes_concat[i], *it);
  }
  TESTASSERT_EQ(bytes_concat.size(), i);

  // const iterator
  i = 0;
  for (byte_buffer::const_iterator it = pdu.cbegin(); it != pdu.cend(); ++it, ++i) {
    TESTASSERT_EQ(bytes_concat[i], *it);
  }
  TESTASSERT_EQ(bytes_concat.size(), i);

  // distance
  TESTASSERT_EQ(bytes_concat.size(), pdu.length());
  TESTASSERT_EQ(bytes_concat.size(), (size_t)(pdu.end() - pdu.begin()));
  TESTASSERT_EQ(bytes_concat.size() - 2, (size_t)(pdu.end() - (pdu.begin() + 2)));
}

void test_byte_buffer_copy()
{
  byte_buffer pdu;

  std::vector<uint8_t> bytes = make_small_vec(), bytes2 = make_big_vec();
  auto                 bytes_concat = bytes;
  bytes_concat.insert(bytes_concat.end(), bytes2.begin(), bytes2.end());
  pdu.append(bytes);

  // Deep copy.
  byte_buffer pdu2;
  pdu2 = pdu.deep_copy();
  TESTASSERT(not pdu2.empty() and not pdu.empty());
  TESTASSERT_EQ(pdu.length(), pdu2.length());
  TESTASSERT(pdu == pdu2);
  TESTASSERT(pdu2 == bytes);

  pdu2.append(bytes2);
  TESTASSERT(pdu == bytes);
  TESTASSERT(pdu != pdu2);
  TESTASSERT_EQ(bytes_concat.size(), pdu2.length());
  TESTASSERT(pdu2 == bytes_concat);

  // Shallow copy
  {
    byte_buffer pdu3 = pdu.copy();
    TESTASSERT(pdu3 == pdu);
    TESTASSERT(pdu3 == bytes);
    pdu3.append(bytes2);
    TESTASSERT(pdu3 == pdu);
    TESTASSERT(pdu == bytes_concat);
  }
  TESTASSERT(pdu == bytes_concat);
}

void test_byte_buffer_move()
{
  byte_buffer          pdu;
  std::vector<uint8_t> bytes = {1, 2, 3, 4, 5, 6};
  pdu.append(bytes);

  byte_buffer pdu2{std::move(pdu)};
  TESTASSERT(not pdu2.empty() and pdu.empty());
  TESTASSERT(pdu2 == bytes);
}

void test_byte_buffer_formatter()
{
  byte_buffer          pdu;
  std::vector<uint8_t> bytes = {1, 2, 3, 4, 15, 16, 255};
  pdu.append(bytes);

  fmt::print("PDU: {}\n", pdu);
  std::string result = fmt::format("{}", pdu);
  TESTASSERT(result == "01 02 03 04 0f 10 ff");
}

void test_byte_buffer_view()
{
  byte_buffer          pdu;
  std::vector<uint8_t> bytes = make_small_vec();
  pdu.append(bytes);

  byte_buffer_view view = pdu;

  TESTASSERT(not view.empty());
  TESTASSERT_EQ(6, view.length());
  TESTASSERT_EQ(6, view.end() - view.begin());
  TESTASSERT_EQ(4, view.view(0, 4).length());
  TESTASSERT_EQ(4, view.view(2, 4).length());
}

void test_byte_buffer_trim()
{
  byte_buffer          pdu;
  std::vector<uint8_t> bytes = make_small_vec();
  pdu.append(bytes);

  TESTASSERT(pdu == bytes);
  pdu.trim_head(2);
  TESTASSERT(pdu == span<const uint8_t>{bytes}.subspan(2, bytes.size() - 2));

  std::vector<uint8_t> bytes2 = make_big_vec();
  pdu.append(bytes2);

  auto bytes_concat = bytes;
  bytes_concat.insert(bytes_concat.end(), bytes2.begin(), bytes2.end());
  TESTASSERT(pdu == span<const uint8_t>{bytes_concat}.subspan(2, bytes_concat.size() - 2));

  pdu.trim_head(bytes.size() - 2);
  TESTASSERT(pdu == bytes2);

  // should make the first segment empty and remove it.
  pdu.trim_head(bytes2.size() - 2);
  TESTASSERT(pdu == span<const uint8_t>{bytes2}.last(2));
}

void test_byte_buffer_linearize()
{
  byte_buffer          pdu;
  std::vector<uint8_t> bytes        = make_small_vec();
  std::vector<uint8_t> bytes2       = make_big_vec();
  auto                 bytes_concat = bytes;
  bytes_concat.insert(bytes_concat.end(), bytes2.begin(), bytes2.end());

  pdu.append(bytes);
  TESTASSERT(pdu.is_contiguous());
  pdu.append(bytes2);
  TESTASSERT(not pdu.is_contiguous());

  TESTASSERT(pdu == bytes_concat);
  TESTASSERT(pdu.linearize() < 0);
  TESTASSERT(pdu == bytes_concat);

  TESTASSERT(pdu.trim_tail(bytes.size() - 1) == 0);
  TESTASSERT(not pdu.is_contiguous());
  TESTASSERT(pdu.linearize() < 0);

  TESTASSERT(pdu.trim_tail(1) == 0);
  TESTASSERT(not pdu.is_contiguous());
  TESTASSERT(pdu.linearize() == 0);
  TESTASSERT(pdu.is_contiguous());
}

void test_byte_buffer_initializer_list()
{
  byte_buffer pdu = {1, 2, 3, 4, 5, 6};
  TESTASSERT_EQ(6, pdu.length());

  bool are_equal = pdu == std::vector<uint8_t>{1, 2, 3, 4, 5, 6};
  TESTASSERT(are_equal);
}

void test_byte_buffer_from_hexdump()
{
  std::vector<uint8_t> bytes{0x1, 0x2, 0x3, 0x4, 0x5, 0xff};
  byte_buffer          pdu = make_byte_buffer("0102030405FF");
  TESTASSERT(pdu == bytes);
}

void test_byte_buffer_iterator_plus_equal_op()
{
  // Test with small vector of bytes
  // Make initial vector
  byte_buffer          pdu;
  std::vector<uint8_t> bytes = make_small_vec();
  pdu.append(bytes);

  auto it = pdu.begin();
  TESTASSERT(it != pdu.cend());

  // Point reader at 1 byte before the end
  it += (pdu.length() - 1);
  TESTASSERT(it != pdu.cend());

  // Point reader at the end
  it = pdu.begin();
  it += pdu.length();
  TESTASSERT(it == pdu.cend());

  // Test with big vector of bytes
  bytes = make_big_vec();
  pdu.clear();
  pdu.append(bytes);

  it = pdu.begin();
  TESTASSERT(it != pdu.cend());

  // Point reader at 1 byte before the end
  it += (pdu.length() - 1);
  TESTASSERT(pdu.cend() != it);

  // Point reader at the end
  it = pdu.begin();
  it += pdu.length();
  TESTASSERT(it == pdu.cend());

  // Test with byte buffer made of several segments
  pdu.append(bytes);
  pdu.append(bytes);

  it = pdu.begin();
  TESTASSERT(pdu.cend() != it);

  // Point reader at 1 byte before the end
  it += (pdu.length() - 1);
  TESTASSERT(it != pdu.cend());

  // Point reader at the end
  it = pdu.begin();
  it += pdu.length();
  TESTASSERT(it == pdu.cend());
}

void test_byte_buffer_reader_split_advance()
{
  // Test with small vector of bytes
  // Make initial vector
  byte_buffer          pdu;
  std::vector<uint8_t> bytes = make_small_vec();
  pdu.append(bytes);

  auto it = pdu.begin();
  TESTASSERT(it != pdu.cend());

  // Get reader and check that beginning and end coincide with the original
  byte_buffer_reader pdu_reader{byte_buffer_view{pdu}};
  TESTASSERT(pdu_reader.begin() == pdu.begin());
  TESTASSERT(pdu_reader.end() == pdu.end());

  // Stop reader at 1 byte to the end
  auto view = pdu_reader.split_and_advance(pdu.length() - 1);
  TESTASSERT(pdu_reader.begin() != pdu_reader.end());
  TESTASSERT(pdu_reader.end() == pdu.end());
  TESTASSERT(view.begin() == pdu.begin());
  TESTASSERT(view.end() == pdu_reader.begin());

  // Move reader 1 byte ahead, to the end
  view = pdu_reader.split_and_advance(1);
  TESTASSERT(pdu_reader.begin() == pdu_reader.end());
  TESTASSERT(view.end() == pdu_reader.begin());

  // Test with byte buffer made of several segments
  // Make initial vector
  bytes = make_big_vec();
  pdu.clear();
  pdu.append(bytes);
  pdu.append(bytes);
  pdu.append(bytes);

  it = pdu.begin();
  TESTASSERT(it != pdu.cend());

  // Get reader and check that beginning and end coincide with the original
  byte_buffer_reader pdu_long_reader{byte_buffer_view{pdu}};
  TESTASSERT(pdu_long_reader.begin() == pdu.begin());
  TESTASSERT(pdu_long_reader.end() == pdu.end());

  // Stop reader at 1 byte to the end
  view = pdu_long_reader.split_and_advance(pdu.length() - 1);
  TESTASSERT(pdu_long_reader.begin() != pdu_long_reader.end());
  TESTASSERT(pdu_long_reader.end() == pdu.end());
  TESTASSERT(view.begin() == pdu.begin());
  TESTASSERT(view.end() == pdu_long_reader.begin());

  // Move reader 1 byte ahead, to the end
  view = pdu_long_reader.split_and_advance(1);
  TESTASSERT(pdu_long_reader.begin() == pdu_long_reader.end());
  TESTASSERT(view.end() == pdu_long_reader.begin());
}

void test_byte_buffer_writer()
{
  byte_buffer        pdu;
  byte_buffer_writer writer{pdu};

  TESTASSERT(pdu.empty());
  TESTASSERT(writer.empty());

  writer.append(5);
  TESTASSERT(not pdu.empty());
  TESTASSERT(not writer.empty());
  TESTASSERT_EQ(1, pdu.length());
  TESTASSERT_EQ(1, writer.length());

  writer.append({0, 1, 2, 3, 4});
  TESTASSERT_EQ(6, pdu.length());
  TESTASSERT_EQ(6, writer.length());
  bool is_eq = pdu == std::vector<uint8_t>{5, 0, 1, 2, 3, 4};
  TESTASSERT(is_eq);

  TESTASSERT_EQ(4, writer.back());
  writer.back() += 6;
  TESTASSERT_EQ(10, writer.back());
}

void test_byte_buffer_reserve_prepend()
{
  byte_buffer pdu;

  // Prepend small vector
  std::vector<uint8_t> small_vec = make_small_vec();
  byte_buffer_view     view      = pdu.reserve_prepend(small_vec.size());
  TESTASSERT_EQ(small_vec.size(), pdu.length());
  TESTASSERT_EQ(small_vec.size(), view.length());

  std::copy(small_vec.begin(), small_vec.end(), view.begin());
  TESTASSERT(pdu == small_vec);
  TESTASSERT(view == small_vec);

  // Prepend big vector
  std::vector<uint8_t> big_vec = make_big_vec();
  byte_buffer_view     view2   = pdu.reserve_prepend(big_vec.size());
  TESTASSERT_EQ(small_vec.size() + big_vec.size(), pdu.length());
  TESTASSERT_EQ(big_vec.size(), view2.length());

  std::copy(big_vec.begin(), big_vec.end(), view2.begin());
  TESTASSERT(view2 == big_vec);
}

void test_byte_buffer_chain()
{
  byte_buffer pdu;
  TESTASSERT(pdu.empty());
  std::vector<uint8_t> big_vec      = make_big_vec();
  std::vector<uint8_t> small_vec    = make_small_vec();
  std::vector<uint8_t> bytes_concat = big_vec;
  bytes_concat.insert(bytes_concat.end(), small_vec.begin(), small_vec.end());
  std::vector<uint8_t> bytes_concat2 = bytes_concat;
  bytes_concat2.insert(bytes_concat2.end(), big_vec.begin(), big_vec.end());

  // Chain big vector to empty buffer
  byte_buffer pdu2(small_vec);
  TESTASSERT(pdu2 == small_vec);
  pdu.chain_before(std::move(pdu2));
  TESTASSERT(not pdu.empty());
  TESTASSERT(pdu == small_vec);
  TESTASSERT(pdu2.empty());

  // Chain byte_buffer before another non-empty byte_buffer.
  pdu2.append(big_vec);
  pdu.chain_before(std::move(pdu2));
  TESTASSERT(pdu2.empty());
  TESTASSERT_EQ(big_vec.size() + small_vec.size(), pdu.length());
  TESTASSERT(pdu == bytes_concat);

  // Chain byte_buffer after another non-empty byte_buffer.
  pdu2.append(big_vec);
  pdu.chain_after(std::move(pdu2));
  TESTASSERT(pdu2.empty());
  TESTASSERT_EQ(big_vec.size() * 2 + small_vec.size(), pdu.length());
  TESTASSERT(pdu == bytes_concat2);
}

int main()
{
  test_buffer_segment();
  test_byte_buffer_append();
  test_byte_buffer_prepend();
  test_byte_buffer_clear();
  test_byte_buffer_iterator();
  test_byte_buffer_compare();
  test_byte_buffer_copy();
  test_byte_buffer_move();
  test_byte_buffer_formatter();
  test_byte_buffer_view();
  test_byte_buffer_trim();
  test_byte_buffer_linearize();
  test_byte_buffer_initializer_list();
  test_byte_buffer_from_hexdump();
  test_byte_buffer_iterator_plus_equal_op();
  test_byte_buffer_reader_split_advance();
  test_byte_buffer_writer();
  test_byte_buffer_reserve_prepend();
  test_byte_buffer_chain();
}