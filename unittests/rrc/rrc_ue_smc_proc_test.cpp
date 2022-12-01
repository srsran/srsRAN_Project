/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_ue_test_helpers.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/rrc/rrc_du_factory.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Fixture class RRC Setup tests preparation
class rrc_smc : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override
  {
    init();

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
TEST_F(rrc_smc, when_key_provided_smc_generated)
{
  // Initialize security context and capabilities.
  rrc_init_security_context init_sec_ctx = {};
  std::fill(init_sec_ctx.supported_int_algos.begin(), init_sec_ctx.supported_int_algos.end(), true);
  std::fill(init_sec_ctx.supported_enc_algos.begin(), init_sec_ctx.supported_enc_algos.end(), true);

  // Make sure SRB1 PDCP is not configured
  check_integrity_enabled(false);
  check_ciphering_enabled(false);
  check_security_configured(false);

  // Trigger SMC
  init_security_context(init_sec_ctx);
  check_smc_pdu();

  // Make sure SRB1 PDCP is configured
  check_integrity_enabled(true);
  check_ciphering_enabled(true);
  check_security_configured(true);

  // Receive SMC complete
  receive_smc_complete();
}

TEST_F(rrc_smc, when_reply_missing_procedure_timeout)
{
  // Initialize security context and capabilities.
  rrc_init_security_context init_sec_ctx = {};
  std::fill(init_sec_ctx.supported_int_algos.begin(), init_sec_ctx.supported_int_algos.end(), true);
  std::fill(init_sec_ctx.supported_enc_algos.begin(), init_sec_ctx.supported_enc_algos.end(), true);

  // Trigger SMC
  init_security_context(init_sec_ctx);
  check_smc_pdu();

  // check that UE has been created and was not requested to be released
  check_ue_release_not_requested();

  // tick timer until RRC setup complete timer fires
  tick_timer();

  // verify that RRC requested UE context release
  check_ue_release_requested();
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
