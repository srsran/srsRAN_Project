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
#include "srsran/cu_cp/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class RRC Reestablishment tests preparation
class rrc_ue_reest : public rrc_ue_test_helper, public ::testing::Test
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
TEST_F(rrc_ue_reest, when_invalid_reestablishment_request_received_then_rrc_setup_sent)
{
  connect_amf();
  receive_invalid_reestablishment_request(0, to_rnti(0x4601));

  // check if the RRC Setup Request was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // check if SRB1 was created
  check_srb1_exists();

  receive_setup_complete();

  check_initial_ue_message_sent();
}

/// Test the RRC Reestablishment
TEST_F(rrc_ue_reest, when_valid_reestablishment_request_received_but_security_context_not_found_then_rrc_setup_sent)
{
  connect_amf();
  receive_valid_reestablishment_request(1, to_rnti(0x4601));

  // check if the RRC Setup Request was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // check if SRB1 was created
  check_srb1_exists();

  receive_setup_complete();

  check_initial_ue_message_sent();
}

/// Test the RRC Reestablishment
TEST_F(rrc_ue_reest,
       when_valid_reestablishment_request_for_different_du_received_then_rrc_reestablishment_without_old_ue_index_sent)
{
  connect_amf();
  ue_index_t old_ue_index = generate_ue_index(uint_to_du_index(1), 2);
  add_ue_reestablishment_context(old_ue_index);
  receive_valid_reestablishment_request(1, to_rnti(0x4601));

  // check if SRB1 was created
  check_srb1_exists();

  // check if the RRC Reestablishment was generated
  ASSERT_EQ(get_srb1_pdu_type(), asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types::rrc_reest);
  // check if the old ue index was send to the f1ap
  ASSERT_EQ(get_old_ue_index(), ue_index_t::invalid);

  receive_reestablishment_complete();
}

/// Test the RRC Reestablishment
TEST_F(rrc_ue_reest,
       when_valid_reestablishment_request_for_same_du_received_then_rrc_reestablishment_with_old_ue_index_sent)
{
  connect_amf();
  ue_index_t old_ue_index = uint_to_ue_index(0);
  add_ue_reestablishment_context(old_ue_index);
  receive_valid_reestablishment_request(1, to_rnti(0x4601));

  // check if SRB1 was created
  check_srb1_exists();

  // check if the RRC Reestablishment was generated
  ASSERT_EQ(get_srb1_pdu_type(), asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types::rrc_reest);
  // check if the old ue index was send to the f1ap
  ASSERT_EQ(get_old_ue_index(), old_ue_index);

  receive_reestablishment_complete();
}
