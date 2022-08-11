/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

std::random_device rd;
std::mt19937       g(rd());

template <typename T>
class bitmask_tester : public ::testing::Test
{
protected:
  using Integer                    = T;
  static constexpr size_t nof_bits = sizeof(Integer) * 8U;
};
using mask_integer_types = ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>;
TYPED_TEST_SUITE(bitmask_tester, mask_integer_types);

TYPED_TEST(bitmask_tester, lsb_ones)
{
  using Integer = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ(0, mask_lsb_ones<Integer>(0));
  ASSERT_EQ(static_cast<Integer>(-1), mask_lsb_ones<Integer>(this->nof_bits));
  ASSERT_EQ(0b11, mask_lsb_ones<Integer>(2));

  // test all combinations.
  for (unsigned nof_ones = 0; nof_ones != this->nof_bits; ++nof_ones) {
    Integer expected = (static_cast<uint64_t>(1U) << nof_ones) - 1U;
    ASSERT_EQ(expected, mask_lsb_ones<Integer>(nof_ones)) << "for nof_ones=" << nof_ones;
  }
}

TYPED_TEST(bitmask_tester, lsb_zeros)
{
  using Integer = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ((Integer)-1, mask_lsb_zeros<Integer>(0));
  ASSERT_EQ(0, mask_lsb_zeros<Integer>(this->nof_bits));

  // test all combinations.
  for (unsigned nof_zeros = 0; nof_zeros != this->nof_bits; ++nof_zeros) {
    Integer expected = (static_cast<uint64_t>(1U) << nof_zeros) - 1U;
    expected         = ~expected;
    ASSERT_EQ(expected, mask_lsb_zeros<Integer>(nof_zeros)) << "for nof_zeros=" << nof_zeros;
    ASSERT_EQ((Integer)~mask_lsb_ones<Integer>(nof_zeros), mask_lsb_zeros<Integer>(nof_zeros));
  }
}

TYPED_TEST(bitmask_tester, msb_ones)
{
  using Integer = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ(0, mask_msb_ones<Integer>(0));
  ASSERT_EQ(static_cast<Integer>(-1), mask_msb_ones<Integer>(this->nof_bits));

  // test all combinations.
  for (unsigned nof_ones = 0; nof_ones != this->nof_bits; ++nof_ones) {
    Integer expected = 0;
    if (nof_ones > 0) {
      unsigned nof_lsb_zeros = this->nof_bits - nof_ones;
      expected               = ~((static_cast<Integer>(1U) << (nof_lsb_zeros)) - 1U);
    }
    ASSERT_EQ(expected, mask_msb_ones<Integer>(nof_ones)) << "for nof_ones=" << nof_ones;
  }
}

TYPED_TEST(bitmask_tester, msb_zeros)
{
  using Integer = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ((Integer)-1, mask_msb_zeros<Integer>(0));
  ASSERT_EQ(0, mask_msb_zeros<Integer>(this->nof_bits));

  // test all combinations.
  for (unsigned nof_zeros = 0; nof_zeros != this->nof_bits; ++nof_zeros) {
    Integer expected = 0;
    if (nof_zeros > 0) {
      unsigned nof_lsb_ones = this->nof_bits - nof_zeros;
      expected              = ~((static_cast<Integer>(1U) << (nof_lsb_ones)) - 1U);
    }
    expected = ~expected;
    ASSERT_EQ(expected, mask_msb_zeros<Integer>(nof_zeros)) << "for nof_zeros=" << nof_zeros;
    ASSERT_EQ((Integer)~mask_lsb_ones<Integer>(nof_zeros), mask_lsb_zeros<Integer>(nof_zeros));
  }
}

TYPED_TEST(bitmask_tester, first_lsb_one)
{
  using Integer = typename TestFixture::Integer;
  std::uniform_int_distribution<Integer> rd_int{0, std::numeric_limits<Integer>::max()};

  // sanity checks.
  ASSERT_EQ(std::numeric_limits<Integer>::digits, find_first_lsb_one<Integer>(0));
  ASSERT_EQ(0, find_first_lsb_one<Integer>(-1));
  ASSERT_EQ(0, find_first_lsb_one<Integer>(0b1));
  ASSERT_EQ(1, find_first_lsb_one<Integer>(0b10));
  ASSERT_EQ(0, find_first_lsb_one<Integer>(0b11));

  // test all combinations.
  for (unsigned one_idx = 0; one_idx != this->nof_bits - 1; ++one_idx) {
    Integer mask  = mask_lsb_zeros<Integer>(one_idx);
    Integer value = rd_int(g) & mask;

    ASSERT_EQ(find_first_lsb_one(mask), one_idx);
    ASSERT_GE(find_first_lsb_one(value), one_idx) << fmt::format("for value {:#b}", value);
  }
}

