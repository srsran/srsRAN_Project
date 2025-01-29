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

TEST(phy_time_unit_test, ul_rtoa)
{
  // Use TS38.133 Table 13.1.1-1 entries as test points.
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985025).to_ul_rtoa(0), 0);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985024).to_ul_rtoa(0), 1);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985023).to_ul_rtoa(0), 2);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-2).to_ul_rtoa(0), 985023);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-1).to_ul_rtoa(0), 985024);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(0).to_ul_rtoa(0), 985024);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(1).to_ul_rtoa(0), 985025);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(2).to_ul_rtoa(0), 985026);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(3).to_ul_rtoa(0), 985027);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985023).to_ul_rtoa(0), 1970047);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985024).to_ul_rtoa(0), 1970048);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985025).to_ul_rtoa(0), 1970049);

  // Use TS38.133 Table 13.1.1-2 entries as test points.
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985025).to_ul_rtoa(1), 0);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985024).to_ul_rtoa(1), 1);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985023).to_ul_rtoa(1), 1);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985022).to_ul_rtoa(1), 2);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985021).to_ul_rtoa(1), 2);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-4).to_ul_rtoa(1), 492511);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-3).to_ul_rtoa(1), 492511);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-2).to_ul_rtoa(1), 492512);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-1).to_ul_rtoa(1), 492512);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(0).to_ul_rtoa(1), 492512);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(1).to_ul_rtoa(1), 492513);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(2).to_ul_rtoa(1), 492513);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(3).to_ul_rtoa(1), 492514);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(4).to_ul_rtoa(1), 492514);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(5).to_ul_rtoa(1), 492515);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(6).to_ul_rtoa(1), 492515);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985023).to_ul_rtoa(1), 985024);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985024).to_ul_rtoa(1), 985024);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985025).to_ul_rtoa(1), 985025);

  // Use TS38.133 Table 13.1.1-3 entries as test points.
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985025).to_ul_rtoa(2), 0);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985024).to_ul_rtoa(2), 1);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985021).to_ul_rtoa(2), 1);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985020).to_ul_rtoa(2), 2);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985017).to_ul_rtoa(2), 2);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-8).to_ul_rtoa(2), 246255);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-5).to_ul_rtoa(2), 246255);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-4).to_ul_rtoa(2), 246256);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-1).to_ul_rtoa(2), 246256);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(0).to_ul_rtoa(2), 246256);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(1).to_ul_rtoa(2), 246257);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(4).to_ul_rtoa(2), 246257);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(5).to_ul_rtoa(2), 246258);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(8).to_ul_rtoa(2), 246258);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(9).to_ul_rtoa(2), 246259);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(12).to_ul_rtoa(2), 246259);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985021).to_ul_rtoa(2), 492512);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985024).to_ul_rtoa(2), 492512);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985025).to_ul_rtoa(2), 492513);

  // Use TS38.133 Table 13.1.1-4 entries as test points.
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985025).to_ul_rtoa(3), 0);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985024).to_ul_rtoa(3), 1);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985017).to_ul_rtoa(3), 1);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985016).to_ul_rtoa(3), 2);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-985009).to_ul_rtoa(3), 2);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-16).to_ul_rtoa(3), 123127);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-9).to_ul_rtoa(3), 123127);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-8).to_ul_rtoa(3), 123128);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(-1).to_ul_rtoa(3), 123128);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(0).to_ul_rtoa(3), 123128);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(1).to_ul_rtoa(3), 123129);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(8).to_ul_rtoa(3), 123129);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(9).to_ul_rtoa(3), 123130);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(16).to_ul_rtoa(3), 123130);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(17).to_ul_rtoa(3), 123131);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(24).to_ul_rtoa(3), 123131);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985017).to_ul_rtoa(3), 246256);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985024).to_ul_rtoa(3), 246256);
  ASSERT_EQ(phy_time_unit::from_units_of_Tc(985025).to_ul_rtoa(3), 246257);

  // Assume TS38.133 Tables 13.1.1-5/6 are covered from the cases above.
  // ...
}
