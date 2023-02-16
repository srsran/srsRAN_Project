/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_test_helpers.h"
#include "lib/e2/common/e2ap_asn1_packer.h"
#include <gtest/gtest.h>

using namespace srsgnb;

/// Fixture class for E2AP ASN1 packer
class e2_asn1_packer_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("E2-ASN1-PCK").set_level(srslog::basic_levels::debug);
    srslog::init();

    gw     = std::make_unique<dummy_network_gateway_data_handler>();
    e2     = std::make_unique<dummy_e2_message_handler>();
    packer = std::make_unique<srsgnb::e2ap_asn1_packer>(*gw, *e2);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<dummy_network_gateway_data_handler> gw;
  std::unique_ptr<dummy_e2_message_handler>           e2;
  std::unique_ptr<srsgnb::e2ap_asn1_packer>           packer;
  srslog::basic_logger&                               test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test successful packing and unpacking
TEST_F(e2_asn1_packer_test, when_packing_successful_then_unpacking_successful)
{
  // Action 1: Create valid e2 message
  e2_message e2_setup_request = generate_e2_setup_request();

  // Action 2: Pack message and forward to gateway
  packer->handle_message(e2_setup_request);

  // Action 3: Unpack message and forward to e2
  packer->handle_packed_pdu(std::move(gw->last_pdu));

  // Assert that the originally created message is equal to the unpacked message
  ASSERT_EQ(e2->last_msg.pdu.type(), e2_setup_request.pdu.type());
}
