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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/support/test_utils.h"
#include <bitset>
#include <gtest/gtest.h>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsran;

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
  using IntegerType = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ(0, mask_lsb_ones<IntegerType>(0));
  ASSERT_EQ(static_cast<IntegerType>(-1), mask_lsb_ones<IntegerType>(this->nof_bits))
      << "for nof_bits=" << (unsigned)this->nof_bits;
  ASSERT_EQ(0b11, mask_lsb_ones<IntegerType>(2));

  // test all combinations.
  for (unsigned nof_ones = 0; nof_ones != this->nof_bits; ++nof_ones) {
    IntegerType expected = (static_cast<uint64_t>(1U) << nof_ones) - 1U;
    ASSERT_EQ(expected, mask_lsb_ones<IntegerType>(nof_ones)) << "for nof_ones=" << nof_ones;
  }
}

TYPED_TEST(bitmask_tester, lsb_zeros)
{
  using IntegerType = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ((IntegerType)-1, mask_lsb_zeros<IntegerType>(0));
  ASSERT_EQ(0, mask_lsb_zeros<IntegerType>(this->nof_bits));

  // test all combinations.
  for (unsigned nof_zeros = 0; nof_zeros != this->nof_bits; ++nof_zeros) {
    IntegerType expected = (static_cast<uint64_t>(1U) << nof_zeros) - 1U;
    expected             = ~expected;
    ASSERT_EQ(expected, mask_lsb_zeros<IntegerType>(nof_zeros)) << "for nof_zeros=" << nof_zeros;
    ASSERT_EQ((IntegerType)~mask_lsb_ones<IntegerType>(nof_zeros), mask_lsb_zeros<IntegerType>(nof_zeros));
  }
}

TYPED_TEST(bitmask_tester, msb_ones)
{
  using IntegerType = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ(0, mask_msb_ones<IntegerType>(0));
  ASSERT_EQ(static_cast<IntegerType>(-1), mask_msb_ones<IntegerType>(this->nof_bits));

  // test all combinations.
  for (unsigned nof_ones = 0; nof_ones != this->nof_bits; ++nof_ones) {
    IntegerType expected = 0;
    if (nof_ones > 0) {
      unsigned nof_lsb_zeros = this->nof_bits - nof_ones;
      expected               = ~((static_cast<IntegerType>(1U) << (nof_lsb_zeros)) - 1U);
    }
    ASSERT_EQ(expected, mask_msb_ones<IntegerType>(nof_ones)) << "for nof_ones=" << nof_ones;
  }
}

TYPED_TEST(bitmask_tester, msb_zeros)
{
  using IntegerType = typename TestFixture::Integer;
  // sanity checks.
  ASSERT_EQ((IntegerType)-1, mask_msb_zeros<IntegerType>(0));
  ASSERT_EQ(0, mask_msb_zeros<IntegerType>(this->nof_bits));

  // test all combinations.
  for (unsigned nof_zeros = 0; nof_zeros != this->nof_bits; ++nof_zeros) {
    IntegerType expected = 0;
    if (nof_zeros > 0) {
      unsigned nof_lsb_ones = this->nof_bits - nof_zeros;
      expected              = ~((static_cast<IntegerType>(1U) << (nof_lsb_ones)) - 1U);
    }
    expected = ~expected;
    ASSERT_EQ(expected, mask_msb_zeros<IntegerType>(nof_zeros)) << "for nof_zeros=" << nof_zeros;
    ASSERT_EQ((IntegerType)~mask_lsb_ones<IntegerType>(nof_zeros), mask_lsb_zeros<IntegerType>(nof_zeros));
  }
}

TYPED_TEST(bitmask_tester, first_lsb_one)
{
  using IntegerType = typename TestFixture::Integer;
  std::uniform_int_distribution<IntegerType> rd_int{0, std::numeric_limits<IntegerType>::max()};

  // sanity checks.
  ASSERT_EQ(std::numeric_limits<IntegerType>::digits, find_first_lsb_one<IntegerType>(0));
  ASSERT_EQ(0, find_first_lsb_one<IntegerType>(-1));
  ASSERT_EQ(0, find_first_lsb_one<IntegerType>(0b1));
  ASSERT_EQ(1, find_first_lsb_one<IntegerType>(0b10));
  ASSERT_EQ(0, find_first_lsb_one<IntegerType>(0b11));

  // test all combinations.
  for (unsigned one_idx = 0; one_idx != this->nof_bits - 1; ++one_idx) {
    IntegerType mask  = mask_lsb_zeros<IntegerType>(one_idx);
    IntegerType value = rd_int(test_rgen::get()) & mask;

    ASSERT_EQ(find_first_lsb_one(mask), one_idx);
    ASSERT_GE(find_first_lsb_one(value), one_idx) << fmt::format("for value {:#b}", value);
  }
}

