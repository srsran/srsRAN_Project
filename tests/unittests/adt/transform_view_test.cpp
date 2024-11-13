/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/ranges/transform.h"
#include "srsran/adt/span.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(transform_view_test, mutable_range_same_return_type_transform)
{
  std::vector<unsigned> vec        = {1, 2, 3, 4, 5};
  auto                  double_vec = views::transform(vec, [](unsigned i) { return i * 2; });

  // static checks
  static_assert(std::is_same_v<decltype(double_vec)::value_type, unsigned>, "Invalid value_type");
  static_assert(std::is_same_v<decltype(double_vec.begin())::iterator_category, std::random_access_iterator_tag>,
                "Invalid iterator_category");
  static_assert(std::is_same_v<std::iterator_traits<decltype(vec.begin())>::iterator_category,
                               std::iterator_traits<decltype(double_vec.begin())>::iterator_category>,
                "Invalid iterator_category");

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

  static_assert(std::is_same_v<decltype(double_vec)::value_type, unsigned>, "Invalid value_type");
  static_assert(std::is_same_v<decltype(double_vec)::iterator::value_type, unsigned>, "Invalid value_type");
  static_assert(std::is_same_v<decltype(double_vec.begin())::iterator_category, std::random_access_iterator_tag>,
                "Invalid iterator_category");
  static_assert(std::is_same_v<std::iterator_traits<decltype(double_vec.begin())>::iterator_category,
                               std::random_access_iterator_tag>,
                "Invalid iterator_category");

  unsigned i = 0;
  for (const auto& v : double_vec) {
    ASSERT_EQ(vec[i++] * 2, v);
  }
  ASSERT_EQ(i, vec.size());
}
