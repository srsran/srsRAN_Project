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

#include "rrc_ue_test_helpers.h"
#include "rrc_ue_test_messages.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/support/async/async_task_loop.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class RRC DL info transfer tests preparation (to bring UE in RRC connected state)
class rrc_ue_dl_info_transfer : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override
  {
    init();

    srslog::basic_logger& rrc_logger = srslog::fetch_basic_logger("RRC", false);
    rrc_logger.set_level(srslog::basic_levels::debug);
    rrc_logger.set_hex_dump_max_size(32);

    connect_amf();
    receive_setup_request();

    // check if the RRC setup message was generated
    ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

    // check if SRB1 was created
    check_srb1_exists();

    receive_setup_complete();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

/// Test the RRC setup with connected AMF
TEST_F(rrc_ue_dl_info_transfer, when_srb2_missing_dl_info_tranfer_goes_over_srb1)
{
  send_dl_info_transfer({0x00, 0x01, 0x02, 0x03});
  asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::options type = get_srb1_pdu_type();
  ASSERT_EQ(asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::options::dl_info_transfer, type);
}

/// Test the RRC setup with connected AMF
TEST_F(rrc_ue_dl_info_transfer, when_srb2_present_dl_info_tranfer_goes_over_srb2)
{
  create_srb2();
  send_dl_info_transfer({0x00, 0x01, 0x02, 0x03});
  asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::options type = get_srb2_pdu_type();
  ASSERT_EQ(asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::options::dl_info_transfer, type);
}