TYPED_TEST(bitmask_tester, first_msb_one)
{
  using IntegerType = typename TestFixture::Integer;
  std::uniform_int_distribution<IntegerType> rd_int{0, std::numeric_limits<IntegerType>::max()};

  // sanity checks.
  ASSERT_EQ(std::numeric_limits<IntegerType>::digits, find_first_msb_one<IntegerType>(0));
  ASSERT_EQ(this->nof_bits - 1, find_first_msb_one<IntegerType>(-1));
  ASSERT_EQ(0, find_first_msb_one<IntegerType>(0b1));
  ASSERT_EQ(1, find_first_msb_one<IntegerType>(0b10));
  ASSERT_EQ(1, find_first_msb_one<IntegerType>(0b11));

  // test all combinations.
  for (unsigned one_idx = 0; one_idx != this->nof_bits - 1; ++one_idx) {
    IntegerType mask  = mask_lsb_ones<IntegerType>(one_idx + 1);
    IntegerType value = std::max((IntegerType)(rd_int(test_rgen::get()) & mask), (IntegerType)1U);

    ASSERT_EQ(one_idx, find_first_msb_one(mask));
    ASSERT_LE(find_first_msb_one(value), one_idx) << fmt::format("for value {:#b}", value);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename BoundedBitset>
class bounded_bitset_tester : public ::testing::Test
{
protected:
  using bitset_type         = BoundedBitset;
  using reverse_bitset_type = bounded_bitset<bitset_type::max_size(), not bitset_type::bit_order()>;

  static constexpr unsigned max_size() { return bitset_type::max_size(); }

  unsigned get_random_size(unsigned min_val = 1, unsigned max_val = bitset_type::max_size()) const
  {
    srsran_assert(max_val <= bitset_type::max_size(), "Invalid test bitset size argument");
    return test_rgen::uniform_int<unsigned>(min_val, max_val);
  }

  bitset_type create_bitset_with_zeros(unsigned size) const { return bitset_type(size); }

  static bitset_type create_bitset_with_ones(unsigned size)
  {
    std::vector<bool> data(size, true);
    return bitset_type(data.begin(), data.end());
  }

  static bitset_type create_random_bitset(unsigned size)
  {
    std::vector<bool> data(size);
    for (auto it = data.begin(); it != data.end(); ++it) {
      *it = std::bernoulli_distribution{}(test_rgen::get());
    }
    return bitset_type(data.begin(), data.end());
  }

  static std::vector<bool> create_random_vector(unsigned size)
  {
    std::vector<bool> vec(size);
    for (auto it = vec.begin(); it != vec.end(); ++it) {
      *it = std::bernoulli_distribution{}(test_rgen::get());
    }
    return vec;
  }
};
using bitset_types = ::testing::Types<bounded_bitset<25>,
                                      bounded_bitset<25, true>,
                                      bounded_bitset<120>,
                                      bounded_bitset<120, true>,
                                      bounded_bitset<512>,
                                      bounded_bitset<512, true>>;
TYPED_TEST_SUITE(bounded_bitset_tester, bitset_types);

TYPED_TEST(bounded_bitset_tester, all_zeros)
{
  auto zero_mask = this->create_bitset_with_zeros(this->get_random_size());
  ASSERT_GT(zero_mask.size(), 0);
  ASSERT_EQ(0, zero_mask.count());
  ASSERT_TRUE(zero_mask.none());
  ASSERT_FALSE(zero_mask.any());
  ASSERT_FALSE(zero_mask.all());
}

TYPED_TEST(bounded_bitset_tester, empty_bitset)
{
  auto zero_sized_mask = this->create_bitset_with_zeros(0);
  ASSERT_EQ(this->max_size(), zero_sized_mask.max_size());
  ASSERT_EQ(0, zero_sized_mask.size());
  ASSERT_EQ(0, zero_sized_mask.count());
  ASSERT_TRUE(zero_sized_mask.none());
  ASSERT_FALSE(zero_sized_mask.any());
  ASSERT_TRUE(zero_sized_mask.all());

  zero_sized_mask.flip();
  ASSERT_TRUE(zero_sized_mask.none()) << "Flipping empty bitset should be a no-op";
  ASSERT_EQ(0, zero_sized_mask.size()) << "Flipping empty bitset should be a no-op";

  auto zero_mask = this->create_bitset_with_zeros(this->get_random_size());
  ASSERT_NE(zero_mask, zero_sized_mask);
  zero_sized_mask.resize(zero_mask.size());
  ASSERT_EQ(zero_mask, zero_sized_mask);
}

TYPED_TEST(bounded_bitset_tester, initializer_list_constructor)
{
  typename TestFixture::bitset_type bitmap = {true, true, true, true, false};
  ASSERT_EQ(bitmap.size(), 5);
  ASSERT_TRUE(bitmap.test(0));
  ASSERT_TRUE(bitmap.test(1));
  ASSERT_TRUE(bitmap.test(3));
  ASSERT_FALSE(bitmap.test(4));
}

TYPED_TEST(bounded_bitset_tester, iterator_constructor)
{
  std::vector<bool>                 data = this->create_random_vector(this->get_random_size());
  typename TestFixture::bitset_type bitmap(data.begin(), data.end());
  ASSERT_EQ(bitmap.size(), data.size());

  for (unsigned i = 0; i != data.size(); ++i) {
    ASSERT_EQ(data[i], bitmap.test(i));
  }
}

TYPED_TEST(bounded_bitset_tester, all_ones)
{
  typename TestFixture::bitset_type ones_bitmap = this->create_bitset_with_ones(this->get_random_size());

  ASSERT_TRUE(ones_bitmap.all());
  ASSERT_TRUE(ones_bitmap.any());
  ASSERT_FALSE(ones_bitmap.none());
  ASSERT_EQ(ones_bitmap.size(), ones_bitmap.count());
}

TYPED_TEST(bounded_bitset_tester, flip)
{
  auto bitmap          = this->create_random_bitset(this->get_random_size());
  auto original_bitmap = bitmap;
  bitmap.flip();

  ASSERT_GT(bitmap.size(), 0);
  ASSERT_EQ(bitmap.size(), original_bitmap.size());
  for (unsigned i = 0; i != bitmap.size(); ++i) {
    ASSERT_NE(bitmap.test(i), original_bitmap.test(i));
  }
}

TYPED_TEST(bounded_bitset_tester, set_bit)
{
  std::vector<bool>                 data = this->create_random_vector(this->get_random_size());
  typename TestFixture::bitset_type bitmap(data.size());
  typename TestFixture::bitset_type expected_bitmap(data.begin(), data.end());

  for (unsigned i = 0; i != data.size(); ++i) {
    if (data[i]) {
      bitmap.set(i);
    }
  }
  ASSERT_EQ(bitmap, expected_bitmap) << fmt::format("failed {} != {}", bitmap, expected_bitmap);
}

TYPED_TEST(bounded_bitset_tester, bitwise_or)
{
  typename TestFixture::bitset_type bitmap       = this->create_random_bitset(this->get_random_size());
  typename TestFixture::bitset_type zeros_bitmap = this->create_bitset_with_zeros(bitmap.size());
  typename TestFixture::bitset_type ones_bitmap  = this->create_bitset_with_ones(bitmap.size());

  ASSERT_EQ(bitmap | bitmap, bitmap);
  ASSERT_EQ(bitmap | zeros_bitmap, bitmap);
  ASSERT_EQ(zeros_bitmap | bitmap, bitmap);
  ASSERT_EQ(bitmap | ones_bitmap, ones_bitmap);
  ASSERT_EQ(ones_bitmap | bitmap, ones_bitmap);

  auto flipped_bitmap = bitmap;
  flipped_bitmap.flip();
  ASSERT_EQ(flipped_bitmap | bitmap, this->create_bitset_with_ones(bitmap.size()));
  ASSERT_EQ(bitmap | flipped_bitmap, this->create_bitset_with_ones(bitmap.size()));

  bitmap |= flipped_bitmap;
  ASSERT_EQ(bitmap, this->create_bitset_with_ones(bitmap.size()));
}

TYPED_TEST(bounded_bitset_tester, bitwise_and)
{
  typename TestFixture::bitset_type bitmap       = this->create_random_bitset(this->get_random_size());
  typename TestFixture::bitset_type zeros_bitmap = this->create_bitset_with_zeros(bitmap.size());
  typename TestFixture::bitset_type ones_bitmap  = this->create_bitset_with_ones(bitmap.size());

  ASSERT_EQ(bitmap & bitmap, bitmap);
  ASSERT_EQ(bitmap & zeros_bitmap, zeros_bitmap);
  ASSERT_EQ(zeros_bitmap & bitmap, zeros_bitmap);
  ASSERT_EQ(bitmap & ones_bitmap, bitmap);
  ASSERT_EQ(ones_bitmap & bitmap, bitmap);

  auto flipped_bitmap = bitmap;
  flipped_bitmap.flip();
  ASSERT_EQ(flipped_bitmap & bitmap, this->create_bitset_with_zeros(bitmap.size()));
  ASSERT_EQ(bitmap & flipped_bitmap, this->create_bitset_with_zeros(bitmap.size()));

  bitmap &= flipped_bitmap;
  ASSERT_EQ(bitmap, this->create_bitset_with_zeros(bitmap.size()));
}

TYPED_TEST(bounded_bitset_tester, any_range)
{
  std::vector<bool>                 vec = this->create_random_vector(this->get_random_size());
  typename TestFixture::bitset_type bitmap(vec.begin(), vec.end());

  for (unsigned l = 1; l < vec.size(); ++l) {
    for (unsigned i = 0; i < vec.size() - l; ++i) {
      bool expected_val = false;
      for (unsigned j = 0; j != l; ++j) {
        if (vec[i + j]) {
          expected_val = true;
          break;
        }
      }
      ASSERT_EQ(expected_val, bitmap.any(i, i + l))
          << fmt::format("For bitmap={:x} of size={} in [{}, {})", bitmap, bitmap.size(), i, i + l);
    }
  }
}

TYPED_TEST(bounded_bitset_tester, all_range)
{
  std::vector<bool>                 vec = this->create_random_vector(this->get_random_size());
  typename TestFixture::bitset_type bitmap(vec.begin(), vec.end());

  for (unsigned l = 1; l < vec.size(); ++l) {
    for (unsigned i = 0; i < vec.size() - l; ++i) {
      bool expected_val = true;
      for (unsigned j = 0; j != l; ++j) {
        if (not vec[i + j]) {
          expected_val = false;
          break;
        }
      }
      ASSERT_EQ(expected_val, bitmap.all(i, i + l))
          << fmt::format("For bitmap={:x} of size={} in [{}, {})", bitmap, bitmap.size(), i, i + l);
    }
  }
}

TYPED_TEST(bounded_bitset_tester, fill_ones)
{
  unsigned   bitset_size  = this->get_random_size();
  const auto zeros_bitmap = this->create_bitset_with_zeros(bitset_size);

  for (unsigned l = 1; l < bitset_size; ++l) {
    for (unsigned i = 0; i < bitset_size - l; ++i) {
      auto bitmap = zeros_bitmap;
      bitmap.fill(i, i + l);
      ASSERT_FALSE(bitmap.any(0, i));
      ASSERT_TRUE(bitmap.all(i, i + l)) << fmt::format(
          "For bitmap={:x} of size={} in [{}, {})", bitmap, bitmap.size(), i, i + l);
      ASSERT_FALSE(bitmap.any(i + l, bitset_size));
    }
  }
}

TYPED_TEST(bounded_bitset_tester, fill_zeros)
{
  unsigned   bitset_size = this->get_random_size();
  const auto ones_bitmap = this->create_bitset_with_ones(bitset_size);

  for (unsigned l = 1; l < bitset_size; ++l) {
    for (unsigned i = 0; i < bitset_size - l; ++i) {
      auto bitmap = ones_bitmap;
      bitmap.fill(i, i + l, false);
      ASSERT_TRUE(bitmap.all(0, i)) << fmt::format(
          "For bitmap={:x} of size={} in [{}, {})", bitmap, bitmap.size(), i, i + l);
      ASSERT_FALSE(bitmap.any(i, i + l));
      ASSERT_TRUE(bitmap.all(i + l, bitset_size))
          << fmt::format("For bitmap={:x} of size={} in [{}, {})", bitmap, bitmap.size(), i, i + l);
    }
  }
}

TYPED_TEST(bounded_bitset_tester, slice)
{
  using big_bitset_type                   = typename TestFixture::bitset_type;
  unsigned                big_bitset_size = this->get_random_size();
  const std::vector<bool> vec             = this->create_random_vector(big_bitset_size);
  big_bitset_type         big_bitmap(vec.begin(), vec.end());

  constexpr size_t N_small   = TestFixture::bitset_type::max_size() / 2;
  using small_bitset_type    = bounded_bitset<N_small, big_bitset_type::bit_order()>;
  unsigned small_bitset_size = test_rgen::uniform_int<unsigned>(0, std::min((unsigned)N_small, big_bitset_size - 1));
  unsigned offset            = test_rgen::uniform_int<unsigned>(0, small_bitset_size);
  unsigned end_offset        = test_rgen::uniform_int<unsigned>(offset, small_bitset_size);

  small_bitset_type small_bitmap = big_bitmap.template slice<N_small>(offset, end_offset);
  ASSERT_EQ(end_offset - offset, small_bitmap.size());

  for (unsigned i = 0; i != small_bitmap.size(); ++i) {
    ASSERT_EQ(small_bitmap.test(i), big_bitmap.test(i + offset))
        << fmt::format("For slice={} of size={} taken from [{}, {}) of big_bitmap={} of size={}",
                       small_bitmap,
                       small_bitmap.size(),
                       offset,
                       end_offset,
                       big_bitmap,
                       big_bitmap.size());
  }
}

TYPED_TEST(bounded_bitset_tester, resize_keeps_existing_bit_values)
{
  const unsigned large_size   = this->get_random_size(2);
  const unsigned smaller_size = this->get_random_size(1, large_size - 1);

  auto ones_bitmap = this->create_bitset_with_ones(large_size);
  ASSERT_EQ(ones_bitmap.count(), large_size);
  ones_bitmap.resize(smaller_size);
  ASSERT_EQ(ones_bitmap.count(), smaller_size);
  ones_bitmap.resize(large_size);
  ASSERT_EQ(ones_bitmap.count(), smaller_size);
}

TYPED_TEST(bounded_bitset_tester, copy_preserves_size_and_values)
{
  const unsigned size1 = this->get_random_size();
  const unsigned size2 = this->get_random_size();

  typename TestFixture::bitset_type bitset1 = this->create_random_bitset(size1);
  typename TestFixture::bitset_type bitset2 = this->create_random_bitset(size2);

  bitset2 = bitset1;
  ASSERT_EQ(bitset1, bitset2);
}

TYPED_TEST(bounded_bitset_tester, binary_format_is_mirror_of_binary_format_reverse)
{
  static constexpr size_t         N        = TestFixture::bitset_type::max_size();
  static constexpr bool           BitOrder = TestFixture::bitset_type::bit_order();
  std::vector<bool>               vec      = this->create_random_vector(this->get_random_size());
  bounded_bitset<N, BitOrder>     bitmap(vec.begin(), vec.end());
  bounded_bitset<N, not BitOrder> bitmap_reversed(vec.begin(), vec.end());

  std::string str          = fmt::format("{:b}", bitmap);
  std::string str_reverse  = fmt::format("{:br}", bitmap);
  std::string str2         = fmt::format("{:b}", bitmap_reversed);
  std::string str2_reverse = fmt::format("{:br}", bitmap_reversed);

  ASSERT_TRUE(std::equal(str.begin(), str.end(), str_reverse.rbegin(), str_reverse.rend()));
  ASSERT_TRUE(std::equal(str2.begin(), str2.end(), str2_reverse.rbegin(), str2_reverse.rend()));
  ASSERT_EQ(str, str2_reverse);
}

TYPED_TEST(bounded_bitset_tester, bit_positions_format_is_agnostic_to_reverse)
{
  static constexpr size_t         N        = TestFixture::bitset_type::max_size();
  static constexpr bool           BitOrder = TestFixture::bitset_type::bit_order();
  std::vector<bool>               vec      = this->create_random_vector(this->get_random_size());
  bounded_bitset<N, BitOrder>     bitmap(vec.begin(), vec.end());
  bounded_bitset<N, not BitOrder> bitmap_reversed(vec.begin(), vec.end());

  std::string str         = fmt::format("{:n}", bitmap);
  std::string str_reverse = fmt::format("{:n}", bitmap_reversed);

  ASSERT_EQ(str, str_reverse);
}

TYPED_TEST(bounded_bitset_tester, hex_format_is_mirror_of_hex_format_reverse)
{
  static constexpr size_t         N        = TestFixture::bitset_type::max_size();
  static constexpr bool           BitOrder = TestFixture::bitset_type::bit_order();
  std::vector<bool>               vec      = this->create_random_vector(this->get_random_size());
  bounded_bitset<N, BitOrder>     bitmap(vec.begin(), vec.end());
  bounded_bitset<N, not BitOrder> bitmap_reversed(vec.begin(), vec.end());

  std::string str          = fmt::format("{:b}", bitmap);
  std::string str_reverse  = fmt::format("{:br}", bitmap);
  std::string str2         = fmt::format("{:b}", bitmap_reversed);
  std::string str2_reverse = fmt::format("{:br}", bitmap_reversed);

  ASSERT_EQ(str_reverse, str_reverse);
  ASSERT_EQ(str, str2_reverse);
}

TEST(bounded_bitset_test, bitset_integer_conversion_consistent_with_std_bitset)
{
  unsigned bitset_size = 23;
  unsigned nof_ones    = test_rgen::uniform_int<unsigned>(1, bitset_size);
  unsigned integermask = mask_lsb_ones<unsigned>(nof_ones);

  bounded_bitset<25> mask(bitset_size);
  mask.from_uint64(integermask);
  std::bitset<25> std_mask(integermask);

  ASSERT_EQ(bitset_size, mask.size());
  ASSERT_EQ(25, std_mask.size());
  ASSERT_EQ(nof_ones, mask.count());
  ASSERT_EQ(nof_ones, std_mask.count());
  ASSERT_TRUE(mask.any());
  ASSERT_TRUE(std_mask.any());
  ASSERT_FALSE(mask.none());
  ASSERT_FALSE(std_mask.none());
  ASSERT_EQ(integermask, mask.to_uint64());
  ASSERT_EQ(integermask, std_mask.to_ulong());
}

TEST(bounded_bitset_test, bitset_integer_conversion_with_template_arg)
{
  unsigned bitset_size = 23;

  bounded_bitset<25>       mask(bitset_size);
  bounded_bitset<25, true> mask_reversed(bitset_size);
  std::bitset<25>          std_mask;

  mask.set(3);
  mask_reversed.set(3);
  std_mask.set(3);

  ASSERT_EQ(mask.to_uint64(), 0b1000);
  ASSERT_EQ(mask_reversed.to_uint64(), 1U << (bitset_size - 4));
  ASSERT_EQ(std_mask.to_ulong(), 0b1000);
}

TEST(bounded_bitset_test, one_word_bitset_format)
{
  bounded_bitset<25> bitset(23);
  bitset.set(0);
  bitset.set(5);
  bounded_bitset<25, true> bitset_reversed(23);
  bitset_reversed.set(0);
  bitset_reversed.set(5);

  ASSERT_EQ(fmt::format("{:b}", bitset), "00000000000000000100001");
  ASSERT_EQ(fmt::format("{:br}", bitset), "10000100000000000000000");
  ASSERT_EQ(fmt::format("{:x}", bitset), "000021");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "420000");
  ASSERT_EQ(fmt::format("{:n}", bitset), "0 5");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "0 5");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:n}", bitset), fmt::format("{:nr}", bitset));

  bitset.set(22);
  bitset_reversed.set(22);
  ASSERT_EQ(fmt::format("{:b}", bitset), "10000000000000000100001");
  ASSERT_EQ(fmt::format("{:br}", bitset), "10000100000000000000001");
  ASSERT_EQ(fmt::format("{:x}", bitset), "400021");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "420001");
  ASSERT_EQ(fmt::format("{:n}", bitset), "0 5 22");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "0 5 22");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
}

