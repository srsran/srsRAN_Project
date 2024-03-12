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

#include "rrc_ue_test_helpers.h"
#include "rrc_ue_test_messages.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class RRC Reconfiguration tests preparation (to bring UE in RRC connected state)
class rrc_ue_reconfig : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override
  {
    init();

    srslog::basic_logger& rrc_logger = srslog::fetch_basic_logger("RRC", false);
    rrc_logger.set_level(srslog::basic_levels::debug);
    rrc_logger.set_hex_dump_max_size(32);

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

/// Test the RRC reconfig with connected AMF
TEST_F(rrc_ue_reconfig, when_reconfig_complete_received_proc_successful)
{
  // Prepare args
  rrc_reconfiguration_procedure_request args = generate_rrc_reconfiguration_procedure_request();

  // Trigger Reconfig
  async_task<bool>         t = get_rrc_ue_control_message_handler()->handle_rrc_reconfiguration_request(args);
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());

  check_rrc_reconfig_pdu();

  // Receive Reconfig complete
  receive_reconfig_complete();

  ASSERT_TRUE(t.ready());
}
