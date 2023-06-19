/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include <algorithm>
#include <array>
#include <cstdint>
#include <gtest/gtest.h>
#include <string>

TEST(srs_sib19_test, make_asn1_rrc_cell_sib19_buffer)
{
  using namespace srsran;

  ntn_config ntn_cfg;
  // Create an NTN configuration to use in the test
  ntn_cfg.epoch_time.sfn             = 1;
  ntn_cfg.epoch_time.subframe_number = 9;
  ntn_cfg.cell_specific_koffset      = 123;
  ntn_cfg.k_mac                      = 456;
  ntn_cfg.ta_info.ta_common          = 10;

  // Call the function being tested
  std::string js_str;
  auto        buf = srsran::srs_du::make_asn1_rrc_cell_sib19_buffer(ntn_cfg, &js_str);

  // Check that the buffer is not empty
  EXPECT_FALSE(buf.empty());

  // Check that the JSON string is not empty
  EXPECT_FALSE(js_str.empty());

  // Decode the buffer to verify its contents
  asn1::cbit_ref            bref(buf);
  asn1::rrc_nr::sib19_r17_s sib19;
  asn1::SRSASN_CODE         ret = sib19.unpack(bref);
  EXPECT_EQ(ret, asn1::SRSASN_SUCCESS);

  // Check that the decoded SIB19 matches the NTN configuration used in the test
  EXPECT_TRUE(sib19.ntn_cfg_r17_present);
  EXPECT_EQ(sib19.ntn_cfg_r17.cell_specific_koffset_r17, ntn_cfg.cell_specific_koffset);
  EXPECT_EQ(sib19.ntn_cfg_r17.kmac_r17, ntn_cfg.k_mac);
  EXPECT_TRUE(sib19.ntn_cfg_r17.ta_info_r17_present);
  EXPECT_EQ(sib19.ntn_cfg_r17.ta_info_r17.ta_common_r17, ntn_cfg.ta_info.ta_common);
  EXPECT_TRUE(sib19.ntn_cfg_r17.ephemeris_info_r17_present);
}
