/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/f1ap/common/f1ap_asn1_packer.h"
#include "test_helpers.h"
#include "unittests/f1ap/cu_cp/f1_cu_test_helpers.h"
#include "unittests/gateways/test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Fixture class for F1AP ASN1 packer
class f1c_asn1_packer_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("F1C-ASN1-PCK").set_level(srslog::basic_levels::debug);
    srslog::init();

    gw     = std::make_unique<dummy_network_gateway_data_handler>();
    f1c    = std::make_unique<dummy_f1c_message_handler>();
    packer = std::make_unique<srsgnb::f1ap_asn1_packer>(*gw, *f1c);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<dummy_network_gateway_data_handler> gw;
  std::unique_ptr<dummy_f1c_message_handler>          f1c;
  std::unique_ptr<srsgnb::f1ap_asn1_packer>           packer;
  srslog::basic_logger&                               test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test successful packing and unpacking
TEST_F(f1c_asn1_packer_test, when_packing_successful_then_unpacking_successful)
{
  // Action 1: Create valid f1c message
  f1c_message f1_setup_request = generate_f1_setup_request();

  // Action 2: Pack message and forward to gateway
  packer->handle_message(f1_setup_request);

  // Action 3: Unpack message and forward to f1c
  packer->handle_packed_pdu(std::move(gw->last_pdu));

  // Assert that the originally created message is equal to the unpacked message
  ASSERT_EQ(f1c->last_msg.pdu.type(), f1_setup_request.pdu.type());
}

/// Test unsuccessful packing
TEST_F(f1c_asn1_packer_test, when_packing_unsuccessful_then_message_not_forwarded)
{
  // Action 1: Generate, pack and forward valid message to bring gw into known state
  f1c_message f1_setup_request = generate_f1_setup_request();
  packer->handle_message(f1_setup_request);
  // store size of valid pdu
  int valid_pdu_size = gw->last_pdu.length();

  // Action 2: Create invalid f1c message
  f1c_message f1c_msg                         = generate_f1_setup_request();
  auto&       setup_req                       = f1c_msg.pdu.init_msg().value.f1_setup_request();
  setup_req->gnb_du_served_cells_list_present = true;
  setup_req->gnb_du_served_cells_list.value.clear();

  // Action 3: Pack message and forward to gateway
  packer->handle_message(f1c_msg);

  // check that msg was not forwarded to gw
  ASSERT_EQ(gw->last_pdu.length(), valid_pdu_size);
}

// TODO: test unsuccessful unpacking
