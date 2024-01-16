/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/asn1/asn1_utils.h"
#include "srsran/support/srsran_test.h"
#include "srsran/support/test_utils.h"
#include <cmath>
#include <gtest/gtest.h>
#include <numeric>
#include <random>

using namespace std;
using namespace asn1;
using srsran::byte_buffer;

std::random_device rd;
std::mt19937       g(rd());

srsran::log_sink_spy* test_spy = nullptr;

TEST(asn1_bit_ref_test, unpack_empty_buffer)
{
  byte_buffer pdu;
  cbit_ref    bref(pdu);
  uint8_t     dummy  = 0;
  SRSASN_CODE result = bref.unpack(dummy, 1);
  ASSERT_EQ(result, SRSASN_ERROR_DECODE_FAIL);

  // Make sure the log backend has already processed the generated log entries.
  srslog::flush();
  TESTASSERT(test_spy->get_error_counter() == 1 and test_spy->get_warning_counter() == 0);
  test_spy->reset_counters();
}

TEST(asn1_bit_ref_test, if_no_pack_is_called_buffer_stays_empty)
{
  byte_buffer pdu;
  {
    bit_ref bref(pdu);
    ASSERT_EQ(bref.distance(), 0);
    ASSERT_EQ(bref.distance_bytes(), 0);
  }
  ASSERT_EQ(pdu.length(), 0);
}

TEST(asn1_bit_ref_test, pack_up_to_1_byte_fills_underlying_buffer)
{
  byte_buffer pdu;
  bit_ref     bref(pdu);
  unsigned    nof_bits = std::uniform_int_distribution<unsigned>{1, 8}(g);
  unsigned    value    = std::uniform_int_distribution<unsigned>{0, (1U << nof_bits) - 1U}(g);

  bref.pack(value, nof_bits);
  ASSERT_EQ(bref.distance(), nof_bits);
  ASSERT_EQ(bref.distance_bytes(), 1);
  ASSERT_EQ(pdu.length(), 1);
  ASSERT_EQ(*pdu.begin(), value << (8U - nof_bits));
}

TEST(asn1_array_test, pack_unpack_operators)
{
  /* Test Ext Array */
  ext_array<int> ext_ar;
  const int*     old_ptr = ext_ar.data();
  // Test Resize until goes out of small buffer
  for (uint32_t i = 0; i < ext_ar.small_buffer_size + 1; ++i) {
    TESTASSERT(ext_ar.is_in_small_buffer());
    TESTASSERT(ext_ar.capacity() == ext_ar.small_buffer_size);
    TESTASSERT(ext_ar.size() == i);
    ext_ar.push_back(i);
  }
  TESTASSERT(not ext_ar.is_in_small_buffer());
  TESTASSERT(ext_ar.size() == ext_ar.small_buffer_size + 1);
  TESTASSERT(old_ptr != ext_ar.data());
  TESTASSERT(ext_ar.capacity() > ext_ar.small_buffer_size);
  // Test Copy
  {
    ext_array<int> ext_ar2(ext_ar);
    TESTASSERT(ext_ar2 == ext_ar);
    TESTASSERT(not ext_ar.is_in_small_buffer());
  }
  TESTASSERT(ext_ar.size() == 5);
  // Test Move
  {
    ext_array<int> ext_ar2(std::move(ext_ar));
    TESTASSERT(ext_ar.is_in_small_buffer());
    TESTASSERT(ext_ar2.size() == 5);
    TESTASSERT(ext_ar2[2] == 2);
  }
  ext_ar = ext_array<int>(5);
  TESTASSERT(not ext_ar.is_in_small_buffer());
  TESTASSERT(ext_ar.size() == 5);
  ext_ar.resize(0);
  TESTASSERT(not ext_ar.is_in_small_buffer());
}

