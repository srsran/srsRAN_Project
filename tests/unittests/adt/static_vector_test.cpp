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

#include "srsran/adt/static_vector.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsran;

static_assert(std::is_same<static_vector<int, 5>::value_type, int>::value, "Invalid traits");
static_assert(std::is_trivially_destructible<static_vector<int, 5>>::value, "Invalid traits");
static_assert(std::is_default_constructible<static_vector<int, 5>>::value, "Invalid traits");
static_assert(not std::is_trivially_destructible<static_vector<moveonly_test_object, 5>>::value, "Invalid traits");
static_assert(std::is_default_constructible<static_vector<moveonly_test_object, 5>>::value, "Invalid traits");
static_assert(std::is_default_constructible<static_vector<nondefault_ctor_test_object, 5>>::value, "Invalid traits");

std::vector<int> create_test_vector(size_t sz)
{
  std::vector<int> v(sz);
  for (unsigned i = 0; i != v.size(); ++i) {
    v[i] = test_rgen::uniform_int<int>();
  }
  return v;
}

template <typename T>
class static_vector_tester : public ::testing::Test
{
protected:
  using value_type = T;
};
using test_value_types = ::testing::Types<int, moveonly_test_object>;
TYPED_TEST_SUITE(static_vector_tester, test_value_types);

TYPED_TEST(static_vector_tester, default_ctor_creates_empty_vector)
{
  static_vector<typename TestFixture::value_type, 10> vec;
  ASSERT_TRUE(vec.empty());
  ASSERT_FALSE(vec.full());
  ASSERT_EQ(vec.size(), 0);
  ASSERT_EQ(vec.capacity(), 10);
  ASSERT_EQ(vec.begin(), vec.end());
}

TYPED_TEST(static_vector_tester, size_ctor_sets_size)
{
  size_t                                              sz = test_rgen::uniform_int(1, 10);
  static_vector<typename TestFixture::value_type, 10> vec(sz);
  ASSERT_FALSE(vec.empty());
  ASSERT_EQ(vec.full(), vec.size() == vec.capacity());
  ASSERT_EQ(vec.size(), sz);
  ASSERT_NE(vec.begin(), vec.end());
  ASSERT_EQ(vec.end() - vec.begin(), vec.size());
}

TEST(static_vector_test, size_value_ctor_sets_size_and_value)
{
  size_t                 sz = test_rgen::uniform_int(1, 10);
  int                    val{test_rgen::uniform_int<int>()};
  static_vector<int, 10> vec(sz, val);
  ASSERT_FALSE(vec.empty());
  ASSERT_EQ(vec.full(), vec.size() == vec.capacity());
  ASSERT_EQ(vec.size(), sz);
  ASSERT_TRUE(std::all_of(vec.begin(), vec.end(), [val](int v) { return v == val; }));
}

TEST(static_vector_test, initializer_list_ctor_sets_vector_values)
{
  static_vector<int, 10> vec = {0, 1, 2, 3, 4, 5};
  ASSERT_FALSE(vec.empty());
  ASSERT_EQ(vec.size(), 6);
  ASSERT_EQ(vec.end() - vec.begin(), vec.size());
  std::array<int, 6> expected = {0, 1, 2, 3, 4, 5};
  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), expected.begin(), expected.end()));
}

TEST(static_vector_test, copy_ctor_sets_vector_values)
{
  static_vector<int, 10> vec = {0, 1, 2, 3, 4, 5};
  static_vector<int, 10> vec2(vec);
  ASSERT_EQ(vec2.size(), 6);
  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), vec2.begin(), vec2.end()));
}

TEST(static_vector_test, move_ctor_sets_vector_values)
{
  static_vector<moveonly_test_object, 10> vec;
  vec.emplace_back(0);
  vec.emplace_back(1);
  static_vector<moveonly_test_object, 10> vec2(std::move(vec));
  ASSERT_EQ(2, vec2.size());
  ASSERT_EQ(*vec2.begin(), 0);
  ASSERT_EQ(*(vec2.begin() + 1), 1);
}

TEST(static_vector_test, copy_assignment_sets_vector_values)
{
  std::vector<int>       expected = create_test_vector(test_rgen::uniform_int(0, 10));
  static_vector<int, 10> vec(expected.begin(), expected.end());
  static_vector<int, 10> vec2;
  vec2 = vec;
  ASSERT_TRUE(std::equal(vec2.begin(), vec2.end(), expected.begin(), expected.end()));
  ASSERT_EQ(vec2, vec);
}

TYPED_TEST(static_vector_tester, move_assignment_sets_vector_values)
{
  using T                       = typename TestFixture::value_type;
  std::vector<int>     expected = create_test_vector(test_rgen::uniform_int(0, 10));
  static_vector<T, 10> vec(expected.begin(), expected.end());
  static_vector<T, 10> vec2;
  ASSERT_TRUE(vec2.empty());
  vec2 = std::move(vec);
  ASSERT_TRUE(std::equal(vec2.begin(), vec2.end(), expected.begin(), expected.end()));
}

TYPED_TEST(static_vector_tester, emplace_back_creates_new_element_at_the_back)
{
  using T = typename TestFixture::value_type;
  static_vector<T, 10> vec;

  vec.emplace_back(5);
  vec.emplace_back(15);
  ASSERT_EQ(vec.size(), 2);
  ASSERT_EQ(*vec.begin(), 5);
  ASSERT_EQ(*(vec.begin() + 1), 15);
}

