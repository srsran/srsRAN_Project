/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