TEST(asn1_bit_ref, pack_unpack_operators)
{
  for (uint32_t n_bit_stride = 1; n_bit_stride < 32; ++n_bit_stride) {
    srsran::byte_buffer buf;
    uint32_t            nof_bytes_to_pack = n_bit_stride * 4;

    // Pack in batches of n_bit_stride.
    bit_ref bref(buf);
    for (uint32_t i = 0; i < nof_bytes_to_pack * 8 / n_bit_stride; ++i) {
      ASSERT_EQ(i * n_bit_stride, (unsigned)bref.distance());
      ASSERT_EQ(SRSASN_SUCCESS, bref.pack(i, n_bit_stride));
    }
    ASSERT_EQ(nof_bytes_to_pack, (unsigned)bref.distance_bytes());
    ASSERT_EQ(nof_bytes_to_pack, (unsigned)bref.distance() / 8U);
    ASSERT_EQ(0, bref.distance() % 8);
    //    bool is_eq = buf == vec_compare;
    //    TESTASSERT(is_eq);

    // Unpack in batches of n_bit_stride.
    cbit_ref bref2(buf);
    uint64_t bitmask = (1u << n_bit_stride) - 1;
    ASSERT_EQ(0, bref2.distance_bytes());
    for (uint32_t i = 0; i < nof_bytes_to_pack * 8 / n_bit_stride; ++i) {
      uint64_t val;
      ASSERT_EQ(i * n_bit_stride, (unsigned)bref2.distance());
      ASSERT_EQ(SRSASN_SUCCESS, bref2.unpack(val, n_bit_stride));
      TESTASSERT((i & bitmask) == val);
    }
    ASSERT_EQ(nof_bytes_to_pack, (unsigned)bref2.distance_bytes());
    ASSERT_EQ(bref.distance(), bref2.distance());
  }

  // pack bytes aligned
  {
    srsran::byte_buffer            buf;
    constexpr uint32_t             nof_bytes = 512;
    uint32_t                       start     = 1;
    bit_ref                        bref(buf);
    std::array<uint8_t, nof_bytes> buf2;
    for (uint32_t i = 0; i < nof_bytes; ++i) {
      buf2[i] = start + i;
    }

    ASSERT_EQ(SRSASN_SUCCESS, bref.pack_bytes(buf2));
    ASSERT_EQ((int)nof_bytes, bref.distance_bytes());
    ASSERT_EQ(nof_bytes, buf.length());
    TESTASSERT(buf == buf2);
    cbit_ref bref2(buf);
    ASSERT_EQ(SRSASN_SUCCESS, bref2.unpack_bytes(buf2));
    ASSERT_EQ((int)nof_bytes, bref2.distance_bytes());
    TESTASSERT(buf == buf2);
  }

  // pack bytes unaligned
  {
    constexpr uint32_t             nof_bytes = 128;
    uint32_t                       start     = 1;
    srsran::byte_buffer            buf;
    bit_ref                        bref(buf);
    std::array<uint8_t, nof_bytes> buf2, buf3;
    for (uint32_t i = 0; i < nof_bytes; ++i) {
      buf2[i] = start + i;
    }

    // pack.
    ASSERT_EQ(SRSASN_SUCCESS, bref.pack(0, 1)); // this unaligns.
    ASSERT_EQ(SRSASN_SUCCESS, bref.pack_bytes(buf2));
    ASSERT_EQ(bref.distance_bytes(), (int)nof_bytes + 1);
    ASSERT_EQ(bref.distance(), (int)nof_bytes * 8 + 1);
    ASSERT_EQ(bref.distance_bytes(), (int)buf.length());

    // unpack and check original bytes with unpacked ones.
    cbit_ref bref2(buf);
    uint32_t val;
    ASSERT_EQ(SRSASN_SUCCESS, bref2.unpack(val, 1));
    TESTASSERT(val == 0);
    ASSERT_EQ(SRSASN_SUCCESS, bref2.unpack_bytes(buf3));
    TESTASSERT(bref2.distance_bytes() == (int)nof_bytes + 1);
    TESTASSERT(bref2.distance_bytes() == bref.distance_bytes());
    TESTASSERT(std::equal(buf2.begin(), buf2.end(), buf3.begin(), buf3.end()));
  }

  // test advance bits
  {
    srsran::byte_buffer buf{std::vector<uint8_t>(256)};
    cbit_ref            bref(buf);
    ASSERT_EQ(SRSASN_SUCCESS, bref.advance_bits(4));
    ASSERT_EQ(4, bref.distance());
    ASSERT_EQ(SRSASN_SUCCESS, bref.advance_bits(4));
    ASSERT_EQ(8, bref.distance());
    ASSERT_EQ(SRSASN_SUCCESS, bref.advance_bits(3));
    ASSERT_EQ(11, bref.distance());
    ASSERT_EQ(SRSASN_SUCCESS, bref.advance_bits(200));
    ASSERT_EQ(211, bref.distance());
    ASSERT_EQ(SRSASN_SUCCESS, bref.advance_bits(5));
    ASSERT_EQ(216, bref.distance());
  }
}

