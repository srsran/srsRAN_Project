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

#include "lib/rrc/ue/rrc_measurement_types_asn1_converters.h"
#include "rrc_ue_test_helpers.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg.h"
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

/// Test the RRC Reestablishment
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
      asn1_to_measurement_results(ul_dcch_msg.msg.c1().meas_report().crit_exts.meas_report().meas_results);

  // check if the meas results conversion was successful
  check_meas_results(meas_results);
}
