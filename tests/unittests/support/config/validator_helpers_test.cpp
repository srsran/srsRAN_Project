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

#include "srsran/support/config/validator_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;

enum class id_type { MAX_ID_TYPE_VALUE = 1000 };

struct example_resource {
  id_type id_field;
};

std::vector<example_resource> make_resource_list(const std::initializer_list<unsigned>& list)
{
  std::vector<example_resource> ret;
  for (unsigned i : list) {
    ret.push_back({(id_type)i});
  }
  return ret;
}

TEST(validator_helpers_test, detection_of_unique_id)
{
  const std::vector<example_resource> vec1 = make_resource_list({1, 2, 6, 3, 4});
  const std::vector<example_resource> vec2 = make_resource_list({1, 2, 4, 3, 4});

  ASSERT_TRUE(has_unique_ids(vec1, [](example_resource r) { return r.id_field; }));
  ASSERT_TRUE(has_unique_ids(vec1, &example_resource::id_field));
  ASSERT_FALSE(has_unique_ids(vec2, [](example_resource r) { return r.id_field; }));
  ASSERT_FALSE(has_unique_ids(vec2, &example_resource::id_field));
}