TEST(asn1_octet_string_test, pack_unpack_operators)
{
  std::string        hexstr = "014477aaff";
  fixed_octstring<5> statstr;
  dyn_octstring      dynstr;
  statstr.from_string(hexstr);
  dynstr.from_string(hexstr);

  TESTASSERT(sizeof(statstr) == statstr.size());
  TESTASSERT(statstr.size() == 5);
  TESTASSERT(dynstr.size() == 5);
  TESTASSERT(dynstr[0] == 1);
  TESTASSERT(statstr[4] == 255);
  TESTASSERT(statstr.to_string() == hexstr);
  TESTASSERT(statstr.to_string() == dynstr.to_string());
  TESTASSERT(statstr.to_number() == dynstr.to_number());

  // check endianess
  TESTASSERT(statstr.to_number() == 5443660543);
  TESTASSERT(dynstr.to_number() == 5443660543);
  dynstr.from_number(dynstr.to_number());
  TESTASSERT(dynstr.to_number() == 5443660543);

  statstr.from_string(hexstr);
  dynstr.from_string(hexstr);

  srsran::byte_buffer buf;
  bit_ref             b{buf};
  statstr.pack(b);
  TESTASSERT(std::equal(buf.begin(), buf.end(), statstr.begin(), statstr.end())); // no prefix in static strings.
  TESTASSERT(b.distance() == (int)statstr.size() * 8);
  fixed_octstring<5> statstr2;
  cbit_ref           b2 = cbit_ref(buf);
  ASSERT_EQ(SRSASN_SUCCESS, statstr2.unpack(b2));
  TESTASSERT(statstr == statstr2);

  buf.clear();
  b = {buf};
  dynstr.pack(b);
  TESTASSERT(*buf.begin() == dynstr.size()); // true for small strings.
  TESTASSERT(std::equal(buf.begin() + 1, buf.end(), dynstr.begin(), dynstr.end()));
  TESTASSERT(b.distance() == (int)(dynstr.size() * 8 + 8));
  dyn_octstring dynstr2; // unpacker allocates automatically
  b2 = cbit_ref(buf);
  dynstr2.unpack(b2);
  TESTASSERT(dynstr == dynstr2);

  // test copy ctor
  {
    fixed_octstring<5> statcopy(statstr);
    TESTASSERT(statcopy == statstr);
  }
  {
    dyn_octstring scopy(dynstr);
    TESTASSERT(scopy == dynstr);
  }

  b.pack(1, 1);
  ASSERT_EQ(b.distance(), (int)(hexstr.size() * 8 / 2 + 9));
  b.align_bytes_zero();
  TESTASSERT(b.distance() == (int)(hexstr.size() * 8 / 2 + 16));
}

TEST(asn1_fixed_bit_string, integer_conversion)
{
  fixed_bitstring<48> bitstr;
  ASSERT_EQ(bitstr.from_number(4211117727).to_number(), 4211117727);
}

