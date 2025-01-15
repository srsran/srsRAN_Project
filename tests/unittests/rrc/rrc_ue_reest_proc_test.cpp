/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  receive_valid_reestablishment_request(1, to_rnti(0x4601));

  // check if the RRC Setup Request was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // check if SRB1 was created
  check_srb1_exists();

  receive_setup_complete();

  check_initial_ue_message_sent();
}

/// Test the RRC Reestablishment
TEST_F(rrc_ue_reest, when_reestablishment_request_with_cause_recfg_fail_received_then_rrc_setup_sent)
{
  ue_index_t old_ue_index = uint_to_ue_index(0);
  add_ue_reestablishment_context(old_ue_index);
  receive_valid_reestablishment_request_with_cause_recfg_fail(1, to_rnti(0x4601));

  // check if the RRC Setup Request was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // check if SRB1 was created
  check_srb1_exists();

  receive_setup_complete();

  check_initial_ue_message_sent();
}

/// Test the RRC Reestablishment
TEST_F(rrc_ue_reest,
       when_valid_reestablishment_request_for_same_du_received_then_rrc_reestablishment_with_old_ue_index_sent)
{
  ue_index_t old_ue_index = uint_to_ue_index(0);
  add_ue_reestablishment_context(old_ue_index);
  receive_valid_reestablishment_request(1, to_rnti(0x4601));

  // check if SRB1 was created
  check_srb1_exists();

  // check if the RRC message was sent over SRB1
  ASSERT_EQ(get_last_srb(), srb_id_t::srb1);

  receive_reestablishment_complete();
}
