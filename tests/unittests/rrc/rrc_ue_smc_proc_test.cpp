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
#include "srsran/rrc/rrc_du_factory.h"
#include "srsran/support/async/async_task_loop.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class RRC Setup tests preparation
class rrc_ue_smc : public rrc_ue_test_helper, public ::testing::Test
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
TEST_F(rrc_ue_smc, when_key_provided_smc_generated)
{
  const char* sk_gnb_cstr = "45cbc3f8a81193fd5c5229300d59edf812e998a115ec4e0ce903ba89367e2628";
  const char* k_enc_cstr  = "4ea96992c8c7e82977231ad001309062ae9f31ead90a4d0842af6cd25cb44dc4";
  const char* k_int_cstr  = "aeeb5e0ae02c6188ecb1625c4a9e022fdfc2a1fc845b44b44443ac9a3bda667c";

  // Pack hex strings into srsgnb types
  security::sec_key sk_gnb = make_sec_key(sk_gnb_cstr);
  security::sec_key k_enc  = make_sec_key(k_enc_cstr);
  security::sec_key k_int  = make_sec_key(k_int_cstr);

  // Create expected SRB1 sec config
  security::sec_as_config sec_cfg         = {};
  sec_cfg.domain                          = security::sec_domain::rrc;
  sec_cfg.integ_algo                      = security::integrity_algorithm::nia2;
  sec_cfg.cipher_algo                     = security::ciphering_algorithm::nea0;
  sec_cfg.k_enc                           = k_enc;
  sec_cfg.k_int                           = k_int;
  security::sec_128_as_config sec_128_cfg = security::truncate_config(sec_cfg);

  // Initialize security context and capabilities.
  security::security_context init_sec_ctx = {};
  init_sec_ctx.k                          = sk_gnb;
  std::fill(init_sec_ctx.supported_int_algos.begin(), init_sec_ctx.supported_int_algos.end(), true);
  std::fill(init_sec_ctx.supported_enc_algos.begin(), init_sec_ctx.supported_enc_algos.end(), true);

  // Make sure SRB1 PDCP is not configured
  check_integrity_enabled(false);
  check_ciphering_enabled(false);
  check_security_configured(false, {});

  // Trigger SMC
  async_task<bool>         t = get_rrc_ue_security_handler()->handle_init_security_context(init_sec_ctx);
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());
  check_smc_pdu();

  // Make sure SRB1 PDCP is configured
  check_integrity_enabled(true);
  check_ciphering_enabled(true);
  check_security_configured(true, sec_128_cfg);

  // Receive SMC complete
  receive_smc_complete();

  ASSERT_TRUE(t.ready());
}

TEST_F(rrc_ue_smc, when_reply_missing_procedure_timeout)
{
  // Initialize security context and capabilities.
  security::security_context init_sec_ctx = {};
  std::fill(init_sec_ctx.supported_int_algos.begin(), init_sec_ctx.supported_int_algos.end(), true);
  std::fill(init_sec_ctx.supported_enc_algos.begin(), init_sec_ctx.supported_enc_algos.end(), true);

  // Trigger SMC
  async_task<bool>         t = get_rrc_ue_security_handler()->handle_init_security_context(init_sec_ctx);
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());
  check_smc_pdu();

  // check that UE has been created and was not requested to be released
  check_ue_release_not_requested();

  // tick timer until RRC setup complete timer fires
  tick_timer();

  // verify that RRC requested UE context release
  check_ue_release_requested();

  ASSERT_TRUE(t.ready());
}