TEST(bounded_bitset_test, contiguous_bitset_format)
{
  bounded_bitset<25> bitset(9);
  bitset.set(1);
  bitset.set(2);
  bitset.set(3);
  bounded_bitset<25, true> bitset_reversed(9);
  bitset_reversed.set(1);
  bitset_reversed.set(2);
  bitset_reversed.set(3);

  ASSERT_EQ(fmt::format("{:b}", bitset), "000001110");
  ASSERT_EQ(fmt::format("{:br}", bitset), "011100000");
  ASSERT_EQ(fmt::format("{:x}", bitset), "00e");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "0e0");
  ASSERT_EQ(fmt::format("{:n}", bitset), "[1, 4)");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "[1, 4)");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:n}", bitset), fmt::format("{:nr}", bitset));

  bitset.reset();
  bitset_reversed.reset();
  bitset.set(1);
  bitset_reversed.set(1);
  bitset.set(4);
  bitset_reversed.set(4);
  bitset.set(5);
  bitset_reversed.set(5);
  bitset.set(7);
  bitset_reversed.set(7);
  bitset.set(8);
  bitset_reversed.set(8);

  ASSERT_EQ(fmt::format("{:b}", bitset), "110110010");
  ASSERT_EQ(fmt::format("{:br}", bitset), "010011011");
  ASSERT_EQ(fmt::format("{:x}", bitset), "1b2");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "09b");
  ASSERT_EQ(fmt::format("{:n}", bitset), "1 4 5 7 8");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "1 4 5 7 8");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:n}", bitset), fmt::format("{:nr}", bitset));
}

