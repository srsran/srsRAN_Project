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

#include "srsran/adt/ring_buffer.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsran;

static_assert(std::is_same<static_ring_buffer<int, 10>::value_type, int>::value, "Invalid value_type");
static_assert(std::is_same<ring_buffer<int>::value_type, int>::value, "Invalid value_type");

template <typename CircBuffer>
class ring_buffer_test : public ::testing::Test
{
protected:
  using ring_type = CircBuffer;
  using elem_type = typename CircBuffer::value_type;

  ~ring_buffer_test()
  {
    report_fatal_error_if_not(moveonly_test_object::object_count() == 0, "Failed to correctly destroy objects");
  }

  template <
      typename U = CircBuffer,
      std::enable_if_t<
          std::is_same<U, ring_buffer<typename CircBuffer::value_type, CircBuffer::max_size_is_power_of_2>>::value,
          int> = 0>
  ring_type create_empty_ring()
  {
    ring_type ring{test_rgen::uniform_int<unsigned>(2, 1000)};
    // randomize read position of the ring.
    unsigned nof_objs = test_rgen::uniform_int<unsigned>(0, ring.max_size());
    for (unsigned i = 0; i != nof_objs; ++i) {
      ring.push(create_value());
      ring.pop();
    }
    return ring;
  }
  template <
      typename U = CircBuffer,
      std::enable_if_t<
          not std::is_same<U, ring_buffer<typename CircBuffer::value_type, CircBuffer::max_size_is_power_of_2>>::value,
          int> = 0>
  ring_type create_empty_ring()
  {
    ring_type ring;
    // randomize read position of the ring.
    unsigned nof_objs = test_rgen::uniform_int<unsigned>(0, ring.max_size());
    for (unsigned i = 0; i != nof_objs; ++i) {
      ring.push(create_value());
      ring.pop();
    }
    return ring;
  }

  ring_type create_semi_filled_ring()
  {
    ring_type ring     = create_empty_ring();
    unsigned  nof_objs = test_rgen::uniform_int<unsigned>(1, ring.max_size() - 1);
    for (unsigned i = 0; i != nof_objs; ++i) {
      ring.push(create_value());
    }
    return ring;
  }

  elem_type create_value(int v = test_rgen::uniform_int<int>()) { return elem_type{v}; }
};
using test_value_types = ::testing::Types<static_ring_buffer<int, 10>,
                                          static_ring_buffer<int, 200>,
                                          static_ring_buffer<moveonly_test_object, 10>,
                                          static_ring_buffer<moveonly_test_object, 200>,
                                          ring_buffer<int, false>,
                                          ring_buffer<moveonly_test_object, false>,
                                          ring_buffer<int, true>,
                                          ring_buffer<moveonly_test_object, true>>;
TYPED_TEST_SUITE(ring_buffer_test, test_value_types);

template <typename CircBuffer>
class copyable_ring_tester : public ring_buffer_test<CircBuffer>
{
};
using copyable_test_value_types =
    ::testing::Types<static_ring_buffer<int, 10>, static_ring_buffer<int, 200>, ring_buffer<int>>;
TYPED_TEST_SUITE(copyable_ring_tester, copyable_test_value_types);

TYPED_TEST(ring_buffer_test, empty_ring_is_in_valid_state)
{
  auto ring = this->create_empty_ring();
  ASSERT_TRUE(ring.empty());
  ASSERT_FALSE(ring.full());
  ASSERT_EQ(ring.size(), 0);
  ASSERT_GT(ring.max_size(), 0);
  ASSERT_EQ(ring.begin(), ring.end());
  ASSERT_EQ(ring.end() - ring.begin(), 0);
  ASSERT_EQ(std::distance(ring.begin(), ring.end()), 0);
}

TYPED_TEST(ring_buffer_test, clear_called_on_empty_ring_is_no_op)
{
  auto ring = this->create_empty_ring();
  ring.clear();
  ASSERT_EQ(ring.size(), 0);
  ASSERT_GT(ring.max_size(), 0);
}

