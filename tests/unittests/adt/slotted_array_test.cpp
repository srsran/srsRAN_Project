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

#include "srsran/adt/slotted_array.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsran;

std::random_device rd;
std::mt19937       g(rd());

static unsigned get_random_int(int lb = 0, int ub = std::numeric_limits<int>::max())
{
  return std::uniform_int_distribution<int>{lb, ub}(g);
}

static_assert(std::is_same<slotted_array<int, 5>::value_type, int>::value, "Invalid container value_type");
static_assert(std::is_same<slotted_array<int, 5>::iterator::value_type, int>::value, "Invalid container value_type");
static_assert(std::is_same<slotted_array<int, 5>::const_iterator::value_type, int>::value,
              "Invalid container value_type");
static_assert(std::is_same<slotted_vector<int>::value_type, int>::value, "Invalid container value_type");
static_assert(std::is_same<slotted_vector<int>::iterator::value_type, int>::value, "Invalid container value_type");
static_assert(std::is_same<slotted_vector<int>::const_iterator::value_type, int>::value,
              "Invalid container value_type");

template <typename T>
class slotted_array_tester : public ::testing::Test
{
protected:
  using SlotArrayType = T;
  using ValueType     = typename SlotArrayType::value_type;

  ValueType create_elem(int val) { return ValueType{val}; }

  SlotArrayType vec;
};
using slotted_array_types = ::testing::Types<slotted_array<int, 20>,
                                             slotted_array<moveonly_test_object, 20>,
                                             slotted_vector<int>,
                                             slotted_vector<moveonly_test_object>>;
TYPED_TEST_SUITE(slotted_array_tester, slotted_array_types);

TYPED_TEST(slotted_array_tester, default_ctor_creates_empty_array)
{
  ASSERT_TRUE(this->vec.empty());
  ASSERT_EQ(this->vec.size(), 0);
  ASSERT_EQ(this->vec.begin(), this->vec.end());
  ASSERT_FALSE(this->vec.contains(get_random_int(0, 1000)));
}

TYPED_TEST(slotted_array_tester, insert_creates_entry_in_slotted_array)
{
  int      value = get_random_int();
  unsigned idx   = get_random_int(0, 19);

  this->vec.insert(idx, this->create_elem(value));
  ASSERT_EQ(this->vec.size(), 1);
  ASSERT_FALSE(this->vec.empty());
  ASSERT_TRUE(this->vec.contains(idx));
  ASSERT_EQ(this->vec[idx], this->create_elem(value));
  ASSERT_NE(this->vec.begin(), this->vec.end());
  ASSERT_EQ(*this->vec.begin(), this->create_elem(value));
}

TYPED_TEST(slotted_array_tester, emplace_constructs_element_in_slotted_array)
{
  int      value = get_random_int();
  unsigned idx   = get_random_int(0, 19);

  this->vec.emplace(idx, value);
  ASSERT_EQ(this->vec.size(), 1);
  ASSERT_FALSE(this->vec.empty());
  ASSERT_TRUE(this->vec.contains(idx));
  ASSERT_EQ(this->vec[idx], this->create_elem(value));
  ASSERT_NE(this->vec.begin(), this->vec.end());
  ASSERT_EQ(*this->vec.begin(), this->create_elem(value));
}

TYPED_TEST(slotted_array_tester, insert_in_already_inserted_position_does_not_alter_slotted_array_size)
{
  int      value = get_random_int(), value2 = get_random_int();
  unsigned idx = get_random_int(0, 19);

  ASSERT_TRUE(this->vec.insert(idx, this->create_elem(value)));
  ASSERT_FALSE(this->vec.insert(idx, this->create_elem(value2)));
  ASSERT_EQ(this->vec.size(), 1);
  ASSERT_TRUE(this->vec.contains(idx));
  ASSERT_EQ(this->vec[idx], this->create_elem(value));
  ASSERT_EQ(*this->vec.begin(), this->create_elem(value));
}

TYPED_TEST(slotted_array_tester, iterator_skips_empty_positions)
{
  std::vector<int> values;
  for (unsigned i = get_random_int(0, 19); i < 20; i += get_random_int(1, 5)) {
    values.push_back(get_random_int());
    this->vec.insert(i, this->create_elem(values.back()));
  }

  ASSERT_EQ(this->vec.size(), values.size());
  size_t count = 0;
  for (const auto& e : this->vec) {
    ASSERT_EQ(e, this->create_elem(values[count++]));
  }
}

TYPED_TEST(slotted_array_tester, erase_removes_element_from_slotted_array_and_updates_size)
{
  int      value = get_random_int();
  unsigned idx   = get_random_int(0, 19);

  this->vec.insert(idx, this->create_elem(value));
  ASSERT_FALSE(this->vec.empty());
  this->vec.erase(idx);
  ASSERT_TRUE(this->vec.empty());
  ASSERT_EQ(this->vec.size(), 0);
  ASSERT_EQ(this->vec.begin(), this->vec.end());
}

