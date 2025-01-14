/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/phy_time_unit.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(phy_time_unit_test, build_from_negative_seconds_store_negative_tc)
{
  float         time     = -0.000001f;
  phy_time_unit phy_time = phy_time_unit::from_seconds(time);

  ASSERT_LT(phy_time.to_Tc(), 0);
  ASSERT_NEAR(time, phy_time.to_seconds<float>(), 1e-9);
}

TEST(phy_time_unit_test, build_from_negative_tc_store_negative_tc)
{
  int           time     = -100;
  phy_time_unit phy_time = phy_time_unit::from_units_of_Tc(time);

  ASSERT_EQ(phy_time.to_Tc(), time);
}

TEST(phy_time_unit_test, build_from_negative_ta_store_negative_tc)
{
  const subcarrier_spacing scs      = subcarrier_spacing::kHz30;
  int                      time     = -100;
  phy_time_unit            phy_time = phy_time_unit::from_timing_advance(time, scs);

  ASSERT_EQ(phy_time.to_Tc(), time * 64 * 16 / static_cast<int>(pow2(to_numerology_value(scs))));
  ASSERT_LT(phy_time.to_Tc(), 0);
}

TEST(phy_time_unit_test, store_positive_tc)
{
  unsigned      time     = 10;
  phy_time_unit phy_time = phy_time_unit::from_units_of_Tc(time);

  ASSERT_EQ(phy_time.to_Tc(), time);
}

TEST(phy_time_unit_test, store_positive_ta)
{
  unsigned                 time     = 10;
  const subcarrier_spacing scs      = subcarrier_spacing::kHz30;
  phy_time_unit            phy_time = phy_time_unit::from_timing_advance(time, scs);

  ASSERT_EQ(phy_time.to_Ta(scs), time);
}

TEST(phy_time_unit_test, store_positive_seconds)
{
  float         time     = 0.00001f;
  phy_time_unit phy_time = phy_time_unit::from_seconds(time);

  ASSERT_GT(phy_time.to_Tc(), 0);
  ASSERT_NEAR(time, phy_time.to_seconds<float>(), 1e-9);
}