TYPED_TEST(ring_buffer_test, full_ring_is_in_valid_state)
{
  auto ring = this->create_empty_ring();
  for (unsigned i = 0; i != ring.max_size(); ++i) {
    ring.push(this->create_value());
  }
  ASSERT_TRUE(ring.full());
  ASSERT_EQ(ring.size(), ring.max_size());
  ASSERT_NE(ring.begin(), ring.end());
  ASSERT_EQ(ring.end() - ring.begin(), ring.size());
  ASSERT_EQ(std::distance(ring.begin(), ring.end()), ring.size());
  ASSERT_EQ(*ring.begin(), ring.top());
}

TYPED_TEST(ring_buffer_test, when_push_is_called_then_ring_size_is_updated)
{
  auto ring = this->create_semi_filled_ring();
  ASSERT_FALSE(ring.empty());
  ASSERT_EQ(ring.full(), ring.size() == ring.max_size());
  ASSERT_GT(ring.size(), 0);
  ASSERT_NE(ring.begin(), ring.end());
  ASSERT_EQ(ring.end() - ring.begin(), ring.size());
  ASSERT_EQ(std::distance(ring.begin(), ring.end()), ring.size());
}

TYPED_TEST(ring_buffer_test, when_clear_is_called_then_ring_becomes_empty)
{
  auto ring = this->create_semi_filled_ring();
  ASSERT_FALSE(ring.empty());
  ring.clear();
  ASSERT_TRUE(ring.empty());
  ASSERT_EQ(ring.begin(), ring.end());
  ASSERT_EQ(ring.end() - ring.begin(), 0);
  ASSERT_EQ(std::distance(ring.begin(), ring.end()), 0);
}

TYPED_TEST(ring_buffer_test, pop_and_top_in_reverse_order_of_push)
{
  auto     ring     = this->create_empty_ring();
  unsigned nof_objs = test_rgen::uniform_int<unsigned>(1, ring.max_size());
  for (unsigned i = 0; i != nof_objs; ++i) {
    ring.push(this->create_value(i));
  }

  for (unsigned i = 0; i != nof_objs; ++i) {
    ASSERT_EQ(ring.size(), nof_objs - i);
    ASSERT_EQ(ring.top(), i);
    ring.pop();
  }
  ASSERT_TRUE(ring.empty());
  ASSERT_EQ(ring.size(), 0);
}

TYPED_TEST(ring_buffer_test, iterator_remains_valid_after_pushes)
{
  auto     ring     = this->create_empty_ring();
  unsigned nof_objs = test_rgen::uniform_int<unsigned>(1, ring.max_size());
  for (unsigned i = 0; i != nof_objs; ++i) {
    ring.push(this->create_value(i));
  }

  // iterator.
  unsigned count = 0;
  for (auto it = ring.begin(); it != ring.end(); ++it) {
    ASSERT_EQ(*it, count++);
  }

  // const_iterator.
  const auto& cring = ring;
  count             = 0;
  for (auto cit = cring.begin(); cit != cring.end(); ++cit) {
    ASSERT_EQ(*cit, count++);
  }
}

TYPED_TEST(ring_buffer_test, when_move_ctor_is_called_then_elements_are_moved)
{
  auto     ring1    = this->create_empty_ring();
  unsigned nof_objs = test_rgen::uniform_int<unsigned>(1, ring1.max_size());
  for (unsigned i = 0; i != nof_objs; ++i) {
    ring1.push(this->create_value(i));
  }
  auto ring2(std::move(ring1));
  ASSERT_EQ(nof_objs, ring2.size());
  unsigned count = 0;
  for (const auto& v : ring2) {
    ASSERT_EQ(v, count++);
  }
  ASSERT_TRUE(ring1.empty());
}

TYPED_TEST(copyable_ring_tester, when_copy_ctor_is_called_then_elements_are_copied)
{
  auto ring1 = this->create_semi_filled_ring();
  auto ring2(ring1);
  ASSERT_EQ(ring2.size(), ring2.size());
  ASSERT_EQ(ring1, ring2);
}

