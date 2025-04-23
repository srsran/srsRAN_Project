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

#include "srsran/adt/circular_map.h"
#include <gtest/gtest.h>

using namespace srsran;

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

namespace {

/// The static variant of the circular_map should be trivially copyable for trivially copyable value types.
static_assert(std::is_trivially_copyable_v<static_circular_map<unsigned, int, 4>>);

template <typename U>
struct is_static_circular_map : std::false_type {};
template <typename K, typename V, std::size_t N>
struct is_static_circular_map<static_circular_map<K, V, N>> : std::true_type {};

template <typename T>
class circular_map_tester_1 : public ::testing::Test
{
protected:
  template <typename U = T, std::enable_if_t<is_static_circular_map<U>::value, int> = 0>
  constexpr circular_map_tester_1()
  {
  }

  template <typename U = T, std::enable_if_t<!is_static_circular_map<U>::value, int> = 0>
  constexpr circular_map_tester_1() : mymap(4)
  {
  }

  T mymap;
};

using test_value_types_1 = ::testing::Types<static_circular_map<unsigned, std::string, 4>,
                                            circular_map<unsigned, std::string, true>,
                                            circular_map<unsigned, std::string, false>>;

TYPED_TEST_SUITE(circular_map_tester_1, test_value_types_1);

TYPED_TEST(circular_map_tester_1, test_id_map)
{
  ASSERT_EQ(0, this->mymap.size());
  ASSERT_TRUE(this->mymap.empty() and not this->mymap.full());
  ASSERT_TRUE(this->mymap.begin() == this->mymap.end());

  ASSERT_TRUE(not this->mymap.contains(0));
  ASSERT_TRUE(this->mymap.insert(0, "obj0"));
  ASSERT_TRUE(this->mymap.contains(0) and this->mymap[0] == "obj0");
  ASSERT_EQ(1, this->mymap.size());
  ASSERT_TRUE(not this->mymap.empty() and not this->mymap.full());
  ASSERT_TRUE(this->mymap.begin() != this->mymap.end());

  ASSERT_TRUE(not this->mymap.insert(0, "obj0"));
  ASSERT_TRUE(this->mymap.insert(1, "obj1"));
  ASSERT_TRUE(this->mymap.contains(0) and this->mymap.contains(1) and this->mymap[1] == "obj1");
  ASSERT_TRUE(this->mymap.size() == 2 and not this->mymap.empty() and not this->mymap.full());

  ASSERT_TRUE(this->mymap.find(1) != this->mymap.end());
  ASSERT_EQ(1, this->mymap.find(1)->first);
  ASSERT_TRUE(this->mymap.find(1)->second == "obj1");

  // TEST: iteration.
  uint32_t count = 0;
  for (const auto& obj : this->mymap) {
    ASSERT_TRUE(obj.second == "obj" + std::to_string(count++));
  }

  // TEST: const iteration.
  count = 0;
  for (const auto& obj : this->mymap) {
    ASSERT_TRUE(obj.second == "obj" + std::to_string(count++));
  }

  ASSERT_TRUE(this->mymap.erase(0));
  ASSERT_TRUE(this->mymap.erase(1));
  ASSERT_TRUE(this->mymap.size() == 0 and this->mymap.empty());

  ASSERT_TRUE(this->mymap.insert(0, "obj0"));
  ASSERT_TRUE(this->mymap.insert(1, "obj1"));
  ASSERT_TRUE(this->mymap.size() == 2 and not this->mymap.empty() and not this->mymap.full());
  this->mymap.clear();
  ASSERT_TRUE(this->mymap.size() == 0 and this->mymap.empty());
}

struct C {
  C() { ++count; }
  ~C() { --count; }
  C(C&&) { ++count; }
  C(const C&)       = delete;
  C& operator=(C&&) = default;

  static size_t count;
};
size_t C::count = 0;

template <typename T>
class circular_map_tester_2 : public ::testing::Test
{
protected:
  template <typename U = T, std::enable_if_t<is_static_circular_map<U>::value, int> = 0>
  constexpr circular_map_tester_2()
  {
  }

  template <typename U = T, std::enable_if_t<!is_static_circular_map<U>::value, int> = 0>
  constexpr circular_map_tester_2() : mymap(4), mymap2(4), mymap3(4)
  {
  }

  T mymap;
  T mymap2;
  T mymap3;
};

using test_value_types_2 = ::testing::
    Types<static_circular_map<uint32_t, C, 4>, circular_map<uint32_t, C, true>, circular_map<uint32_t, C, false>>;

TYPED_TEST_SUITE(circular_map_tester_2, test_value_types_2);

TYPED_TEST(circular_map_tester_2, test_correct_destruction)
{
  ASSERT_TRUE(C::count == 0);
  ASSERT_TRUE(this->mymap.insert(0, C{}));
  ASSERT_TRUE(C::count == 1);
  ASSERT_TRUE(this->mymap.insert(1, C{}));
  ASSERT_TRUE(this->mymap.insert(2, C{}));
  ASSERT_TRUE(this->mymap.insert(3, C{}));
  ASSERT_TRUE(C::count == 4);
  ASSERT_TRUE(not this->mymap.insert(4, C{}));
  ASSERT_TRUE(C::count == 4);
  ASSERT_TRUE(this->mymap.erase(1));
  ASSERT_TRUE(C::count == 3);
  ASSERT_TRUE(not this->mymap.contains(1));

  std::array<uint32_t, 3> content{0, 2, 3};
  size_t                  i = 0;
  for (const auto& e : this->mymap) {
    ASSERT_TRUE(content[i] == e.first);
    ++i;
  }

  ASSERT_TRUE(C::count == 3);
  this->mymap2 = std::move(this->mymap);
  this->mymap.clear();
  ASSERT_TRUE(C::count == 3);

  ASSERT_TRUE(this->mymap3.insert(1, C{}));
  ASSERT_TRUE(C::count == 4);
  this->mymap2 = std::move(this->mymap3);
  this->mymap3.clear();
  ASSERT_TRUE(C::count == 1);

  this->mymap2.clear();
  ASSERT_TRUE(C::count == 0);
}

} // namespace
