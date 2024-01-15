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

#include "srsran/adt/circular_map.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

namespace srsran {

class full_circular_map : public ::testing::Test
{
protected:
  void SetUp() override
  {
    mymap.insert(0, "0");
    mymap.insert(1, "1");
    mymap.insert(2, "2");
    mymap.insert(3, "3");
  };

  bool remove_first_item()
  {
    // This code is:
    // - An action in one test case
    // - A setup in another one
    return mymap.erase(0);
  }

  void assert_map_is_full()
  {
    // Hide here related asserts to reuse them between test cases
    EXPECT_TRUE(mymap.full());
    EXPECT_EQ(mymap.size(), 4);
    EXPECT_FALSE(mymap.empty());
  }

  circular_map<uint32_t, std::string, 4> mymap;
};

// Good cases
TEST_F(full_circular_map, can_validate_fullness)
{
  // There is one test case (and only one) that validates the setup
  assert_map_is_full();
};

TEST_F(full_circular_map, can_remove_an_item)
{
  auto result = remove_first_item();

  EXPECT_TRUE(result);
  EXPECT_FALSE(mymap.full());
}

TEST_F(full_circular_map, can_insert_after_matching_index_removed)
{
  // This is our test case setup and we don't validate it
  // It's already validated before
  remove_first_item();

  auto result = mymap.insert(4, "4");

  EXPECT_TRUE(result);
  assert_map_is_full();
}

// Bad Cases
TEST_F(full_circular_map, fails_to_insert_when_full)
{
  auto result = mymap.insert(4, "4");

  EXPECT_FALSE(result);
  assert_map_is_full();
};

TEST_F(full_circular_map, fails_to_remove_out_of_index)
{
  auto result = mymap.erase(4);

  EXPECT_FALSE(result);
  assert_map_is_full();
};

} // namespace srsran