TEST(asn1_bit_string_test, pack_unpack_operators)
{
  fixed_bitstring<10>      bstr1;
  bounded_bitstring<5, 15> bound_bstr1(10);
  dyn_bitstring            dyn_bstr1(10);
  bstr1.set(7, true);
  bstr1.set(9, true);
  bound_bstr1.set(7, true);
  bound_bstr1.set(9, true);
  dyn_bstr1.set(7, true);
  dyn_bstr1.set(9, true);

  /* Test BitSet and BitGet and Length */
  // fixed
  TESTASSERT(bstr1.length() == 10);
  TESTASSERT(bstr1.get(7));
  TESTASSERT(bstr1.get(9));
  TESTASSERT(not bstr1.get(8));
  TESTASSERT(not bstr1.get(0));
  // bounded
  TESTASSERT(bound_bstr1.length() == 10);
  TESTASSERT(bound_bstr1.get(7));
  TESTASSERT(bound_bstr1.get(9));
  TESTASSERT(not bound_bstr1.get(8));
  TESTASSERT(not bound_bstr1.get(0));
  // dyn
  TESTASSERT(dyn_bstr1.length() == 10);
  TESTASSERT(dyn_bstr1.get(7));
  TESTASSERT(dyn_bstr1.get(9));
  TESTASSERT(not dyn_bstr1.get(8));
  TESTASSERT(not dyn_bstr1.get(0));

  /* Test Copy, Dtor, and Equal Comparison */
  {
    // fixed
    fixed_bitstring<10> bstr2;
    bstr2 = bstr1;
    TESTASSERT(bstr2 == bstr1);
    // bound
    bounded_bitstring<5, 15> bound_bstr2;
    bound_bstr2 = bound_bstr1;
    TESTASSERT(bound_bstr2 == bound_bstr1);
    // dyn
    dyn_bitstring dyn_bstr2(10);
    dyn_bstr2 = dyn_bstr1;
    TESTASSERT(dyn_bstr2 == dyn_bstr1);
    // dtors called
  }

  /* Test Number and String conversion */
  // fixed
  TESTASSERT(bstr1.to_string() == "1010000000");
  TESTASSERT(bstr1.to_number() == 640);
  bstr1.set(1, true);
  TESTASSERT(bstr1.to_string() == "1010000010");
  TESTASSERT(bstr1.to_number() == 642);
  TESTASSERT(bstr1 == fixed_bitstring<10>().from_number(642));
  // bounded
  TESTASSERT(bound_bstr1.to_string() == "1010000000");
  TESTASSERT(bound_bstr1.to_number() == 640);
  bound_bstr1.set(1, true);
  TESTASSERT(bound_bstr1.to_string() == "1010000010");
  TESTASSERT(bound_bstr1.to_number() == 642);
  TESTASSERT((bound_bstr1 == bounded_bitstring<5, 15>().from_number(642)));
  TESTASSERT((bound_bstr1 == bounded_bitstring<5, 15>().from_string("1010000010")));
  // dyn
  TESTASSERT(dyn_bstr1.to_string() == "1010000000");
  TESTASSERT(dyn_bstr1.to_number() == 640);
  dyn_bstr1.set(1, true);
  TESTASSERT(dyn_bstr1.to_string() == "1010000010");
  TESTASSERT(dyn_bstr1.to_number() == 642);
  TESTASSERT(dyn_bstr1 == dyn_bitstring().from_number(642));
  TESTASSERT(dyn_bstr1 == dyn_bitstring().from_string("1010000010"));
  // zero case
  TESTASSERT(bound_bstr1.from_number(0) == "00000");

  /* Test Resize */
  bound_bstr1.resize(11);
  TESTASSERT(not bound_bstr1.get(9)); // resets content
  TESTASSERT(bound_bstr1.length() == 11);
  bound_bstr1.set(9, true);
  dyn_bstr1.resize(11);
  TESTASSERT(not dyn_bstr1.get(9));
  TESTASSERT(dyn_bstr1.length() == 11);
  dyn_bstr1.set(9, true);
  TESTASSERT(dyn_bstr1.get(9));
  TESTASSERT(not dyn_bstr1.get(10));

  /* Test Packing/Unpacking */
  srsran::byte_buffer buf;
  bit_ref             bref(buf);
  // fixed
  ASSERT_EQ(SRSASN_SUCCESS, bstr1.pack(bref));
  fixed_bitstring<10> bstr2;
  TESTASSERT(bstr2.length() == 10);
  cbit_ref bref2(buf);
  ASSERT_EQ(SRSASN_SUCCESS, bstr2.unpack(bref2));
  TESTASSERT(bstr2 == bstr1);
  TESTASSERT(bref.distance() == 10 and bref.distance() == bref2.distance());
  // bounded
  buf.clear();
  bref = bit_ref(buf);
  TESTASSERT(bound_bstr1.pack(bref) == SRSASN_SUCCESS);
  bounded_bitstring<5, 15> bound_bstr2(dyn_bstr1.length());
  TESTASSERT(bound_bstr2.length() == 11);
  bref2 = cbit_ref(buf);
  TESTASSERT(bound_bstr2.unpack(bref2) == SRSASN_SUCCESS);
  TESTASSERT(bound_bstr2 == bound_bstr1);
  TESTASSERT(bref.distance() == (11 + 4) and bref.distance() == bref2.distance());
  // dyn
  buf.clear();
  bref = bit_ref(buf);
  TESTASSERT(dyn_bstr1.pack(bref) == SRSASN_SUCCESS);
  dyn_bitstring dyn_bstr2(dyn_bstr1.length());
  TESTASSERT(dyn_bstr2.length() == 11);
  bref2 = cbit_ref(buf);
  TESTASSERT(dyn_bstr2.unpack(bref2) == SRSASN_SUCCESS);
  TESTASSERT(dyn_bstr2 == dyn_bstr1);
  //  printf("%s==%s\n", dyn_bstr1.to_string().c_str(), dyn_bstr2.to_string().c_str());

  // disable temporarily the prints to check failures
  //  srsran::nullsink_log null_log("NULL");
  //  bit_ref bref3(&buffer[0], sizeof(buffer));
  //  TESTASSERT(dyn_bstr1.pack(bref3, false, 5, 10)==SRSASN_ERROR_ENCODE_FAIL);

  /* Test Pack/Unpack 2 */
  buf.clear();
  ASSERT_TRUE(buf.append(0));
  ASSERT_TRUE(buf.append(7));
  bref2 = cbit_ref(buf);
  fixed_bitstring<16> bstr3;
  bstr3.unpack(bref2);
  TESTASSERT(bstr3 == "0000000000000111");
  TESTASSERT(bstr3.to_string() == "0000000000000111");
}