TYPED_TEST(bitmask_tester, first_msb_one)
{
  using Integer = typename TestFixture::Integer;
  std::uniform_int_distribution<Integer> rd_int{0, std::numeric_limits<Integer>::max()};

  // sanity checks.
  ASSERT_EQ(std::numeric_limits<Integer>::digits, find_first_msb_one<Integer>(0));
  ASSERT_EQ(this->nof_bits - 1, find_first_msb_one<Integer>(-1));
  ASSERT_EQ(0, find_first_msb_one<Integer>(0b1));
  ASSERT_EQ(1, find_first_msb_one<Integer>(0b10));
  ASSERT_EQ(1, find_first_msb_one<Integer>(0b11));

  // test all combinations.
  for (unsigned one_idx = 0; one_idx != this->nof_bits - 1; ++one_idx) {
    Integer mask  = mask_lsb_ones<Integer>(one_idx + 1);
    Integer value = std::max((Integer)(rd_int(g) & mask), (Integer)1U);

    ASSERT_EQ(one_idx, find_first_msb_one(mask));
    ASSERT_LE(find_first_msb_one(value), one_idx) << fmt::format("for value {:#b}", value);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(BoundedBitset, all_zeros)
{
  constexpr size_t         MAX_SIZE = 25;
  unsigned                 size     = std::uniform_int_distribution<unsigned>{1, MAX_SIZE - 1}(g);
  bounded_bitset<MAX_SIZE> zero_mask(size);

  ASSERT_EQ(MAX_SIZE, zero_mask.max_size());
  ASSERT_EQ(size, zero_mask.size());
  ASSERT_EQ(0, zero_mask.count());
  ASSERT_TRUE(zero_mask.none());
  ASSERT_FALSE(zero_mask.any());
  ASSERT_FALSE(zero_mask.all());
}

TEST(BoundedBitset, empty_bitset)
{
  constexpr size_t         MAX_SIZE = 25;
  bounded_bitset<MAX_SIZE> zero_sized_mask;
  ASSERT_EQ(MAX_SIZE, zero_sized_mask.max_size());
  ASSERT_EQ(0, zero_sized_mask.size());
  ASSERT_EQ(0, zero_sized_mask.count());
  ASSERT_TRUE(zero_sized_mask.none());
  ASSERT_FALSE(zero_sized_mask.any());
  ASSERT_TRUE(zero_sized_mask.all());

  zero_sized_mask.flip();
  ASSERT_TRUE(zero_sized_mask.none()) << "Flipping empty bitset should be a no-op";
  ASSERT_EQ(0, zero_sized_mask.size()) << "Flipping empty bitset should be a no-op";

  bounded_bitset<MAX_SIZE> zero_mask(std::uniform_int_distribution<unsigned>{1, MAX_SIZE - 1}(g));
  ASSERT_NE(zero_mask, zero_sized_mask);
  zero_sized_mask.resize(zero_mask.size());
  ASSERT_EQ(zero_mask, zero_sized_mask);
}

TEST(BoundedBitset, initializer_list_constructor)
{
  bounded_bitset<25> bitmap = {true, true, true, true, false};
  ASSERT_EQ(bitmap.size(), 5);
  ASSERT_TRUE(bitmap.test(0));
  ASSERT_TRUE(bitmap.test(1));
  ASSERT_TRUE(bitmap.test(3));
  ASSERT_FALSE(bitmap.test(4));
}

TEST(BoundedBitset, iterator_constructor)
{
  std::vector<bool> data(std::uniform_int_distribution<unsigned>{1, 24}(g));
  for (auto it = data.begin(); it != data.end(); ++it) {
    *it = std::bernoulli_distribution{}(g);
  }
  bounded_bitset<25> bitmap(data.begin(), data.end());
  ASSERT_EQ(bitmap.size(), data.size());

  for (unsigned i = 0; i != data.size(); ++i) {
    ASSERT_EQ(data[i], bitmap.test(i));
  }
}

TEST(BoundedBitset, all_ones)
{
  unsigned           bitset_size = std::uniform_int_distribution<unsigned>{1, 24}(g);
  std::vector<bool>  vec(bitset_size, true);
  bounded_bitset<25> mask(vec.begin(), vec.end());

  ASSERT_TRUE(mask.all());
  ASSERT_TRUE(mask.any());
  ASSERT_FALSE(mask.none());
  ASSERT_EQ(mask.size(), mask.count());
}

TEST(BoundedBitset, integer_bitset_conversion)
{
  unsigned           bitset_size = 23;
  unsigned           nof_ones    = std::uniform_int_distribution<unsigned>{1, bitset_size}(g);
  unsigned           integermask = mask_lsb_ones<unsigned>(nof_ones);
  bounded_bitset<25> mask(bitset_size);
  mask.from_uint64(integermask);

  ASSERT_EQ(bitset_size, mask.size());
  ASSERT_EQ(nof_ones, mask.count());
  ASSERT_TRUE(mask.any());
  ASSERT_FALSE(mask.none());
  ASSERT_EQ(integermask, mask.to_uint64());
}

TEST(BoundedBitset, flip)
{
  unsigned           bitset_size = std::uniform_int_distribution<unsigned>{1, 24}(g);
  bounded_bitset<25> mask(bitset_size);
  uint64_t integerbitmap  = std::uniform_int_distribution<uint64_t>{0, mask_lsb_ones<unsigned>(bitset_size)}(g);
  uint64_t flipped_bitmap = (~integerbitmap) & mask_lsb_ones<uint64_t>(bitset_size);

  mask.from_uint64(integerbitmap);
  ASSERT_EQ(integerbitmap, mask.to_uint64());
  mask.flip();
  ASSERT_EQ(flipped_bitmap, mask.to_uint64()) << fmt::format("For original bitmap {:#b}", integerbitmap);
}

TEST(BoundedBitset, setting_bit)
{
  bounded_bitset<25> mask(23);
  mask.set(10);

  ASSERT_TRUE(mask.any());
  ASSERT_TRUE(mask.all(10, 11));
  ASSERT_TRUE(not mask.all(10, 12));
  ASSERT_TRUE(not mask.all());
  ASSERT_TRUE(not mask.test(0));
  ASSERT_TRUE(mask.test(10));
  mask.flip();
  ASSERT_TRUE(not mask.test(10));
  ASSERT_TRUE(mask.test(0));

  bounded_bitset<25> mask1(25);
  mask1.set(10);
  mask1.set(11);
  mask1.set(12);
  mask1.set(13);
  mask1.set(14);
  ASSERT_TRUE(mask1.all(10, 15));
  ASSERT_TRUE(not mask1.all(10, 16));
  ASSERT_TRUE(not mask1.all(9, 15));
  ASSERT_TRUE(mask1.any(10, 15));
  ASSERT_TRUE(mask1.any());

  bounded_bitset<25> mask2(25);
  mask2.set(0);
  mask2.set(1);
  mask2.set(2);
  mask2.set(3);
  mask2.set(4);
  ASSERT_TRUE(mask2.all(0, 5));
  ASSERT_TRUE(not mask2.all(0, 6));
  ASSERT_TRUE(mask2.any(0, 5));
  ASSERT_TRUE(mask2.any());

  bounded_bitset<25> mask3(25);
  mask3.set(20);
  mask3.set(21);
  mask3.set(22);
  mask3.set(23);
  mask3.set(24);
  ASSERT_TRUE(mask3.all(20, 25));
  ASSERT_TRUE(not mask3.all(19, 25));
  ASSERT_TRUE(mask3.any(20, 25));
  ASSERT_TRUE(mask3.any());
}

TEST(BoundedBitset, bitwise_oper)
{
  bounded_bitset<25> mask(23);
  bounded_bitset<25> mask2(23);

  mask.set(10);
  ASSERT_TRUE(mask != mask2);
  mask2 |= mask;
  ASSERT_TRUE(mask == mask2);

  mask.set(11);
  mask2 &= mask;
  ASSERT_TRUE(mask != mask2);
  ASSERT_TRUE(mask2.test(10) and not mask2.test(11));
}

TEST(BoundedBitset, bitwise_format)
{
  {
    bounded_bitset<100> bitset(100);
    bitset.set(0);
    bitset.set(5);

    ASSERT_TRUE(fmt::format("{:x}", bitset) == "0000000000000000000000021");
    ASSERT_TRUE(fmt::format("{:b}", bitset) ==
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100001");

    bitset.set(99);
    ASSERT_TRUE(fmt::format("{:x}", bitset) == "8000000000000000000000021");
    ASSERT_TRUE(fmt::format("{:b}", bitset) ==
                "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100001");
  }

  {
    bounded_bitset<100> bitset(25);
    bitset.set(0);
    bitset.set(4);

    ASSERT_TRUE(fmt::format("{:x}", bitset) == "0000011");
    ASSERT_TRUE(fmt::format("{:b}", bitset) == "0000000000000000000010001");

    bitset.set(24);
    ASSERT_TRUE(fmt::format("{:x}", bitset) == "1000011");
    ASSERT_TRUE(fmt::format("{:b}", bitset) == "1000000000000000000010001");
  }

  {
    bounded_bitset<100, true> bitset(100);
    bitset.set(0);
    bitset.set(5);

    ASSERT_TRUE(fmt::format("{:x}", bitset) == "8400000000000000000000000");
    ASSERT_TRUE(fmt::format("{:b}", bitset) ==
                "1000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");

    bitset.set(99);
    ASSERT_TRUE(fmt::format("{:x}", bitset) == "8400000000000000000000001");
    ASSERT_TRUE(fmt::format("{:b}", bitset) ==
                "1000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001");
  }
}

TEST(BoundedBitset, bitwise_resize)
{
  {
    bounded_bitset<100> bitset;
    ASSERT_TRUE(bitset.none() and bitset.size() == 0);

    bitset.resize(100);
    ASSERT_TRUE(bitset.none() and bitset.size() == 100);
    bitset.fill(0, 100);
    ASSERT_TRUE(bitset.all() and bitset.size() == 100);

    bitset.resize(25);
    ASSERT_TRUE(bitset.to_uint64() == 0x1ffffff);
    ASSERT_TRUE(bitset.all() and bitset.size() == 25); // keeps the data it had for the non-erased bits

    bitset.resize(100);
    ASSERT_TRUE(bitset.count() == 25 and bitset.size() == 100);
  }

  {
    // TEST: Reverse case
    bounded_bitset<100, true> bitset;
    ASSERT_TRUE(bitset.none() and bitset.size() == 0);

    bitset.resize(100);
    ASSERT_TRUE(bitset.none() and bitset.size() == 100);
    bitset.fill(0, 100);
    ASSERT_TRUE(bitset.all() and bitset.size() == 100);

    bitset.resize(25);
    ASSERT_TRUE(bitset.to_uint64() == 0x1ffffff);
    ASSERT_TRUE(bitset.all() and bitset.size() == 25); // keeps the data it had for the non-erased bits

    bitset.resize(100);
    ASSERT_TRUE(bitset.count() == 25 and bitset.size() == 100);
  }
}

template <bool reversed>
void test_bitset_find()
{
  {
    bounded_bitset<25, reversed> bitset(6);

    // 0b000000
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size(), false) == 0);
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size(), true) == -1);

    // 0b000100
    bitset.set(2);
    ASSERT_TRUE(bitset.find_lowest(0, 6) == 2);
    ASSERT_TRUE(bitset.find_lowest(0, 6, false) == 0);
    ASSERT_TRUE(bitset.find_lowest(3, 6) == -1);
    ASSERT_TRUE(bitset.find_lowest(3, 6, false) == 3);

    // 0b000101
    bitset.set(0);
    ASSERT_TRUE(bitset.find_lowest(0, 6) == 0);
    ASSERT_TRUE(bitset.find_lowest(0, 6, false) == 1);
    ASSERT_TRUE(bitset.find_lowest(3, 6) == -1);
    ASSERT_TRUE(bitset.find_lowest(3, 6, false) == 3);

    // 0b100101
    bitset.set(5);
    ASSERT_TRUE(bitset.find_lowest(0, 6) == 0);
    ASSERT_TRUE(bitset.find_lowest(0, 6, false) == 1);
    ASSERT_TRUE(bitset.find_lowest(3, 6) == 5);

    // 0b111111
    bitset.fill(0, 6);
    ASSERT_TRUE(bitset.find_lowest(0, 6) == 0);
    ASSERT_TRUE(bitset.find_lowest(0, 6, false) == -1);
    ASSERT_TRUE(bitset.find_lowest(3, 6, false) == -1);
  }
  {
    bounded_bitset<100, reversed> bitset(95);

    // 0b0...0
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size()) == -1);

    // 0b1000...
    bitset.set(94);
    ASSERT_TRUE(bitset.find_lowest(0, 93) == -1);
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size()) == 94);

    // 0b1000...010
    bitset.set(1);
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size()) == 1);
    ASSERT_TRUE(bitset.find_lowest(1, bitset.size()) == 1);
    ASSERT_TRUE(bitset.find_lowest(2, bitset.size()) == 94);

    // 0b11..11
    bitset.fill(0, bitset.size());
    ASSERT_TRUE(bitset.find_lowest(0, bitset.size()) == 0);
    ASSERT_TRUE(bitset.find_lowest(5, bitset.size()) == 5);
  }
  {
    bounded_bitset<100, reversed> bitset(95);

    // 0b0...0
    ASSERT_TRUE(bitset.find_lowest() == -1);

    // 0b1000...
    bitset.set(94);
    ASSERT_TRUE(bitset.find_lowest() == 94);

    // 0b1000...010
    bitset.set(1);
    ASSERT_TRUE(bitset.find_lowest() == 1);

    // 0b11..11
    bitset.fill(0, bitset.size());
    ASSERT_TRUE(bitset.find_lowest() == 0);
  }
  {
    bounded_bitset<100, reversed> bitset(95);

    // 0b0...0
    ASSERT_TRUE(bitset.find_highest() == -1);

    // 0b1000...
    bitset.set(94);
    ASSERT_EQ(bitset.find_highest(), 94);

    // 0b1000...010
    bitset.set(1);
    ASSERT_EQ(bitset.find_highest(), 94);

    // 0b11..11
    bitset.fill(0, bitset.size());
    ASSERT_EQ(bitset.find_highest(), 94);
  }
}

