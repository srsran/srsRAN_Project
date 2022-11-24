/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/ngap/ngc_asn1_packer.h"
#include "ngc_test_helpers.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Fixture class for NGAP ASN1 packer
class ngc_asn1_packer_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("NGC-ASN1-PCK").set_level(srslog::basic_levels::debug);
    srslog::init();

    gw     = std::make_unique<dummy_network_gateway_data_handler>();
    ngc    = std::make_unique<dummy_ngc_message_handler>();
    packer = std::make_unique<srsgnb::ngc_asn1_packer>(*gw, *ngc);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<dummy_network_gateway_data_handler> gw;
  std::unique_ptr<dummy_ngc_message_handler>          ngc;
  std::unique_ptr<srsgnb::ngc_asn1_packer>            packer;
  srslog::basic_logger&                               test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test successful packing and compare with captured test vector
TEST_F(ngc_asn1_packer_test, when_packing_successful_then_pdu_matches_tv)
{
  // Populate message
  ngc_message ngc_msg = {};
  ngc_msg.pdu.set_init_msg();
  ngc_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_NG_SETUP);
  ngc_msg.pdu.init_msg().value.ng_setup_request() = generate_ng_setup_request_message().msg;

  // Pack message and forward to gateway
  packer->handle_message(ngc_msg);

  // print packed message and TV
  byte_buffer tv({ng_setup_request_packed, sizeof(ng_setup_request_packed)});
  test_logger.debug(tv.begin(), tv.end(), "Test vector ({} bytes):", tv.length());
  test_logger.debug(gw->last_pdu.begin(), gw->last_pdu.end(), "Packed PDU ({} bytes):", gw->last_pdu.length());

  // Compare packed message with captured test vector
  ASSERT_EQ(gw->last_pdu.length(), sizeof(ng_setup_request_packed));
  ASSERT_TRUE(std::equal(gw->last_pdu.begin(), gw->last_pdu.end(), ng_setup_request_packed));
}

/// Test successful packing and unpacking
TEST_F(ngc_asn1_packer_test, when_packing_successful_then_unpacking_successful)
{
  // Action 1: Create valid ngc message
  srs_cu_cp::ngc_message ng_setup_response = generate_ng_setup_response_message();

  // Action 2: Pack message and forward to gateway
  packer->handle_message(ng_setup_response);

  // Action 3: Unpack message and forward to ngc
  packer->handle_packed_pdu(std::move(gw->last_pdu));

  // Assert that the originally created message is equal to the unpacked message
  ASSERT_EQ(ngc->last_msg.pdu.type(), ng_setup_response.pdu.type());
}

/// Test unsuccessful packing
TEST_F(ngc_asn1_packer_test, when_packing_unsuccessful_then_message_not_forwarded)
{
  // Action 1: Generate, pack and forward valid message to bring gw into known state
  srs_cu_cp::ngc_message ng_setup_response = generate_ng_setup_response_message();
  packer->handle_message(ng_setup_response);
  // store size of valid pdu
  int valid_pdu_size = gw->last_pdu.length();

  // Action 2: Create invalid ngc message
  ngc_message ngc_msg = {};
  ngc_msg.pdu.set_successful_outcome();
  ngc_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);
  auto& setup_res = ngc_msg.pdu.successful_outcome().value.ng_setup_resp();
  setup_res->amf_name.value.from_string("open5gs-amf0");

  // Action 3: Pack message and forward to gateway
  packer->handle_message(ngc_msg);

  // check that msg was not forwarded to gw
  ASSERT_EQ(gw->last_pdu.length(), valid_pdu_size);
}

// TODO: test unsuccessful unpacking
