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

#include "lib/du_manager/converters/f1ap_configuration_helpers.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include "srsran/ran/sib/system_info_config.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <gtest/gtest.h>
#include <string>

TEST(srs_sib19_test, make_asn1_rrc_cell_sib19_buffer)
{
  using namespace srsran;

  sib19_info sib19;
  sib19.cell_specific_koffset.emplace();
  sib19.cell_specific_koffset.value() = 260;
  sib19.ephemeris_info.emplace();
  sib19.ephemeris_info.value().position_x  = 1;
  sib19.ephemeris_info.value().position_y  = 2;
  sib19.ephemeris_info.value().position_z  = 3;
  sib19.ephemeris_info.value().velocity_vx = 4;
  sib19.ephemeris_info.value().velocity_vy = 5;
  sib19.ephemeris_info.value().velocity_vz = 6;
  // Call the function being tested
  std::string js_str;
  auto        buf = srsran::srs_du::make_asn1_rrc_cell_sib19_buffer(sib19, &js_str);

  // Check that the buffer is not empty
  EXPECT_FALSE(buf.empty());

  // Check that the JSON string is not empty
  EXPECT_FALSE(js_str.empty());

  // Decode the buffer to verify its contents
  asn1::cbit_ref            bref(buf);
  asn1::rrc_nr::sib19_r17_s sib19_decoded;
  asn1::SRSASN_CODE         ret = sib19_decoded.unpack(bref);
  EXPECT_EQ(ret, asn1::SRSASN_SUCCESS);

  // Check that the decoded SIB19 matches the SIB19 configuration used in the test
  EXPECT_TRUE(sib19_decoded.ntn_cfg_r17_present);
  EXPECT_EQ(sib19_decoded.ntn_cfg_r17.cell_specific_koffset_r17, sib19.cell_specific_koffset);
  EXPECT_TRUE(sib19_decoded.ntn_cfg_r17.ephemeris_info_r17_present);
  EXPECT_EQ(sib19_decoded.ntn_cfg_r17.ephemeris_info_r17.position_velocity_r17().position_x_r17,
            sib19.ephemeris_info.value().position_x);
}