TEST(asn1_seq_of_test, pack_unpack_and_operators)
{
  uint32_t                 fixed_list_size = 33;
  std::array<uint32_t, 33> fixed_list;
  for (uint32_t i = 0; i < fixed_list_size; ++i) {
    fixed_list[i] = i;
  }
  {
    // test copy and ==
    std::array<uint32_t, 33> fixed_list2 = fixed_list;
    TESTASSERT(fixed_list == fixed_list2);
  }

  int      lb = 0, ub = 40;
  uint32_t n_bits = ceil(log2(ub - lb + 1));

  srsran::byte_buffer buffer;
  bit_ref             b{buffer};
  pack_fixed_seq_of(b, fixed_list, fixed_list.size(), integer_packer<uint32_t>(lb, ub, false));
  TESTASSERT(b.distance() == (int)(fixed_list_size * n_bits));
  cbit_ref                 b2(buffer);
  std::array<uint32_t, 33> fixed_list2;
  unpack_fixed_seq_of(fixed_list2, b2, fixed_list.size(), integer_packer<uint32_t>(lb, ub, false));
  TESTASSERT(fixed_list == fixed_list2);

  // bounded seq_of
  buffer.clear();
  bounded_array<uint32_t, 33> bseq;
  TESTASSERT(bseq.size() == 0);
  bseq.resize(fixed_list_size);
  TESTASSERT(bseq.size() == fixed_list_size);
  memcpy(&bseq[0], &fixed_list[0], fixed_list_size * sizeof(uint32_t));
  b = {buffer};
  pack_dyn_seq_of(b, bseq, 0, 33, integer_packer<uint32_t>(lb, ub, false));
  TESTASSERT(b.distance() == (int)((fixed_list_size + 1) * n_bits)); // unaligned
  //  ASSERT_EQ(b.distance(), (int)fixed_list_size * (int)n_bits + 8); // aligned
  bounded_array<uint32_t, 33> bseq2;
  bseq2.resize(fixed_list_size);
  b2 = {buffer};
  unpack_dyn_seq_of(bseq2, b2, 0, 33, integer_packer<uint32_t>(lb, ub, false));
  TESTASSERT(bseq2 == bseq);
  TESTASSERT(std::equal(&bseq2[0], &bseq2[fixed_list_size], &fixed_list[0]));

  {
    bounded_array<uint32_t, 33> bseq3;
    bseq3 = bseq;
    TESTASSERT(bseq == bseq3);
    // call dtor
  }
  TESTASSERT(bseq2 == bseq);

  // dynamic array
  dyn_array<int> vec, vec2(33);
  std::iota(vec2.begin(), vec2.end(), 0);
  std::shuffle(vec2.begin(), vec2.end(), g);
  TESTASSERT(vec2.size() == 33);
  TESTASSERT(vec.size() == 0);
  vec.resize(32);
  std::iota(vec.begin(), vec.end(), 0);
  std::shuffle(vec.begin(), vec.end(), g);
  TESTASSERT(vec.size() == 32);
  vec[5] = 5;
  vec.push_back(33);
  TESTASSERT(vec.size() == 33);
  TESTASSERT(vec[5] == 5); // resize does not affect content
  int* pos = &vec[5];
  vec.resize(10);
  TESTASSERT(vec.size() == 10); // reduction in size does not cause realloc
  TESTASSERT(pos == &vec[5]);
  {
    dyn_array<int> vec3 = vec2;
    TESTASSERT(vec3.size() == vec2.size());
    TESTASSERT(std::equal(vec3.begin(), vec3.end(), vec2.begin()));
    vec3 = vec;
    TESTASSERT(vec3.size() == vec.size());
    TESTASSERT(std::equal(vec3.begin(), vec3.end(), vec.begin()));
  }
  TESTASSERT(vec[5] == 5);
}

