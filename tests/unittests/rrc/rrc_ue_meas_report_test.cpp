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

#include "lib/rrc/ue/rrc_measurement_types_asn1_converters.h"
#include "rrc_ue_test_helpers.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class RRC Reestablishment tests preparation
class rrc_ue_meas_report : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override { init(); }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

TEST_F(rrc_ue_meas_report, when_dummy_meas_report_received_then_conversion_successful)
{
  byte_buffer meas_report_pdu = generate_measurement_report_pdu();

  // Parse UL-CCCH
  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;
  asn1::cbit_ref              bref({meas_report_pdu.begin(), meas_report_pdu.end()});
  ASSERT_EQ(ul_dcch_msg.unpack(bref), asn1::SRSASN_SUCCESS);
  ASSERT_EQ(ul_dcch_msg.msg.type().value, asn1::rrc_nr::ul_dcch_msg_type_c::types_opts::c1);
  ASSERT_EQ(ul_dcch_msg.msg.c1().type().value, asn1::rrc_nr::ul_dcch_msg_type_c::c1_c_::types_opts::meas_report);

  rrc_meas_results meas_results =
      asn1_to_measurement_results(ul_dcch_msg.msg.c1().meas_report().crit_exts.meas_report().meas_results, logger);

  // check if the meas results conversion was successful
  check_meas_results(meas_results);
}

TEST_F(rrc_ue_meas_report, when_invalid_meas_report_received_then_meas_results_are_empty)
{
  std::vector<uint8_t> fuzzed_data{0x01, 0xe2, 0x02, 0xdd, 0x00, 0x5d, 0x5e, 0xaf, 0xe0, 0x51, 0x07, 0x20, 0x04,
                                   0x03, 0xe4, 0x82, 0x07, 0x00, 0x00, 0x02, 0x00, 0x02, 0xbd, 0x60, 0x6c, 0x78,
                                   0x0a, 0xd5, 0x2f, 0x70, 0x70, 0xfb, 0xa3, 0xc0, 0xc1, 0xb9, 0xbf, 0x01, 0x43,
                                   0xe0, 0x01, 0x66, 0x07, 0x08, 0xcf, 0x92, 0x81, 0xc0, 0x40, 0x00, 0x60, 0x00,
                                   0x01, 0x00, 0x81, 0x05, 0x25, 0xb9, 0xb4, 0xad, 0x44, 0x06, 0xa0, 0xd3, 0x0f,
                                   0x4a, 0x6f, 0x10, 0x20, 0xc0, 0xfc, 0x00, 0xfb, 0x85, 0xcd, 0x8e, 0x00, 0x00};
  srsran::byte_buffer  meas_report_pdu = srsran::byte_buffer::create(fuzzed_data).value();

  // Parse UL-CCCH
  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;
  asn1::cbit_ref              bref({meas_report_pdu.begin(), meas_report_pdu.end()});
  ASSERT_EQ(ul_dcch_msg.unpack(bref), asn1::SRSASN_SUCCESS);
  ASSERT_EQ(ul_dcch_msg.msg.type().value, asn1::rrc_nr::ul_dcch_msg_type_c::types_opts::c1);
  ASSERT_EQ(ul_dcch_msg.msg.c1().type().value, asn1::rrc_nr::ul_dcch_msg_type_c::c1_c_::types_opts::meas_report);

  rrc_meas_results meas_results =
      asn1_to_measurement_results(ul_dcch_msg.msg.c1().meas_report().crit_exts.meas_report().meas_results, logger);

  ASSERT_TRUE(meas_results.meas_result_neigh_cells.has_value() and
              meas_results.meas_result_neigh_cells->meas_result_list_nr.empty());
}
