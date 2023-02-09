/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/ngap/ngap_asn1_utils.h"
#include "lib/ngap/ngc_asn1_packer.h"
#include "ngap_test_messages.h"
#include "test_helpers.h"
#include "unittests/gateways/test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

security::sec_as_key make_sec_as_key(std::string hex_str)
{
  byte_buffer          key_buf = make_byte_buffer(hex_str);
  security::sec_as_key key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

/// Fixture class for NGAP ASN1 packer
class ngc_asn1_packer_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("TEST").set_hex_dump_max_size(32);
    srslog::fetch_basic_logger("NGC-ASN1-PCK").set_level(srslog::basic_levels::debug);
    srslog::init();

    gw  = std::make_unique<dummy_network_gateway_data_handler>();
    ngc = std::make_unique<dummy_ngc_message_handler>();

    packer = std::make_unique<srsgnb::ngc_asn1_packer>(*gw, *ngc, pcap);
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
  ngap_pcap                                           pcap;
};

/// Test successful packing and compare with captured test vector
TEST_F(ngc_asn1_packer_test, when_packing_successful_then_pdu_matches_tv)
{
  // Populate message
  ngc_message ngc_msg = {};
  ngc_msg.pdu.set_init_msg();
  ngc_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_NG_SETUP);
  ngc_msg.pdu.init_msg().value.ng_setup_request() = generate_ng_setup_request().msg;

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
  srs_cu_cp::ngc_message ng_setup_response = generate_ng_setup_response();

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
  srs_cu_cp::ngc_message ng_setup_response = generate_ng_setup_response();
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

// test unpacking of initial context setup and
// correct key and algorithm preference list extraction
TEST_F(ngc_asn1_packer_test, when_unpack_init_ctx_extract_sec_params_correctly)
{
  std::string ngap_init_ctx_req =
      "000e008090000008000a0002000c005500020000001c00070000f1100200400000000200010077000918000c000000000000005e00205063"
      "6e38151d62356d9a1a0c9f2391885177307ad494be15281dfe5fdac06302002240080123456700ffff010026402f2e7e02cf5b405e017e00"
      "42010177000bf200f110020040dd00b06454072000f11000000715020101210201005e0129";

  // get expected security key
  const char*          security_key_cstr = "50636e38151d62356d9a1a0c9f2391885177307ad494be15281dfe5fdac06302";
  security::sec_as_key security_key      = make_sec_as_key(security_key_cstr);

  byte_buffer buf = make_byte_buffer(ngap_init_ctx_req);

  asn1::cbit_ref         bref(buf);
  srs_cu_cp::ngc_message msg = {};
  ASSERT_EQ(msg.pdu.unpack(bref), asn1::SRSASN_SUCCESS);

  const asn1::ngap::ngap_pdu_c&                   pdu     = msg.pdu;
  const asn1::ngap::init_context_setup_request_s& request = pdu.init_msg().value.init_context_setup_request();

  security::sec_as_key           security_key_o;
  security::supported_algorithms inte_algos;
  security::supported_algorithms ciph_algos;
  copy_asn1_key(security_key_o, *request->security_key);
  fill_supported_algorithms(inte_algos, request->ue_security_cap->nr_integrity_protection_algorithms);
  fill_supported_algorithms(ciph_algos, request->ue_security_cap->nr_encryption_algorithms);
  test_logger.debug("{}", inte_algos);
  test_logger.debug("{}", ciph_algos);

  ASSERT_EQ(true, inte_algos[0]);          // NIA1 supported
  ASSERT_EQ(true, inte_algos[0]);          // NEA1 supported
  ASSERT_EQ(true, inte_algos[1]);          // NIA2 supported
  ASSERT_EQ(true, inte_algos[1]);          // NEA2 supported
  ASSERT_EQ(false, inte_algos[2]);         // NIA3 not supported
  ASSERT_EQ(false, inte_algos[2]);         // NEA3 not supported
  ASSERT_EQ(security_key, security_key_o); // Key was correctly copied
}
