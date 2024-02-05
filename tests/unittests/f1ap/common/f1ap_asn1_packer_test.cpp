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

#include "lib/f1ap/common/f1ap_asn1_packer.h"
#include "test_helpers.h"
#include "tests/unittests/f1ap/cu_cp/f1ap_cu_test_helpers.h"
#include "tests/unittests/gateways/test_helpers.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class for F1AP ASN1 packer
class f1ap_asn1_packer_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("F1AP-ASN1-PCK").set_level(srslog::basic_levels::debug);
    srslog::init();

    gw     = std::make_unique<dummy_network_gateway_data_handler>();
    f1ap   = std::make_unique<dummy_f1ap_message_handler>();
    packer = std::make_unique<srsran::f1ap_asn1_packer>(*gw, *f1ap);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<dummy_network_gateway_data_handler> gw;
  std::unique_ptr<dummy_f1ap_message_handler>         f1ap;
  std::unique_ptr<srsran::f1ap_asn1_packer>           packer;
  srslog::basic_logger&                               test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test successful packing and unpacking
TEST_F(f1ap_asn1_packer_test, when_packing_successful_then_unpacking_successful)
{
  // Action 1: Create valid f1ap message
  f1ap_message f1_setup_request = generate_f1_setup_request();

  // Action 2: Pack message and forward to gateway
  packer->handle_message(f1_setup_request);

  // Action 3: Unpack message and forward to f1ap
  packer->handle_packed_pdu(std::move(gw->last_pdu));

  // Assert that the originally created message is equal to the unpacked message
  ASSERT_EQ(f1ap->last_msg.pdu.type(), f1_setup_request.pdu.type());
}

/// Test unsuccessful packing
TEST_F(f1ap_asn1_packer_test, when_packing_unsuccessful_then_message_not_forwarded)
{
  // Action 1: Generate, pack and forward valid message to bring gw into known state
  f1ap_message f1_setup_request = generate_f1_setup_request();
  packer->handle_message(f1_setup_request);
  // store size of valid pdu
  int valid_pdu_size = gw->last_pdu.length();

  // Action 2: Create invalid f1ap message
  f1ap_message f1ap_msg                       = generate_f1_setup_request();
  auto&        setup_req                      = f1ap_msg.pdu.init_msg().value.f1_setup_request();
  setup_req->gnb_du_served_cells_list_present = true;
  setup_req->gnb_du_served_cells_list.clear();

  // Action 3: Pack message and forward to gateway
  packer->handle_message(f1ap_msg);

  // check that msg was not forwarded to gw
  ASSERT_EQ(gw->last_pdu.length(), valid_pdu_size);
}

// TODO: test unsuccessful unpacking
