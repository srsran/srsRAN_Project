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

#include "srsran/ran/bcd_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(bcd_helpers_test, plmn_string_to_bcd)
{
  ASSERT_EQ(plmn_string_to_bcd("00101"), 0xf110);
  ASSERT_EQ(plmn_string_to_bcd("20899"), 0x2f899);
}

TEST(bcd_helpers_test, plmn_bcd_to_string)
{
  ASSERT_EQ(plmn_bcd_to_string(0xf110), "00101");
  ASSERT_EQ(plmn_bcd_to_string(0x2f899), "20899");
}
