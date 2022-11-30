/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../lib/ran/bcd_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;

TEST(bcd_helpers_test, plmn_string_to_bcd)
{
  ASSERT_EQ(plmn_string_to_bcd("00101"), 0xf110);
  ASSERT_EQ(plmn_string_to_bcd("20899"), 0x2f899);
}