TEST(bounded_bitset_test, two_word_bitset_format)
{
  bounded_bitset<101> bitset(100);
  bitset.set(0);
  bitset.set(5);
  bounded_bitset<101, true> bitset_reversed(100);
  bitset_reversed.set(0);
  bitset_reversed.set(5);

  ASSERT_EQ(fmt::format("{:b}", bitset),
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100001");
  ASSERT_EQ(fmt::format("{:br}", bitset),
            "1000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
  ASSERT_EQ(fmt::format("{:x}", bitset), "0000000000000000000000021");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "8400000000000000000000000");
  ASSERT_EQ(fmt::format("{:n}", bitset), "0 5");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "0 5");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:n}", bitset), fmt::format("{:nr}", bitset));

  bitset.set(99);
  bitset_reversed.set(99);
  ASSERT_EQ(fmt::format("{:b}", bitset),
            "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100001");
  ASSERT_EQ(fmt::format("{:br}", bitset),
            "1000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001");
  ASSERT_EQ(fmt::format("{:x}", bitset), "8000000000000000000000021");
  ASSERT_EQ(fmt::format("{:xr}", bitset), "8400000000000000000000001");
  ASSERT_EQ(fmt::format("{:n}", bitset), "0 5 99");
  ASSERT_EQ(fmt::format("{:n}", bitset_reversed), "0 5 99");
  ASSERT_EQ(fmt::format("{:b}", bitset), fmt::format("{:br}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:br}", bitset), fmt::format("{:b}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:x}", bitset), fmt::format("{:xr}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:xr}", bitset), fmt::format("{:x}", bitset_reversed));
  ASSERT_EQ(fmt::format("{:n}", bitset), fmt::format("{:nr}", bitset));
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