TEST(asn1_copy_ptr_test, memory_safety)
{
  typedef fixed_octstring<10> TestType;
  char                        buffer[1024];
  uint32_t                    N = 10;
  for (uint32_t i = 0; i < N; ++i) {
    buffer[i] = i;
  }

  TestType*          s = new fixed_octstring<10>();
  copy_ptr<TestType> cptr(s);
  copy_ptr<TestType> cptr2;

  memcpy(&(*cptr)[0], buffer, N);

  TESTASSERT(cptr2.get() == NULL);
  TESTASSERT(memcmp(&(*s)[0], buffer, s->size()) == 0);
  TESTASSERT(*cptr == *s);
  TESTASSERT(cptr.get() == s);
  TESTASSERT(memcmp(&(*cptr)[0], buffer, cptr->size()) == 0);
  TESTASSERT(cptr2.get() == NULL);
  TESTASSERT(cptr.get() != NULL);

  {
    copy_ptr<TestType> cptr3 = cptr;
    TESTASSERT(cptr3 == cptr);
    TESTASSERT(*cptr3 == *s);
    TESTASSERT(cptr3.get() != cptr.get()); // different addresses
    TESTASSERT(memcmp(&(*cptr3)[0], buffer, cptr3->size()) == 0);
    // call dtor
  }
  TESTASSERT(*cptr == *s);

  // Test make_copy_ptr() function. It has the potential to hide all news from the code.
  TestType           s2(*s);
  copy_ptr<TestType> cptr3 = make_copy_ptr(s2);
  TESTASSERT(cptr3.get() != &s2);
  TESTASSERT(cptr3 == cptr);

  // get raw pointer and delete manually
  TestType* s3 = cptr.release();
  TESTASSERT(s3 == s); // same address
  delete s3;           // it should *not* double free
}

class EnumTest
{
public:
  enum options { test5, test10, test20, nulltype };
  options               value;
  static const uint32_t nof_types = 3, nof_exts = 0;
  static const bool     has_ext = false;
  EnumTest() {}
  EnumTest(options v) : value(v) {}
  EnumTest& operator=(options v)
  {
    value = v;
    return *this;
  }
              operator uint8_t() { return (uint8_t)value; }
  std::string to_string() const
  {
    switch (value) {
      case test5:
        return "test5";
      case test10:
        return "test10";
      case test20:
        return "test20";
      default:
        printf("invalid value\n");
    }
    return "";
  }
  int32_t to_number() const
  {
    switch (value) {
      case test5:
        return 5;
      case test10:
        return 10;
      case test20:
        return 20;
      default:
        printf("invalid value\n");
    }
    return -1;
  }
};

TEST(asn1_enumerated, pack_unpack)
{
  EnumTest e;
  EnumTest e2;
  e = EnumTest::test10;
  TESTASSERT(e.nof_types == 3);
  TESTASSERT(e.to_string() == "test10");
  TESTASSERT(e.to_number() == 10);
  TESTASSERT(string_to_enum<EnumTest>(e2, "test10"));
  TESTASSERT(e2 == e);
  TESTASSERT(number_to_enum<EnumTest>(e2, 10));
  TESTASSERT(e2 == e);

  srsran::byte_buffer buffer;
  bit_ref             bref(buffer);
  TESTASSERT(pack_enum(bref, e) == SRSASN_SUCCESS);
  TESTASSERT(bref.distance() == (int)(floor(log2(e.nof_types)) + 1));

  cbit_ref bref2(buffer);
  TESTASSERT(unpack_enum(e2, bref2) == SRSASN_SUCCESS);
  TESTASSERT(bref2.distance() == (int)bref.distance());
  TESTASSERT(e == e2);

  // Test fail path
  buffer.clear();
  TESTASSERT(test_spy->get_error_counter() == 0 and test_spy->get_warning_counter() == 0);
  bref = bit_ref(buffer);
  e    = EnumTest::nulltype;
  TESTASSERT(pack_enum(bref, e) == SRSASN_ERROR_ENCODE_FAIL);
  ASSERT_EQ(0, bref.distance());
  ASSERT_TRUE(buffer.append(255));
  bref2 = cbit_ref(buffer);
  TESTASSERT(unpack_enum(e, bref2) == SRSASN_ERROR_DECODE_FAIL);

  // Make sure the log backend has already processed the generated log entries.
  srslog::flush();
  TESTASSERT(test_spy->get_error_counter() == 2 and test_spy->get_warning_counter() == 0);
  test_spy->reset_counters();
}