TEST(BoundedBitset, bitset_find)
{
  test_bitset_find<false>();
  test_bitset_find<true>();
}

TEST(BoundedBitset, is_contiguous)
{
  // Test contiguous condition 1. No bit set.
  {
    ASSERT_TRUE(bounded_bitset<10>({0, 0, 0, 0}).is_contiguous());
  }
  // Test contiguous condition 2. One bit set.
  {
    ASSERT_TRUE(bounded_bitset<10>({1, 0, 0, 0}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 0, 1, 0}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 1, 0, 0}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 0, 0, 1}).is_contiguous());
  }
  // Test contiguous condition 3. All set bits are contiguous.
  {
    ASSERT_TRUE(bounded_bitset<10>({1, 1, 0, 0}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({1, 1, 1, 0}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({1, 1, 1, 1}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 1, 1, 1}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 0, 1, 1}).is_contiguous());
    ASSERT_TRUE(bounded_bitset<10>({0, 1, 1, 0}).is_contiguous());
  }
  // Not contiguous.
  {
    ASSERT_FALSE(bounded_bitset<10>({1, 0, 1, 1}).is_contiguous());
    ASSERT_FALSE(bounded_bitset<10>({1, 1, 0, 1}).is_contiguous());
    ASSERT_FALSE(bounded_bitset<10>({1, 0, 1, 1}).is_contiguous());
    ASSERT_FALSE(bounded_bitset<10>({1, 1, 0, 1}).is_contiguous());
    ASSERT_FALSE(bounded_bitset<10>({1, 0, 0, 1}).is_contiguous());
  }
}