TEST(BoundedBitset, extract)
{
  static constexpr uint64_t            nof_values = 100;
  bounded_bitset<64 * nof_values>      bitset;
  std::vector<std::array<uint64_t, 3>> data;

  for (unsigned i_value = 0, offset = 0; i_value != nof_values; ++i_value) {
    uint64_t size  = test_rgen::uniform_int(1, 64);
    uint64_t value = (test_rgen::get()() & mask_lsb_ones<uint64_t>(size));

    bitset.push_back(value, size);

    data.push_back({offset, size, value});

    offset += size;
  }

  for (const auto& entry : data) {
    uint64_t offset = entry[0];
    uint64_t size   = entry[1];
    uint64_t value  = entry[2];

    ASSERT_EQ(bitset.extract<uint64_t>(offset, size), value);

    offset += size;
  }
}

TEST(BoundedBitset, extract_inverted)
{
  static constexpr uint64_t             nof_values = 10;
  bounded_bitset<64 * nof_values, true> bitset;
  std::vector<std::array<uint64_t, 3>>  data;

  for (unsigned i_value = 0, offset = 0; i_value != nof_values; ++i_value) {
    uint64_t size  = test_rgen::uniform_int(1, 64);
    uint64_t value = (test_rgen::get()() & mask_lsb_ones<uint64_t>(size));

    bitset.push_back(value, size);

    data.push_back({offset, size, value});

    offset += size;
  }

  for (const auto& entry : data) {
    uint64_t offset = entry[0];
    uint64_t size   = entry[1];
    uint64_t value  = entry[2];

    ASSERT_EQ(bitset.extract<uint64_t>(offset, size), value);

    offset += size;
  }
}