TYPED_TEST(ring_buffer_test, when_move_assignment_is_called_then_lhs_elements_are_destroyed_and_rhs_elements_are_moved)
{
  auto     ring1     = this->create_semi_filled_ring();
  auto     ring2     = this->create_empty_ring();
  unsigned nof_objs2 = test_rgen::uniform_int<unsigned>(0, ring2.max_size());
  for (unsigned i = 0; i != nof_objs2; ++i) {
    ring2.push(this->create_value(i));
  }
  ring1 = std::move(ring2);
  ASSERT_EQ(ring1.size(), nof_objs2);
  unsigned count = 0;
  for (const auto& v : ring1) {
    ASSERT_EQ(v, count++);
  }
  ASSERT_TRUE(ring2.empty());
}

TYPED_TEST(copyable_ring_tester,
           when_copy_assignment_is_called_then_lhs_elements_are_destroyed_and_rhs_elements_are_copied)
{
  auto ring1 = this->create_semi_filled_ring();
  auto ring2 = this->create_semi_filled_ring();
  ring1      = ring2;
  ASSERT_EQ(ring1, ring2);
}

TYPED_TEST(copyable_ring_tester, operator_equal_for_different_scenarios)
{
  auto ring1 = this->create_semi_filled_ring();

  auto ring_eq(ring1);
  ASSERT_EQ(ring_eq, ring1);

  auto ring_diff_val(ring1);
  ring_diff_val[test_rgen::uniform_int<unsigned>(0, ring_diff_val.size() - 1)]++;
  ASSERT_NE(ring_diff_val, ring1);

  auto ring_shorter(ring1);
  ring_shorter.pop();
  ASSERT_NE(ring_shorter, ring1);

  if (not ring1.full()) {
    auto ring_longer(ring1);
    ring_longer.push(this->create_value());
    ASSERT_NE(ring_longer, ring1);
  }
}

TEST(dyn_ring_buffer_test, when_force_power2_flag_is_set_then_buffer_size_is_automatically_set_to_closest_power_of_2)
{
  unsigned               buffer_size = test_rgen::uniform_int<unsigned>(1, 1024);
  ring_buffer<int, true> buf(buffer_size);
  ASSERT_EQ(buf.size(), 0);
  ASSERT_GE(buf.max_size(), buffer_size);
  ASSERT_EQ(buf.max_size() & (buf.max_size() - 1), 0);
}

TEST(dyn_ring_buffer_test,
     when_dtor_of_buffer_of_moveonly_objects_is_called_then_buffer_elements_destructors_are_called)
{
  unsigned buffer_size = test_rgen::uniform_int<unsigned>(1, 20);
  unsigned nof_objs    = test_rgen::uniform_int<unsigned>(1, buffer_size);
  using C              = moveonly_test_object;
  ASSERT_EQ(C::object_count(), 0);
  {
    ring_buffer<C> circ_buffer(buffer_size);
    for (unsigned i = 0; i != nof_objs; ++i) {
      circ_buffer.push(C{test_rgen::uniform_int<int>()});
    }
    ASSERT_EQ(C::object_count(), nof_objs);
  }
  ASSERT_EQ(C::object_count(), 0);
}

TYPED_TEST(copyable_ring_tester, push_in_batches)
{
  auto                                             ring = this->create_empty_ring();
  std::vector<typename decltype(ring)::value_type> vec;
  unsigned                                         nof_objs = test_rgen::uniform_int<unsigned>(1, ring.max_size());
  for (unsigned i = 0; i != nof_objs; ++i) {
    vec.push_back(this->create_value());
  }

  ring.push(vec);
  ASSERT_EQ(ring.size(), vec.size());
  ASSERT_TRUE(std::equal(vec.begin(), vec.end(), ring.begin(), ring.end()));
}

TYPED_TEST(copyable_ring_tester, pop_in_batches)
{
  auto                                             ring = this->create_empty_ring();
  std::vector<typename decltype(ring)::value_type> vec, vec2;
  unsigned                                         nof_objs = test_rgen::uniform_int<unsigned>(1, ring.max_size());
  for (unsigned i = 0; i != nof_objs; ++i) {
    vec.push_back(this->create_value());
  }
  ring.push(vec);

  vec2.resize(vec.size());
  ASSERT_EQ(ring.pop_into(vec2), vec2.size());
  ASSERT_TRUE(ring.empty());
  ASSERT_EQ(vec2, vec);
}
