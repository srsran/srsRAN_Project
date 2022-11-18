/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsgnb;

std::random_device rd;
std::mt19937       g(rd());

static unsigned get_random_int(int lb = 0, int ub = std::numeric_limits<int>::max())
{
  return std::uniform_int_distribution<int>{lb, ub}(g);
}

static_assert(std::is_same<slot_array<int, 5>::value_type, int>::value, "Invalid container value_type");
static_assert(std::is_same<slot_array<int, 5>::iterator::value_type, int>::value, "Invalid container value_type");
static_assert(std::is_same<slot_array<int, 5>::const_iterator::value_type, int>::value, "Invalid container value_type");

template <typename T>
class slot_array_tester : public ::testing::Test
{
protected:
  using SlotArrayType = T;
  using ValueType     = typename SlotArrayType::value_type;
};
using slot_array_types = ::testing::Types<slot_array<int, 20>,
                                          slot_array<moveonly_test_object, 20>,
                                          slot_vector<int>,
                                          slot_vector<moveonly_test_object>>;
TYPED_TEST_SUITE(slot_array_tester, slot_array_types);

TYPED_TEST(slot_array_tester, default_ctor_creates_empty_array)
{
  typename TestFixture::SlotArrayType vec;
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(vec.size(), 0);
  ASSERT_EQ(vec.begin(), vec.end());
  ASSERT_FALSE(vec.contains(get_random_int(0, 1000)));
}

TYPED_TEST(slot_array_tester, insert_creates_entry_in_slot_array)
{
  typename TestFixture::SlotArrayType vec;
  using ValueType = typename TestFixture::ValueType;
  int      value  = get_random_int();
  unsigned idx    = get_random_int(0, 19);

  vec.insert(idx, ValueType(value));
  ASSERT_EQ(vec.size(), 1);
  ASSERT_FALSE(vec.empty());
  ASSERT_TRUE(vec.contains(idx));
  ASSERT_EQ(vec[idx], ValueType(value));
  ASSERT_NE(vec.begin(), vec.end());
  ASSERT_EQ(*vec.begin(), ValueType(value));
}

TYPED_TEST(slot_array_tester, emplace_constructs_element_in_slot_array)
{
  typename TestFixture::SlotArrayType vec;
  using ValueType = typename TestFixture::ValueType;
  int      value  = get_random_int();
  unsigned idx    = get_random_int(0, 19);

  vec.emplace(idx, value);
  ASSERT_EQ(vec.size(), 1);
  ASSERT_FALSE(vec.empty());
  ASSERT_TRUE(vec.contains(idx));
  ASSERT_EQ(vec[idx], ValueType(value));
  ASSERT_NE(vec.begin(), vec.end());
  ASSERT_EQ(*vec.begin(), ValueType(value));
}

TYPED_TEST(slot_array_tester, insert_in_already_inserted_position_does_not_alter_slot_array_size)
{
  typename TestFixture::SlotArrayType vec;
  using ValueType = typename TestFixture::ValueType;
  int      value = get_random_int(), value2 = get_random_int();
  unsigned idx = get_random_int(0, 19);

  vec.insert(idx, ValueType(value));
  vec.insert(idx, ValueType(value2));
  ASSERT_EQ(vec.size(), 1);
  ASSERT_TRUE(vec.contains(idx));
  ASSERT_EQ(vec[idx], ValueType(value2));
  ASSERT_EQ(*vec.begin(), ValueType(value2));
}

TYPED_TEST(slot_array_tester, iterator_skips_empty_positions)
{
  typename TestFixture::SlotArrayType vec;
  using ValueType = typename TestFixture::ValueType;
  std::vector<int> values;
  for (unsigned i = get_random_int(0, 19); i < 20; i += get_random_int(1, 5)) {
    values.push_back(get_random_int());
    vec.insert(i, ValueType(values.back()));
  }

  ASSERT_EQ(vec.size(), values.size());
  size_t count = 0;
  for (const auto& e : vec) {
    ASSERT_EQ(e, ValueType(values[count++]));
  }
}

TYPED_TEST(slot_array_tester, erase_removes_element_from_slot_array_and_updates_size)
{
  typename TestFixture::SlotArrayType vec;
  using ValueType = typename TestFixture::ValueType;
  int      value  = get_random_int();
  unsigned idx    = get_random_int(0, 19);

  vec.insert(idx, ValueType(value));
  ASSERT_FALSE(vec.empty());
  vec.erase(idx);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(vec.size(), 0);
  ASSERT_EQ(vec.begin(), vec.end());
}

TYPED_TEST(slot_array_tester, move_ctor_moves_elements)
{
  typename TestFixture::SlotArrayType vec;
  using ValueType = typename TestFixture::ValueType;
  int      value  = get_random_int();
  unsigned idx    = get_random_int(0, 19);

  vec.insert(idx, ValueType(value));
  auto vec2 = std::move(vec);
  ASSERT_TRUE(vec.empty());
  ASSERT_FALSE(vec2.empty());
  ASSERT_TRUE(vec2.contains(idx));
  ASSERT_EQ(vec2[idx], ValueType(value));
}

TYPED_TEST(slot_array_tester, accessing_empty_position_asserts)
{
  typename TestFixture::SlotArrayType vec;
  using ValueType = typename TestFixture::ValueType;
  unsigned idx    = get_random_int(0, 19);
  unsigned idx2   = (idx + get_random_int(1, 18)) % 20;

  vec.insert(idx, ValueType(get_random_int()));
  ASSERT_DEATH(vec[idx2], ".*") << fmt::format("Accessing index={} for slot_array with element in index={}", idx2, idx);
}

TYPED_TEST(slot_array_tester, find_first_empty_skips_occupied_positions)
{
  typename TestFixture::SlotArrayType vec;
  using ValueType       = typename TestFixture::ValueType;
  unsigned nof_inserted = get_random_int(0, 19);

  for (unsigned i = 0; i != nof_inserted; ++i) {
    vec.insert(i, ValueType(get_random_int()));
  }
  ASSERT_EQ(vec.size(), nof_inserted);
  ASSERT_EQ(vec.find_first_empty(), nof_inserted);
}

/// Test confirms that the slot_array uses nullptr instead of an extra boolean to represent an empty entry in the
/// slot_array/slot_vector.
TEST(detail_slot_array_of_unique_ptrs, slot_array_leverages_null_to_represent_empty_state)
{
  slot_array<std::unique_ptr<int>, 5> ar;
  slot_vector<std::unique_ptr<int>>   vec;

  ar.insert(0, std::make_unique<int>(4));
  ar.insert(1, std::make_unique<int>(4));
  ASSERT_EQ((long unsigned)((char*)&ar[1] - (char*)&ar[0]), sizeof(std::unique_ptr<int>));

  vec.insert(0, std::make_unique<int>(4));
  vec.insert(1, std::make_unique<int>(4));
  ASSERT_EQ((long unsigned)((char*)&vec[1] - (char*)&vec[0]), sizeof(std::unique_ptr<int>));
}