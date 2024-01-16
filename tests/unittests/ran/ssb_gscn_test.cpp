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

#include "srsran/ran/ssb_gscn.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(test_get_ss_ref_from_gscn, mixed_bands)
{
  ASSERT_DOUBLE_EQ(2373750e3, band_helper::get_ss_ref_from_gscn(5934U));
  ASSERT_DOUBLE_EQ(2804450e3, band_helper::get_ss_ref_from_gscn(7010U));
  ASSERT_DOUBLE_EQ(2999.05e6, band_helper::get_ss_ref_from_gscn(7498U));
  ASSERT_DOUBLE_EQ(3000e6, band_helper::get_ss_ref_from_gscn(7499U));
  ASSERT_DOUBLE_EQ(1837450e3, band_helper::get_ss_ref_from_gscn(4594U));
  ASSERT_DOUBLE_EQ(24248.64e6, band_helper::get_ss_ref_from_gscn(22255U));
  ASSERT_DOUBLE_EQ(24250.08e6, band_helper::get_ss_ref_from_gscn(22256U));
  ASSERT_DOUBLE_EQ(37763.04e6, band_helper::get_ss_ref_from_gscn(23038U));
}

TEST(test_get_gscn_from_ss_ref, mixed_bands)
{
  optional<unsigned> gscn = band_helper::get_gscn_from_ss_ref(2373750e3);
  ASSERT_TRUE(gscn.has_value());
  ASSERT_EQ(5934U, gscn.value());

  gscn.reset();
  gscn = band_helper::get_gscn_from_ss_ref(2373750.12e3);
  ASSERT_FALSE(gscn.has_value());

  gscn.reset();
  gscn = band_helper::get_gscn_from_ss_ref(2804450e3);
  ASSERT_TRUE(gscn.has_value());
  ASSERT_EQ(7010U, gscn.value());

  gscn.reset();
  gscn = band_helper::get_gscn_from_ss_ref(1837450e3);
  ASSERT_TRUE(gscn.has_value());
  ASSERT_EQ(4594U, gscn.value());

  gscn.reset();
  gscn = band_helper::get_gscn_from_ss_ref(2999.05e6);
  ASSERT_TRUE(gscn.has_value());
  ASSERT_EQ(7498U, gscn.value());

  gscn.reset();
  gscn = band_helper::get_gscn_from_ss_ref(3000e6);
  ASSERT_TRUE(gscn.has_value());
  ASSERT_EQ(7499U, gscn.value());

  gscn.reset();
  gscn = band_helper::get_gscn_from_ss_ref(3000.012e6);
  ASSERT_FALSE(gscn.has_value());

  gscn.reset();
  gscn = band_helper::get_gscn_from_ss_ref(24248.64e6);
  ASSERT_TRUE(gscn.has_value());
  ASSERT_EQ(22255U, gscn.value());

  gscn.reset();
  gscn = band_helper::get_gscn_from_ss_ref(24250.08e6);
  ASSERT_TRUE(gscn.has_value());
  ASSERT_EQ(22256U, gscn.value());

  gscn.reset();
  gscn = band_helper::get_gscn_from_ss_ref(37763040e3);
  ASSERT_TRUE(gscn.has_value());
  ASSERT_EQ(23038U, gscn.value());

  gscn.reset();
  gscn = band_helper::get_gscn_from_ss_ref(73515.36e6);
  ASSERT_TRUE(gscn.has_value());
  ASSERT_EQ(25107U, gscn.value());
}

TEST(test_is_gscn_valid_given_band, mixed_bands)
{
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(5300U, nr_band::n1, subcarrier_spacing::kHz15).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(5500U, nr_band::n1, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(4518U, nr_band::n3, subcarrier_spacing::kHz15).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(4501U, nr_band::n3, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(2177U, nr_band::n5, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(2224U, nr_band::n5, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(6618U, nr_band::n7, subcarrier_spacing::kHz15).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(6818U, nr_band::n3, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(6618U, nr_band::n7, subcarrier_spacing::kHz15).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(6818U, nr_band::n3, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(3892U, nr_band::n24, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(3886U, nr_band::n24, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(5043U, nr_band::n34, subcarrier_spacing::kHz15).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(5045U, nr_band::n34, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(5045U, nr_band::n34, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(5051U, nr_band::n34, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(6479U, nr_band::n38, subcarrier_spacing::kHz15).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(6494U, nr_band::n38, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(6494U, nr_band::n38, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(6433U, nr_band::n38, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(4747U, nr_band::n39, subcarrier_spacing::kHz15).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(4771U, nr_band::n39, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(6696U, nr_band::n41, subcarrier_spacing::kHz15).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(6695U, nr_band::n41, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(6714U, nr_band::n41, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(6712U, nr_band::n41, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(9444U, nr_band::n46, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(9447U, nr_band::n46, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(7911U, nr_band::n48, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(9447U, nr_band::n48, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(7911U, nr_band::n78, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(7700U, nr_band::n78, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(
                   8720U, nr_band::n79, subcarrier_spacing::kHz30, bs_channel_bandwidth_fr1::MHz40)
                   .is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(
                  8716U, nr_band::n79, subcarrier_spacing::kHz30, bs_channel_bandwidth_fr1::MHz40)
                  .is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(
                   8716U, nr_band::n79, subcarrier_spacing::kHz30, bs_channel_bandwidth_fr1::MHz10)
                   .is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(
                  8885U, nr_band::n79, subcarrier_spacing::kHz30, bs_channel_bandwidth_fr1::MHz10)
                  .is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(
                   6717U, nr_band::n90, subcarrier_spacing::kHz15, bs_channel_bandwidth_fr1::MHz10)
                   .is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(
                  6718U, nr_band::n90, subcarrier_spacing::kHz15, bs_channel_bandwidth_fr1::MHz10)
                  .is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(
                   6718U, nr_band::n90, subcarrier_spacing::kHz15, bs_channel_bandwidth_fr1::MHz5)
                   .is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(
                  6244U, nr_band::n90, subcarrier_spacing::kHz15, bs_channel_bandwidth_fr1::MHz5)
                  .is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(6554U, nr_band::n90, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(6250U, nr_band::n90, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(10062U, nr_band::n96, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(9991U, nr_band::n96, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(4754U, nr_band::n101, subcarrier_spacing::kHz15).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(4750U, nr_band::n101, subcarrier_spacing::kHz15).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(4760U, nr_band::n101, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(4754U, nr_band::n101, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(9798U, nr_band::n102, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(9799U, nr_band::n102, subcarrier_spacing::kHz30).is_error());
  ASSERT_FALSE(band_helper::is_gscn_valid_given_band(10169U, nr_band::n104, subcarrier_spacing::kHz30).is_error());
  ASSERT_TRUE(band_helper::is_gscn_valid_given_band(10165U, nr_band::n104, subcarrier_spacing::kHz30).is_error());
}