TYPED_TEST(static_vector_tester, element_access_returns_correct_value)
{
  static_vector<moveonly_test_object, 10> vec;
  vec.emplace_back(1);
  vec.emplace_back(2);
  vec.emplace_back(3);
  ASSERT_EQ(vec[0], 1);
  ASSERT_EQ(vec[1], 2);
  ASSERT_EQ(vec[2], 3);
  ASSERT_EQ(vec.front(), 1);
  ASSERT_EQ(vec.back(), 3);
  ASSERT_EQ(*vec.begin(), 1);
  const auto& c_vec = vec;
  ASSERT_EQ(c_vec[0], 1);
  ASSERT_EQ(c_vec.front(), 1);
  ASSERT_EQ(c_vec.back(), 3);
  ASSERT_EQ(*c_vec.begin(), 1);
}

TYPED_TEST(static_vector_tester, push_back_creates_new_element_at_the_back)
{
  using T                       = typename TestFixture::value_type;
  std::vector<int>     expected = create_test_vector(test_rgen::uniform_int<size_t>(0, 10));
  static_vector<T, 10> vec;

  for (unsigned i = 0; i != expected.size(); ++i) {
    vec.push_back(T(expected[i]));
  }

  ASSERT_EQ(vec.size(), expected.size());
  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), expected.begin(), expected.end()));
}

TYPED_TEST(static_vector_tester, clear_removes_all_elements)
{
  using T                       = typename TestFixture::value_type;
  std::vector<int>     expected = create_test_vector(test_rgen::uniform_int(0, 10));
  static_vector<T, 10> vec(expected.begin(), expected.end());

  vec.clear();
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(vec.size(), 0);
  ASSERT_EQ(moveonly_test_object::object_count(), 0);
}

TYPED_TEST(static_vector_tester, resize_with_growth_does_not_affect_existing_elements)
{
  using T                        = typename TestFixture::value_type;
  size_t               first_sz  = test_rgen::uniform_int<size_t>(0, 10);
  size_t               second_sz = test_rgen::uniform_int<size_t>(first_sz, 10);
  std::vector<int>     expected  = create_test_vector(first_sz);
  static_vector<T, 10> vec(expected.begin(), expected.end());

  vec.resize(second_sz);
  ASSERT_EQ(vec.size(), second_sz);
  ASSERT_TRUE(std::equal(vec.begin(), vec.begin() + first_sz, expected.begin(), expected.end()));
}

TYPED_TEST(static_vector_tester, resize_with_shrink_correctly_removes_extra_elements)
{
  using T                        = typename TestFixture::value_type;
  size_t               first_sz  = test_rgen::uniform_int<size_t>(0, 10);
  size_t               second_sz = test_rgen::uniform_int<size_t>(0, first_sz);
  std::vector<int>     expected  = create_test_vector(first_sz);
  static_vector<T, 10> vec(expected.begin(), expected.end());

  vec.resize(second_sz);
  ASSERT_EQ(vec.size(), second_sz);
  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), expected.begin(), expected.begin() + second_sz));
}

TYPED_TEST(static_vector_tester, assign_removes_previous_elements)
{
  using T                        = typename TestFixture::value_type;
  std::vector<int>     expected  = create_test_vector(test_rgen::uniform_int(0, 10));
  std::vector<int>     expected2 = create_test_vector(test_rgen::uniform_int(0, 10));
  static_vector<T, 10> vec(expected.begin(), expected.end());

  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), expected.begin(), expected.end()));
  vec.assign(expected2.begin(), expected2.end());
  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), expected2.begin(), expected2.end()));
}

TYPED_TEST(static_vector_tester, pop_back_removes_last_elements)
{
  using T                        = typename TestFixture::value_type;
  size_t               first_sz  = test_rgen::uniform_int<size_t>(0, 10);
  size_t               second_sz = test_rgen::uniform_int<size_t>(0, first_sz);
  std::vector<int>     expected  = create_test_vector(first_sz);
  static_vector<T, 10> vec(expected.begin(), expected.end());

  for (unsigned i = second_sz; i != first_sz; ++i) {
    vec.pop_back();
  }
  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), expected.begin(), expected.begin() + second_sz));
}

TYPED_TEST(static_vector_tester, erase_shifts_last_elements)
{
  using T                       = typename TestFixture::value_type;
  std::vector<int>     expected = create_test_vector(test_rgen::uniform_int<size_t>(1, 10));
  static_vector<T, 10> vec(expected.begin(), expected.end());
  size_t               pos_to_erase = test_rgen::uniform_int<size_t>(0, expected.size() - 1);

  vec.erase(vec.begin() + pos_to_erase);
  expected.erase(expected.begin() + pos_to_erase);
  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), expected.begin(), expected.end()));
}

TYPED_TEST(static_vector_tester, swap_keeps_values)
{
  using T                        = typename TestFixture::value_type;
  std::vector<int>     expected  = create_test_vector(test_rgen::uniform_int<size_t>(1, 10));
  std::vector<int>     expected2 = create_test_vector(test_rgen::uniform_int<size_t>(1, 10));
  static_vector<T, 10> vec(expected.begin(), expected.end());
  static_vector<T, 10> vec2(expected2.begin(), expected2.end());

  vec.swap(vec2);
  ASSERT_FALSE(std::equal(vec.begin(), vec.end(), expected.begin(), expected.end()));
  ASSERT_TRUE(std::equal(vec2.begin(), vec2.end(), expected.begin(), expected.end()));
  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), expected2.begin(), expected2.end()));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  int ret = RUN_ALL_TESTS();
  srsran_assert(moveonly_test_object::object_count() == 0,
                "number of ctor calls matches number of dtor calls for test object");

  return ret;
}
