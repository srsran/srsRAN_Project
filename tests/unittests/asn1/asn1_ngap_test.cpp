/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace std;
using namespace asn1;

#define JSON_OUTPUT 1

class asn1_ngap_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ASN1").set_hex_dump_max_size(-1);

    test_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    srslog::init();

    // Start the log backend.
    srslog::init();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

// initiating message NGSetupRequest
TEST_F(asn1_ngap_test, when_setup_message_correct_then_packing_successful)
{
  asn1::ngap::ngap_pdu_c pdu;

  pdu.set_init_msg();
  pdu.init_msg().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& setup_req = pdu.init_msg().value.ng_setup_request();
  setup_req->global_ran_node_id.value.set_global_gnb_id();
  setup_req->global_ran_node_id.value.global_gnb_id().gnb_id.set_gnb_id();
  setup_req->global_ran_node_id.value.global_gnb_id().gnb_id.gnb_id().from_number(411);
  setup_req->global_ran_node_id.value.global_gnb_id().plmn_id.from_string("00f110");

  setup_req->ran_node_name_present = true;
  setup_req->ran_node_name.value.from_string("srsgnb01");

  setup_req->supported_ta_list.id   = ASN1_NGAP_ID_SUPPORTED_TA_LIST;
  setup_req->supported_ta_list.crit = crit_opts::reject;

  asn1::ngap::supported_ta_item_s   supported_ta_item   = {};
  asn1::ngap::broadcast_plmn_item_s broadcast_plmn_item = {};
  broadcast_plmn_item.plmn_id.from_string("00f110");
  asn1::ngap::slice_support_item_s slice_support_item = {};
  slice_support_item.s_nssai.sst.from_number(1);
  broadcast_plmn_item.tai_slice_support_list.push_back(slice_support_item);
  supported_ta_item.broadcast_plmn_list.push_back(broadcast_plmn_item);

  setup_req->supported_ta_list.value.push_back(supported_ta_item);

  setup_req->default_paging_drx.value.value = asn1::ngap::paging_drx_opts::v256;

  srsran::byte_buffer tx_buffer;
  asn1::bit_ref       bref(tx_buffer);
  ASSERT_EQ(pdu.pack(bref), SRSASN_SUCCESS);

  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);

  std::vector<uint8_t> bytes{tx_buffer.begin(), tx_buffer.end()};
#if JSON_OUTPUT
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(
      bytes.data(), bytes.size(), "NGAP PDU unpacked ({} B): \n {}", bytes.size(), json_writer1.to_string().c_str());
#endif
}

// successful outcome NGSetupResponse
TEST_F(asn1_ngap_test, when_setup_response_correct_then_packing_successful)
{
  asn1::ngap::ngap_pdu_c pdu;

  pdu.set_successful_outcome();
  pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& setup_res = pdu.successful_outcome().value.ng_setup_resp();
  setup_res->amf_name.value.from_string("open5gs-amf0");

  asn1::ngap::served_guami_item_s served_guami_item;
  served_guami_item.guami.plmn_id.from_string("00f110");
  served_guami_item.guami.amf_region_id.from_number(2);
  served_guami_item.guami.amf_set_id.from_number(1);
  served_guami_item.guami.amf_pointer.from_number(0);
  setup_res->served_guami_list.value.push_back(served_guami_item);

  setup_res->relative_amf_capacity.value.value = 255;

  asn1::ngap::plmn_support_item_s plmn_support_item = {};
  plmn_support_item.plmn_id.from_string("00f110");

  asn1::ngap::slice_support_item_s slice_support_item = {};
  slice_support_item.s_nssai.sst.from_number(1);
  plmn_support_item.slice_support_list.push_back(slice_support_item);

  setup_res->plmn_support_list.value.push_back(plmn_support_item);

  srsran::byte_buffer tx_pdu;
  asn1::bit_ref       bref(tx_pdu);
  ASSERT_EQ(pdu.pack(bref), SRSASN_SUCCESS);

  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);

  std::vector<uint8_t> tx_buffer{tx_pdu.begin(), tx_pdu.end()};
#if JSON_OUTPUT
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(tx_buffer.data(),
                   tx_buffer.size(),
                   "NGAP PDU unpacked ({} B): \n {}",
                   tx_buffer.size(),
                   json_writer1.to_string().c_str());
#endif
}

// unsuccessful outcome NGSetupFailure
TEST_F(asn1_ngap_test, when_setup_failure_correct_then_packing_successful)
{
  asn1::ngap::ngap_pdu_c pdu;

  pdu.set_unsuccessful_outcome();
  pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& setup_fail = pdu.unsuccessful_outcome().value.ng_setup_fail();
  setup_fail->cause.value.set_radio_network();
  setup_fail->cause.value.radio_network() = asn1::ngap::cause_radio_network_opts::options::unspecified;
  setup_fail->time_to_wait_present        = true;
  setup_fail->time_to_wait.value          = asn1::ngap::time_to_wait_opts::v10s;
  // add critical diagnostics

  srsran::byte_buffer tx_pdu;
  asn1::bit_ref       bref(tx_pdu);
  ASSERT_EQ(pdu.pack(bref), SRSASN_SUCCESS);

  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);

  std::vector<uint8_t> tx_buffer{tx_pdu.begin(), tx_pdu.end()};
#if JSON_OUTPUT
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(tx_buffer.data(),
                   tx_buffer.size(),
                   "NGAP PDU unpacked ({} B): \n {}",
                   tx_buffer.size(),
                   json_writer1.to_string().c_str());
#endif
}
