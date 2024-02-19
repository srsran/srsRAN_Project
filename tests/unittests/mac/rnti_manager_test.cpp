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

#include "lib/mac/rnti_manager.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(rnti_manager_test, when_allocate_rnti_called_multiple_times_then_rntis_are_unique)
{
  rnti_manager rnti_db;

  unsigned         max_count = 100;
  std::set<rnti_t> prev_rntis;
  for (unsigned count = 0; count != max_count; ++count) {
    rnti_t rnti = rnti_db.allocate();
    ASSERT_EQ(prev_rntis.count(rnti), 0);
    prev_rntis.insert(rnti);
  }

  ASSERT_EQ(rnti_db.nof_ues(), 0) << "No UE should have been added";
}

TEST(rnti_manager_test, when_ue_added_then_allocate_rnti_does_not_repeat_rnti)
{
  rnti_manager rnti_db;

  rnti_t rnti1 = rnti_db.allocate();
  ASSERT_TRUE(rnti_db.add_ue(rnti1, to_du_ue_index(0)));

  rnti_t rnti2 = rnti_db.allocate();
  ASSERT_NE(rnti1, rnti2);

  ASSERT_EQ(rnti_db.nof_ues(), 1);
}