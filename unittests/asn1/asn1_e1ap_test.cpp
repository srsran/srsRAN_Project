/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/pcap/e1ap_pcap.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace asn1;

#define JSON_OUTPUT 1

class asn1_e1ap_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ASN1").set_hex_dump_max_size(-1);

    test_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    srslog::init();

    pcap_writer.open("e1ap.pcap");

    // Start the log backend.
    srslog::init();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();

    pcap_writer.close();
  }

  srsgnb::e1ap_pcap     pcap_writer;
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

TEST_F(asn1_e1ap_test, when_gnb_cu_up_e1_setup_correct_then_packing_successful)
{
  auto& logger = srslog::fetch_basic_logger("ASN1", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

  asn1::e1ap::e1_ap_pdu_c pdu;
  pdu.set_init_msg();
  pdu.init_msg().value.set(asn1::e1ap::e1_ap_elem_procs_o::init_msg_c::types::gnb_cu_up_e1_setup_request);
  pdu.init_msg().proc_code = ASN1_E1AP_ID_G_NB_CU_UP_E1_SETUP;
  pdu.init_msg().crit      = crit_opts::reject;

  auto& setup_request             = pdu.init_msg().value.gnb_cu_up_e1_setup_request();
  setup_request->cn_support.value = asn1::e1ap::cn_support_opts::options::both;

  asn1::e1ap::supported_plmns_item_s plmn;
  plmn.plmn_id.from_string("214002");
  setup_request->supported_plmns->push_back(plmn);

  setup_request->gnb_cu_up_id.value     = 1;
  setup_request->gnb_cu_up_name_present = true;
  setup_request->gnb_cu_up_name.value.from_string("srs-cu-cp");

  srsgnb::byte_buffer buffer;
  asn1::bit_ref       bref(buffer);
  ASSERT_EQ(pdu.pack(bref), SRSASN_SUCCESS);

  // TODO: Accept byte buffer in pcap and log.
  std::vector<uint8_t> bytes{buffer.begin(), buffer.end()};
  pcap_writer.write_pdu(bytes);

  logger.info(bytes.data(), bytes.size(), "Packed PDU ({} bytes):", bref.distance_bytes());

#if JSON_OUTPUT
  int               unpacked_len = bref.distance_bytes();
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(
      bytes.data(), unpacked_len, "E1AP unpacked ({} B): \n {}", unpacked_len, json_writer1.to_string().c_str());
#endif
}

TEST_F(asn1_e1ap_test, when_bearer_context_setup_request_correct_then_unpacking_successful)
{
  uint8_t rx_msg[] = {0x00, 0x08, 0x00, 0x69, 0x00, 0x00, 0x07, 0x00, 0x02, 0x00, 0x02, 0x00, 0x09, 0x00, 0x0d, 0x00,
                      0x13, 0x00, 0x00, 0x10, 0xa6, 0xae, 0x39, 0xef, 0xbe, 0x0d, 0x42, 0x4c, 0xd8, 0x5f, 0x4a, 0x9c,
                      0x3a, 0xee, 0x04, 0x14, 0x00, 0x0e, 0x00, 0x05, 0x30, 0x3b, 0x9a, 0xca, 0x00, 0x00, 0x3a, 0x40,
                      0x03, 0x02, 0xf8, 0x99, 0x00, 0x17, 0x00, 0x01, 0x40, 0x00, 0x0f, 0x00, 0x2a, 0x40, 0x00, 0x01,
                      0x00, 0x2a, 0x00, 0x23, 0x00, 0x40, 0x01, 0x00, 0x02, 0x12, 0x30, 0x05, 0xf5, 0xe1, 0x00, 0x01,
                      0xf0, 0xac, 0x15, 0x04, 0x86, 0x00, 0x00, 0x00, 0x36, 0x00, 0x03, 0x01, 0x30, 0x0a, 0x20, 0xef,
                      0x00, 0x00, 0x00, 0x80, 0x00, 0x09, 0x7a, 0x00, 0x4d, 0x40, 0x02, 0x00, 0x00};
  srsgnb::byte_buffer rx_pdu{rx_msg};

  pcap_writer.write_pdu(rx_msg);

  asn1::cbit_ref          bref{rx_pdu};
  asn1::e1ap::e1_ap_pdu_c pdu;

  ASSERT_EQ(pdu.unpack(bref), SRSASN_SUCCESS);
  ASSERT_EQ(pdu.init_msg().proc_code, ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);
  ASSERT_EQ(pdu.init_msg().value.type(),
            asn1::e1ap::e1_ap_elem_procs_o::init_msg_c::types_opts::bearer_context_setup_request);
  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);

  // Get the message.
  const auto& msg = pdu.init_msg().value.bearer_context_setup_request();

  ASSERT_TRUE(msg->gnb_du_id_present);
  ASSERT_EQ(0, msg->gnb_du_id.value);

  // Check the SystemBearerContextSetupRequest.
  ASSERT_EQ(msg->sys_bearer_context_setup_request->type().value,
            asn1::e1ap::sys_bearer_context_setup_request_c::types_opts::options::ng_ran_bearer_context_setup_request);

  const auto& bearer_cont = msg->sys_bearer_context_setup_request->ng_ran_bearer_context_setup_request();
  ASSERT_EQ(bearer_cont.size(), 1);
  for (const auto& item : bearer_cont) {
    const auto& bearer = item.value();
    ASSERT_EQ(bearer.type(),
              asn1::e1ap::ng_ran_bearer_context_setup_request_o::value_c::types_opts::pdu_session_res_to_setup_list);
  }

  // Get session list.
  const auto& session_list = bearer_cont[0].value().pdu_session_res_to_setup_list();
  ASSERT_EQ(1, session_list.size());

  // Get first session item and check basic fields.
  const auto& session_item1 = session_list[0];
  ASSERT_EQ(session_item1.pdu_session_id, 1);
  ASSERT_EQ(session_item1.pdu_session_type, asn1::e1ap::pdu_session_type_opts::ipv4);

#if JSON_OUTPUT
  int               unpacked_len = bref.distance_bytes();
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(
      rx_msg, unpacked_len, "E1AP PDU unpacked ({} B): \n {}", unpacked_len, json_writer1.to_string().c_str());

  asn1::json_writer json_writer2;
  session_item1.to_json(json_writer2);
  test_logger.info("PDU session item unpacked: \n {}", json_writer2.to_string().c_str());
#endif
}