TEST(BoundedBitset, push_back)
{
  {
    bounded_bitset<10> bitset;
    bitset.push_back(1);
    bitset.push_back(0);
    bitset.push_back(1);
    bitset.push_back(1);
    ASSERT_EQ(bitset, bounded_bitset<10>({1, 0, 1, 1}));
  }
  {
    bounded_bitset<10> bitset;
    bitset.push_back(0xbU, 4);
    ASSERT_EQ(bitset, bounded_bitset<10>({1, 0, 1, 1}));
  }
}

TEST(BoundedBitset, slice)
{
  bounded_bitset<100> big_bitmap(95);
  big_bitmap.fill(5, 70);

  bounded_bitset<35> small_bitmap = big_bitmap.slice<35>(50, 80);
  ASSERT_EQ(30, small_bitmap.size());
  ASSERT_EQ(20, small_bitmap.count());
  ASSERT_EQ(1048575, small_bitmap.to_uint64());
}

TEST(BoundedBitset, fold_and_accumulate)
{
  size_t fold_size = 20;

  bounded_bitset<105> big_bitmap(100);
  for (size_t i = 0; i < big_bitmap.size(); i += fold_size + 1) {
    big_bitmap.set(i);
  }

  bounded_bitset<6> fold_bitset = fold_and_accumulate<6, false>(big_bitmap, fold_size);
  ASSERT_EQ(big_bitmap.size() / fold_size, fold_bitset.count());
  ASSERT_EQ(fold_bitset.count(), big_bitmap.count());
  ASSERT_TRUE(fold_bitset.is_contiguous());
  ASSERT_EQ(0, fold_bitset.find_lowest());
}