TEST(BoundedBitset, fold_and_accumulate)
{
  size_t fold_size = 20;

  bounded_bitset<105> big_bitmap(100);
  for (size_t i = 0; i < big_bitmap.size(); i += fold_size + 1) {
    big_bitmap.set(i);
  }

  bounded_bitset<21> fold_bitset = fold_and_accumulate<21>(big_bitmap, fold_size);
  ASSERT_EQ(big_bitmap.size() / fold_size, fold_bitset.count());
  ASSERT_EQ(fold_bitset.count(), big_bitmap.count());
  ASSERT_TRUE(fold_bitset.is_contiguous());
  ASSERT_EQ(0, fold_bitset.find_lowest());
}

TEST(BoundedBitset, for_each)
{
  bounded_bitset<10>  mask   = {false, true, false, true, false, true, false, true, false, true};
  std::array<int, 10> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::vector<int>    output;

  mask.for_each(0, mask.size(), [&output, &values](int n) { output.emplace_back(values[n]); });
  ASSERT_EQ(output, std::vector<int>({2, 4, 6, 8, 10}));

  output.resize(0);
  mask.for_each(
      0, mask.size(), [&output, &values](int n) { output.emplace_back(values[n]); }, false);
  ASSERT_EQ(output, std::vector<int>({1, 3, 5, 7, 9}));

  output.resize(0);
  mask.for_each(2, mask.size(), [&output, &values](int n) { output.emplace_back(values[n]); });
  ASSERT_EQ(output, std::vector<int>({4, 6, 8, 10}));

  output.resize(0);
  mask.for_each(0, 9, [&output, &values](int n) { output.emplace_back(values[n]); });
  ASSERT_EQ(output, std::vector<int>({2, 4, 6, 8}));
}