TEST_F(asn1_e1ap_test, when_bearer_context_setup_response_correct_then_unpacking_successful)
{
  uint8_t rx_msg[] = {0x20, 0x08, 0x00, 0x37, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x02, 0x00, 0x09, 0x00, 0x03,
                      0x00, 0x03, 0x40, 0x02, 0x80, 0x00, 0x10, 0x40, 0x23, 0x40, 0x00, 0x01, 0x00, 0x2e, 0x40,
                      0x1c, 0x00, 0x00, 0x01, 0x01, 0xf0, 0xac, 0x15, 0x06, 0x09, 0x00, 0x00, 0x02, 0x83, 0x00,
                      0x06, 0x00, 0x1f, 0xac, 0x15, 0x06, 0x09, 0x80, 0x00, 0x02, 0x83, 0x00, 0x00, 0x80};
  srsgnb::byte_buffer rx_pdu{rx_msg};

  pcap_writer.write_pdu(rx_msg);

  asn1::cbit_ref          bref{rx_pdu};
  asn1::e1ap::e1_ap_pdu_c pdu;

  ASSERT_EQ(pdu.unpack(bref), SRSASN_SUCCESS);
  ASSERT_EQ(pdu.init_msg().proc_code, ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);
  ASSERT_EQ(pdu.init_msg().value.type(),
            asn1::e1ap::e1_ap_elem_procs_o::successful_outcome_c::types_opts::bearer_context_setup_resp);
  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);

  // Get the message.
  const auto& msg = pdu.successful_outcome().value.bearer_context_setup_resp();

  // Check IDs.
  ASSERT_EQ(msg->gnb_cu_cp_ue_e1_ap_id.value, 9);
  ASSERT_EQ(msg->gnb_cu_up_ue_e1_ap_id.value, 640);

  // Check the SystemBearerContextSetupResponse.
  ASSERT_EQ(msg->sys_bearer_context_setup_resp->type().value,
            asn1::e1ap::sys_bearer_context_setup_resp_c::types_opts::options::ng_ran_bearer_context_setup_resp);

  const auto& bearer_cont = msg->sys_bearer_context_setup_resp->ng_ran_bearer_context_setup_resp();

  ASSERT_FALSE(bearer_cont.pdu_session_res_failed_list_present);
  ASSERT_EQ(bearer_cont.pdu_session_res_setup_list->size(), 1);

  // Get first resource setup list item.
  for (const auto& item : *bearer_cont.pdu_session_res_setup_list) {
    ASSERT_EQ(item.pdu_session_id, 1);
  }

  // TODO: unpack and check GTPu tunnel and DRB setup list

#if JSON_OUTPUT
  int               unpacked_len = bref.distance_bytes();
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(
      rx_msg, unpacked_len, "E1AP PDU unpacked ({} B): \n {}", unpacked_len, json_writer1.to_string().c_str());
#endif
}