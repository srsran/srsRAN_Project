/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

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
  using T = typename TestFixture::value_type;
  static_vector<T, 10> vec;

  vec.push_back(T(5));
  vec.push_back(T(15));
  ASSERT_EQ(vec.size(), 2);
  ASSERT_EQ(vec[0], 5);
  ASSERT_EQ(vec[1], 15);
}

TYPED_TEST(static_vector_tester, resize_with_growth_does_not_affect_existing_elements)
{
  using T                        = typename TestFixture::value_type;
  size_t               first_sz  = test_rgen::uniform_int<size_t>(0, 10);
  size_t               second_sz = test_rgen::uniform_int<size_t>(first_sz, 10);
  std::vector<int>     expected;
  static_vector<T, 10> vec;

  for (unsigned i = 0; i != first_sz; ++i) {
    expected.push_back(test_rgen::uniform_int<int>());
    vec.emplace_back(expected.back());
  }

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
  static_vector<T, 10> vec;

  for (unsigned i = 0; i != first_sz; ++i) {
    vec.emplace_back(expected[i]);
  }

  vec.resize(second_sz);
  ASSERT_EQ(vec.size(), second_sz);
  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), expected.begin(), expected.begin() + second_sz));
}

// void test_obj_add_rem()
//{
//   // TEST: push_back / emplace_back
//   static_vector<C, 10> a;
//   TESTASSERT(a.size() == 0);
//   TESTASSERT(a.empty());
//   a.push_back(1);
//   a.emplace_back(2);
//   TESTASSERT(a.size() == 2);
//   TESTASSERT(not a.empty());
//
//   // TEST: resize with size growth
//   a.resize(10, 3);
//   TESTASSERT(a.size() == 10);
//   TESTASSERT(a[0] == 1);
//   TESTASSERT(a[1] == 2);
//   TESTASSERT(a[2] == 3 and a.back() == 3);
//
//   // TEST: copy ctor correct insertion
//   static_vector<C, 10> a2(a);
//   TESTASSERT(a2.size() == a.size());
//   TESTASSERT(std::equal(a.begin(), a.end(), a2.begin()));
//
//   // TEST: back() access
//   a.back() = 4;
//   TESTASSERT(not std::equal(a.begin(), a.end(), a2.begin()));
//   a2 = a;
//   TESTASSERT(a == a2);
//
//   // TEST: assign
//   a.resize(5);
//   a2.assign(a.begin(), a.end());
//   TESTASSERT(a2.size() == 5);
//   TESTASSERT(a == a2);
//
//   // TEST: pop_back
//   unsigned last_nof_dtor = C::nof_dtor;
//   a.pop_back();
//   TESTASSERT(a.size() == 4 and last_nof_dtor == C::nof_dtor - 1);
//   TESTASSERT(a != a2);
//
//   // TEST: erase
//   a.erase(a.begin() + 1);
//   srsgnb::static_vector<C, 10> test = {1, 3, 3};
//   TESTASSERT(a == test);
//
//   // TEST: clear
//   last_nof_dtor = C::nof_dtor;
//   a.clear();
//   TESTASSERT(a.size() == 0 and a.empty() and last_nof_dtor == C::nof_dtor - 3);
//
//   // TEST: move assignment
//   TESTASSERT(a2.size() == 5);
//   a = std::move(a2);
//   TESTASSERT(a.size() == 5 and a2.empty());
//   test = {1, 2, 3, 3, 3};
//   TESTASSERT(a == test);
//
//   // TEST: move assignment from empty array
//   a2.clear();
//   a = std::move(a2);
//   TESTASSERT(a.empty() and a2.empty());
// }
//
// void test_move_only_type()
//{
//   static_vector<moveonly, 10> a(5);
//   TESTASSERT(a.size() == 5);
//
//   static_vector<moveonly, 10> a2(std::move(a));
//   TESTASSERT(a2.size() == 5 and a.empty());
//
//   a2[0] = moveonly();
//   moveonly c;
//   a2[1] = std::move(c);
//
//   a2.emplace_back();
//   TESTASSERT(a2.size() == 6);
//
//   a2.push_back(moveonly());
//   TESTASSERT(a2.size() == 7);
// }
//
// void test_swap()
//{
//   static_vector<int, 5> vec1, vec2;
//   vec1 = {1, 2, 3, 4, 5};
//   vec1.swap(vec2);
//   TESTASSERT(vec1.empty());
//   TESTASSERT_EQ(5, vec2.size());
//
//   vec1.push_back(6);
//   vec1.swap(vec2);
//   TESTASSERT_EQ(5, vec1.size());
//   TESTASSERT_EQ(1, vec2.size());
//   TESTASSERT_EQ(6, vec2[0]);
//   TESTASSERT_EQ(5, vec1[4]);
// }

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  int ret = RUN_ALL_TESTS();
  srsgnb_assert(moveonly_test_object::object_count() == 0,
                "number of ctor calls matches number of dtor calls for test object");

  return ret;
}
