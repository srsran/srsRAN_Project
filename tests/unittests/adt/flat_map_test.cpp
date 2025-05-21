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

#include "srsran/adt/flat_map.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

static_assert(std::is_same_v<flat_map<int, char>::key_type, int>, "Invalid key type");
static_assert(std::is_same_v<flat_map<int, char>::mapped_type, char>, "Invalid value type");

TEST(flat_map_test, empty_flat_map)
{
  flat_map<int, int> m;
  ASSERT_EQ(m.size(), 0);
  ASSERT_TRUE(m.empty());
  ASSERT_EQ(m.max_size(), 0);
  ASSERT_EQ(m.begin(), m.end());
  ASSERT_EQ(m.cbegin(), m.cend());
  ASSERT_EQ(m.find(0), m.end());
  ASSERT_EQ(m.lower_bound(0), m.end());
  m.clear();
  ASSERT_EQ(m.size(), 0);
}

TEST(flat_map_test, flat_map_creation_from_containers)
{
  std::vector<int>   keys   = {1, 2, 1, 1, 4};
  std::vector<int>   values = {1, 2, 1, 1, 4};
  flat_map<int, int> m{keys, values};

  ASSERT_FALSE(m.empty());
  ASSERT_EQ(m.size(), 3);
  ASSERT_EQ(m.begin()->first, 1);
  ASSERT_EQ((m.begin() + 1)->first, 2);
  ASSERT_EQ((m.begin() + 2)->first, 4);
  ASSERT_EQ(m.begin() + 3, m.end());
  for (const auto& p : m) {
    ASSERT_EQ(p.first, p.second);
  }
}

TEST(flat_map_test, flat_map_creation_from_containers_with_reverse_order)
{
  std::vector<int>                      keys   = {1, 2, 1, 1, 4};
  std::vector<int>                      values = {1, 2, 1, 1, 4};
  flat_map<int, int, std::greater<int>> m{keys, values};

  ASSERT_FALSE(m.empty());
  ASSERT_EQ(m.size(), 3);
  ASSERT_EQ(m.begin()->first, 4);
  ASSERT_EQ((m.begin() + 1)->first, 2);
  ASSERT_EQ((m.begin() + 2)->first, 1);
  ASSERT_EQ(m.begin() + 3, m.end());
  for (const auto& p : m) {
    ASSERT_EQ(p.first, p.second);
  }
}

TEST(flat_map_test, flat_map_creation_from_containers_of_move_only_types)
{
  std::vector<int>                  keys = {1, 2, 1, 1, 4};
  std::vector<moveonly_test_object> values;
  values.reserve(keys.size());
  for (unsigned i = 0; i != keys.size(); ++i) {
    values.emplace_back(keys[i]);
  }
  flat_map<int, moveonly_test_object> m{keys, std::move(values)};

  ASSERT_FALSE(m.empty());
  ASSERT_EQ(m.size(), 3);
  ASSERT_EQ(m.begin()->first, 1);
  ASSERT_EQ((m.begin() + 1)->first, 2);
  ASSERT_EQ((m.begin() + 2)->first, 4);
  ASSERT_EQ(m.begin() + 3, m.end());
  for (const auto& p : m) {
    ASSERT_EQ(p.first, p.second.value());
  }
}

TEST(flat_map_test, flat_map_creation_from_containers_of_move_only_types_in_reverse_order)
{
  std::vector<int>                  keys = {1, 2, 1, 1, 4};
  std::vector<moveonly_test_object> values;
  values.reserve(keys.size());
  for (unsigned i = 0; i != keys.size(); ++i) {
    values.emplace_back(keys[i]);
  }
  flat_map<int, moveonly_test_object, std::greater<int>> m{keys, std::move(values)};

  ASSERT_FALSE(m.empty());
  ASSERT_EQ(m.size(), 3);
  ASSERT_EQ(m.begin()->first, 4);
  ASSERT_EQ((m.begin() + 1)->first, 2);
  ASSERT_EQ((m.begin() + 2)->first, 1);
  ASSERT_EQ(m.begin() + 3, m.end());
  for (const auto& p : m) {
    ASSERT_EQ(p.first, p.second.value());
  }
}

TEST(flat_map_test, flat_map_copy)
{
  std::vector<int>   keys   = {1, 2, 1, 1, 4};
  std::vector<int>   values = {1, 2, 1, 1, 4};
  flat_map<int, int> m{keys, values};
  flat_map<int, int> copy = m;

  ASSERT_EQ(copy, m);
  ASSERT_EQ(copy.size(), 3);
}

TEST(flat_map_test, flat_map_move)
{
  std::vector<int>   keys   = {1, 2, 1, 1, 4};
  std::vector<int>   values = {1, 2, 1, 1, 4};
  flat_map<int, int> m{keys, values};
  flat_map<int, int> moved = std::move(m);

  ASSERT_EQ(moved.size(), 3);
  ASSERT_EQ(m.size(), 0);
}

