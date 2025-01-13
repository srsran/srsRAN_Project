/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/ranges/transform.h"
#include "srsran/adt/span.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

// static tests

struct TestIdentityFunctor {
  unsigned operator()(unsigned v) const { return v; }
};

using vec_idx_tf_type = decltype(views::transform(std::vector<unsigned>{}, TestIdentityFunctor{}));
static_assert(std::is_same_v<vec_idx_tf_type::value_type, unsigned>, "invalid value_type");
static_assert(std::is_same_v<vec_idx_tf_type::iterator::value_type, unsigned>, "invalid value_type");
static_assert(std::is_same_v<vec_idx_tf_type::const_iterator::value_type, unsigned>, "invalid value_type");
static_assert(std::is_same_v<vec_idx_tf_type::iterator::iterator_category, std::random_access_iterator_tag>,
              "invalid iterator category");
static_assert(
    std::is_same_v<std::iterator_traits<vec_idx_tf_type::iterator>::iterator_category, std::random_access_iterator_tag>,
    "invalid iterator traits");

using span_idx_tf_type = decltype(views::transform(span<const unsigned>{}, TestIdentityFunctor{}));
static_assert(std::is_same_v<span_idx_tf_type::value_type, unsigned>, "invalid value_type");
static_assert(std::is_same_v<span_idx_tf_type::iterator::value_type, unsigned>, "invalid value_type");
static_assert(std::is_same_v<span_idx_tf_type::iterator::iterator_category, std::random_access_iterator_tag>,
              "invalid iterator category");

struct TestChangeTypeFunctor {
  std::string operator()(unsigned v) const { return "hello"; }
};

using vec_change_tf_type = decltype(views::transform(std::vector<unsigned>{}, TestChangeTypeFunctor{}));
static_assert(std::is_same_v<vec_change_tf_type::value_type, std::string>, "invalid value_type");
static_assert(std::is_same_v<vec_change_tf_type::iterator::value_type, std::string>, "invalid value_type");
static_assert(std::is_same_v<vec_change_tf_type::iterator::iterator_category, std::random_access_iterator_tag>,
              "invalid iterator category");

// unit tests

TEST(transform_view_test, mutable_range_same_return_type_transform)
{
  std::vector<unsigned> vec        = {1, 2, 3, 4, 5};
  auto                  double_vec = views::transform(vec, [](unsigned i) { return i * 2; });

  unsigned i = 0;
  for (const auto& v : double_vec) {
    ASSERT_EQ(vec[i++] * 2, v);
  }
  ASSERT_EQ(i, vec.size());
}

TEST(transform_view_test, const_range_same_return_type_transform)
{
  const std::vector<unsigned> vec        = {1, 2, 3, 4, 5};
  auto                        double_vec = views::transform(vec, [](unsigned i) { return i * 2; });

  static_assert(std::is_same_v<decltype(double_vec)::value_type, unsigned>, "Invalid value_type");

  unsigned i = 0;
  for (const auto& v : double_vec) {
    ASSERT_EQ(vec[i++] * 2, v);
  }
  ASSERT_EQ(i, vec.size());
}

TEST(transform_view_test, span_same_return_type_transform)
{
  std::vector<unsigned> vec = {1, 2, 3, 4, 5};
  span<const unsigned>  vec_view{vec.data(), vec.size()};
  auto                  transform_func = [](unsigned i) { return i * 2; };
  auto                  double_vec     = views::transform(vec_view, transform_func);

  unsigned i = 0;
  for (const auto& v : double_vec) {
    ASSERT_EQ(vec[i++] * 2, v);
  }
  ASSERT_EQ(i, vec.size());
}

TEST(transform_view_test, vector_different_return_transform)
{
  std::vector<unsigned> vec        = {1, 2, 3, 4, 5};
  auto                  double_vec = views::transform(vec, [](unsigned i) { return fmt::format("hello {}", i); });

  unsigned i = 0;
  for (const auto& v : double_vec) {
    ASSERT_EQ(fmt::format("hello {}", vec[i++]), v);
  }
  ASSERT_EQ(i, vec.size());
}