class EnumBoolTest
{
public:
  enum options { true_value, nulltype };
  options               value;
  static const uint32_t nof_types = 1, nof_exts = 0;
  static const bool     has_ext = false;
  EnumBoolTest() {}
  EnumBoolTest(options v) : value(v) {}
  EnumBoolTest& operator=(options v)
  {
    value = v;
    return *this;
  }
              operator uint8_t() { return (uint8_t)value; }
  std::string to_string() const
  {
    switch (value) {
      case true_value:
        return "true";
      default:
        printf("invalid value\n");
    }
    return "";
  }
};

TEST(asn1_enumerated, bool_to_enum_test)
{
  EnumBoolTest e, e2;
  TESTASSERT(e.nof_types == 1);
  bool_to_enum<EnumBoolTest>(e, true);
  TESTASSERT(e.to_string() == "true");
  e2 = EnumBoolTest::options::true_value;
  TESTASSERT(enum_to_bool(e2));
  e2 = EnumBoolTest::options::nulltype;
  TESTASSERT(!enum_to_bool(e2));
};

void test_json_writer()
{
  json_writer writer;

  writer.start_array("");
  writer.start_obj("");
  writer.start_obj("obj1");
  writer.write_str("field1", "01010101");
  writer.write_int("field2", 5);
  writer.write_bool("field3", true);
  writer.start_obj("obj2");
  writer.write_int("field4", 1);
  writer.start_obj("obj3");
  writer.end_obj();
  writer.start_array("array1");
  writer.end_array();
  writer.end_obj();
  writer.end_obj();
  writer.end_obj();
  writer.end_array();

  fmt::print("{}\n", writer.to_string().c_str());
}

TEST(asn1_integer_test, large_integer_pack_unpack)
{
  integer<uint64_t, 0, 4294967295, false, true> big_integer = 3172073535;

  srsran::byte_buffer buffer;
  bit_ref             bref(buffer);
  TESTASSERT(big_integer.pack(bref) == 0);

  std::array<uint8_t, 5> bytes{0xc0, 0xbd, 0x12, 0x00, 0x3f};
  TESTASSERT(std::equal(buffer.begin(), buffer.end(), bytes.begin(), bytes.end()));

  integer<uint64_t, 0, 4294967295, false, true> big_integer2;
  cbit_ref                                      cbref(buffer);
  TESTASSERT(big_integer2.unpack(cbref) == 0);
  TESTASSERT(big_integer == big_integer2);
}

TEST(asn1_varlength_field_test, pack)
{
  srsran::byte_buffer buffer;
  bit_ref             bref(buffer);
  ASSERT_EQ(SRSASN_SUCCESS, bref.pack(0, 1));
  ASSERT_EQ(1, bref.distance());
  {
    varlength_field_pack_guard guard(bref);
    ASSERT_EQ(0, bref.distance());
    bref.pack(0, 8);
    ASSERT_EQ(1, bref.distance_bytes());
  }
  ASSERT_EQ(17, bref.distance()); // accounts for length determinant and 1 byte of data
}

int main(int argc, char** argv)
{
  // Setup the log spy to intercept error and warning log entries.
  if (!srslog::install_custom_sink(
          srsran::log_sink_spy::name(),
          std::unique_ptr<srsran::log_sink_spy>(new srsran::log_sink_spy(srslog::get_default_log_formatter())))) {
    return SRSASN_ERROR;
  }
  test_spy = static_cast<srsran::log_sink_spy*>(srslog::find_sink(srsran::log_sink_spy::name()));
  if (!test_spy) {
    return SRSASN_ERROR;
  }

  auto& asn1_logger = srslog::fetch_basic_logger("ASN1", *test_spy, false);
  asn1_logger.set_level(srslog::basic_levels::debug);
  asn1_logger.set_hex_dump_max_size(-1);

  // Start the log backend.
  srslog::init();

  //  TESTASSERT(test_json_writer()==0);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