TEST(flat_map_test, flat_map_clear)
{
  std::vector<int>   keys   = {1, 2, 1, 1, 4};
  std::vector<int>   values = {1, 2, 1, 1, 4};
  flat_map<int, int> m{keys, values};

  ASSERT_EQ(m.size(), 3);
  m.clear();
  ASSERT_EQ(m.size(), 0);
}

TEST(flat_map_test, flat_map_emplace)
{
  std::vector<int>   keys   = {1, 2, 1, 1, 4};
  std::vector<int>   values = {1, 2, 1, 1, 4};
  flat_map<int, int> m{keys, values};

  auto p = m.emplace(3, 3);
  ASSERT_TRUE(p.second);
  ASSERT_EQ(m.size(), 4);
  ASSERT_EQ(p.first->first, 3);
  ASSERT_EQ(p.first->second, 3);

  auto p2 = m.emplace(3, 3);
  ASSERT_FALSE(p2.second);
  ASSERT_EQ(m.size(), 4);

  std::vector<int> expected = {1, 2, 3, 4};
  ASSERT_TRUE(std::equal(expected.begin(), expected.end(), m.keys().begin(), m.keys().end()));
  ASSERT_TRUE(std::equal(expected.begin(), expected.end(), m.values().begin(), m.values().end()));
}

TEST(flat_map_test, flat_map_insert)
{
  std::vector<int>   keys   = {1, 2, 1, 1, 4};
  std::vector<int>   values = {1, 2, 1, 1, 4};
  flat_map<int, int> m{keys, values};

  auto p = m.insert(std::make_pair(3, 3));
  ASSERT_TRUE(p.second);
  ASSERT_EQ(m.size(), 4);
  ASSERT_EQ(p.first->first, 3);
  ASSERT_EQ(p.first->second, 3);

  auto p2 = m.insert(std::make_pair(3, 3));
  ASSERT_FALSE(p2.second);
  ASSERT_EQ(m.size(), 4);

  std::vector<int> expected = {1, 2, 3, 4};
  ASSERT_TRUE(std::equal(expected.begin(), expected.end(), m.keys().begin(), m.keys().end()));
  ASSERT_TRUE(std::equal(expected.begin(), expected.end(), m.values().begin(), m.values().end()));
}

TEST(flat_map_test, flat_map_insert_hint)
{
  std::vector<int>   keys   = {1, 2, 1, 1, 4};
  std::vector<int>   values = {1, 2, 1, 1, 4};
  flat_map<int, int> m{keys, values};

  flat_map<int, int>::iterator it = m.find(4);
  ASSERT_TRUE(it != m.end());
  ASSERT_EQ(it->second, 4);

  auto p = m.insert(it, std::make_pair(3, 3));
  ASSERT_EQ(m.size(), 4);
  ASSERT_EQ(p->first, 3);
  ASSERT_EQ(p->second, 3);

  it      = m.find(4);
  auto p2 = m.insert(it, std::make_pair(3, 3));
  ASSERT_EQ(m.size(), 4);
  ASSERT_EQ(p2->first, 3);
  ASSERT_EQ(p2->second, 3);

  std::vector<int> expected = {1, 2, 3, 4};
  ASSERT_TRUE(std::equal(expected.begin(), expected.end(), m.keys().begin(), m.keys().end()));
  ASSERT_TRUE(std::equal(expected.begin(), expected.end(), m.values().begin(), m.values().end()));
}

TEST(flat_map_test, erase_iterator)
{
  std::vector<int>   keys   = {1, 2, 1, 1, 4};
  std::vector<int>   values = {1, 2, 1, 1, 4};
  flat_map<int, int> m{keys, values};

  auto it = m.erase(m.find(2));
  ASSERT_EQ(m.size(), 2);
  ASSERT_EQ(it->first, 4);
  ASSERT_EQ(it->second, 4);
}

TEST(flat_map_test, erase_key)
{
  std::vector<int>   keys   = {1, 2, 1, 1, 4};
  std::vector<int>   values = {1, 2, 1, 1, 4};
  flat_map<int, int> m{keys, values};

  ASSERT_EQ(m.erase(2), 1);
  ASSERT_EQ(m.size(), 2);
  ASSERT_EQ(m.find(2), m.end());

  ASSERT_EQ(m.erase(2), 0);
  ASSERT_EQ(m.size(), 2);

  ASSERT_EQ(m.erase(1), 1);
  ASSERT_EQ(m.size(), 1);
  ASSERT_EQ(m.erase(4), 1);
  ASSERT_EQ(m.size(), 0);
  ASSERT_TRUE(m.keys().empty());
  ASSERT_TRUE(m.values().empty());
}