TEST(bounded_bitset_test, to_packed_bits_one_byte)
{
  bounded_bitset<10>       bitset{true, true, false, false, true};
  bounded_bitset<10, true> bitset_rev{true, true, false, false, true};

  std::array<uint8_t, 1> packed_bits = {};
  ASSERT_EQ(bitset.to_packed_bits(span<uint8_t>{packed_bits}), 1);
  ASSERT_EQ(packed_bits[0], 0b00010011);
  ASSERT_EQ(bitset_rev.to_packed_bits(span<uint8_t>{packed_bits}), 1);
  ASSERT_EQ(packed_bits[0], 0b11001000);
}

TEST(bounded_bitset_test, to_packed_bits_two_byte)
{
  bounded_bitset<20>       bitset(15);
  bounded_bitset<20, true> bitset_rev(15);
  bitset.set(0);
  bitset.set(1);
  bitset.set(9);
  bitset_rev.set(0);
  bitset_rev.set(1);
  bitset_rev.set(9);

  std::array<uint8_t, 3> packed_bits = {}, packed_bits2 = {};
  ASSERT_EQ(bitset.to_packed_bits(span<uint8_t>{packed_bits}), 2);
  std::array<uint8_t, 2> expected_packed_bits = {0b00000011, 0b00000010};
  ASSERT_TRUE(std::equal(expected_packed_bits.begin(), expected_packed_bits.end(), packed_bits.begin()));
  ASSERT_EQ(bitset_rev.to_packed_bits(span<uint8_t>{packed_bits2}), 2);
  std::array<uint8_t, 2> expected_packed_bits2 = {0b11000000, 0b01000000};
  ASSERT_TRUE(std::equal(expected_packed_bits2.begin(), expected_packed_bits2.end(), packed_bits2.begin()));
}