#ifdef ASSERTS_ENABLED
TYPED_TEST(slotted_array_tester, accessing_empty_position_asserts)
{
  unsigned idx  = get_random_int(0, 19);
  unsigned idx2 = (idx + get_random_int(1, 18)) % 20;

  this->vec.insert(idx, this->create_elem(get_random_int()));
  ASSERT_DEATH(this->vec[idx2], ".*") << fmt::format(
      "Accessing index={} for slotted_array with element in index={}", idx2, idx);
}
#endif

TYPED_TEST(slotted_array_tester, find_first_empty_skips_occupied_positions)
{
  unsigned nof_inserted = get_random_int(0, 19);

  for (unsigned i = 0; i != nof_inserted; ++i) {
    this->vec.insert(i, this->create_elem(get_random_int()));
  }
  ASSERT_EQ(this->vec.size(), nof_inserted);
  ASSERT_EQ(this->vec.find_first_empty(), nof_inserted);
}

TYPED_TEST(slotted_array_tester, iterator_converts_to_const_iterator)
{
  using SlotArray = typename TestFixture::SlotArrayType;
  this->vec.emplace(get_random_int(0, 19), get_random_int());
  const SlotArray& vec_ref = this->vec;

  auto it  = this->vec.begin();
  auto it2 = vec_ref.begin();
  ASSERT_EQ(it, it2);
  it2 = it;
  ASSERT_EQ(it, it2);
}

TYPED_TEST(slotted_array_tester, removed_last_index_becomes_available_for_reuse)
{
  int    value     = get_random_int();
  size_t first_idx = get_random_int(0, 1), second_idx = first_idx == 1 ? 0 : 1;

  this->vec.insert(first_idx, this->create_elem(value));
  this->vec.insert(second_idx, this->create_elem(value + 1));
  ASSERT_EQ(this->vec.size(), 2);

  ASSERT_TRUE(this->vec.erase(second_idx));
  ASSERT_EQ(this->vec.size(), 1);
  ASSERT_EQ(this->vec[first_idx], this->create_elem(value));
  ASSERT_EQ(this->vec.find_first_empty(), second_idx);
}

TYPED_TEST(slotted_array_tester, removed_first_index_becomes_available_for_reuse)
{
  int    value     = get_random_int();
  size_t first_idx = get_random_int(0, 1), second_idx = first_idx == 1 ? 0 : 1;

  this->vec.insert(first_idx, this->create_elem(value));
  this->vec.insert(second_idx, this->create_elem(value + 1));
  ASSERT_EQ(this->vec.size(), 2);

  ASSERT_TRUE(this->vec.erase(first_idx));
  ASSERT_EQ(this->vec.size(), 1);
  ASSERT_EQ(this->vec[second_idx], this->create_elem(value + 1));
  ASSERT_EQ(this->vec.find_first_empty(), first_idx);
}

/// Test confirms that the slotted_array uses nullptr instead of an extra boolean to represent an empty entry in the
/// slotted_array/slotted_vector.
TEST(detail_slotted_array_of_unique_ptrs, slotted_array_leverages_null_to_represent_empty_state)
{
  slotted_array<std::unique_ptr<int>, 5> ar;
  slotted_vector<std::unique_ptr<int>>   vec;

  ar.insert(0, std::make_unique<int>(4));
  ar.insert(1, std::make_unique<int>(4));
  ASSERT_EQ((long unsigned)((char*)&ar[1] - (char*)&ar[0]), sizeof(std::unique_ptr<int>));

  vec.insert(0, std::make_unique<int>(4));
  vec.insert(1, std::make_unique<int>(4));
  ASSERT_EQ((long unsigned)((char*)&vec[1] - (char*)&vec[0]), sizeof(std::unique_ptr<int>));
}

TEST(slotted_vector, move_ctor_empties_original_vector)
{
  slotted_vector<moveonly_test_object> vec;
  int                                  value = get_random_int();
  unsigned                             idx   = get_random_int(0, 19);
  vec.insert(idx, moveonly_test_object(value));

  auto vec2 = std::move(vec);
  ASSERT_TRUE(vec.empty());
  ASSERT_FALSE(vec2.empty());
  ASSERT_TRUE(vec2.contains(idx));
  ASSERT_EQ(vec2[idx], moveonly_test_object(value));
}

TEST(slotted_array, move_ctor_moves_the_value_of_elements)
{
  slotted_array<moveonly_test_object, 20> vec;
  int                                     value = get_random_int();
  unsigned                                idx   = get_random_int(0, 19);
  vec.insert(idx, moveonly_test_object(value));

  auto vec2 = std::move(vec);
  ASSERT_EQ(vec.size(), 1);
  ASSERT_EQ(vec2.size(), 1);
  ASSERT_TRUE(vec.contains(idx));
  ASSERT_TRUE(vec2.contains(idx));
  ASSERT_EQ(vec2[idx], moveonly_test_object(value));
  ASSERT_FALSE(vec[idx].has_value());
}
