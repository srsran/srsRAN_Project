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

#include "lib/pcap/dlt_pcap_impl.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include <gtest/gtest.h>

using namespace asn1;

#define JSON_OUTPUT 1

class asn1_e1ap_test : public ::testing::Test
{
protected:
  asn1_e1ap_test()
  {
    srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ASN1").set_hex_dump_max_size(-1);

    test_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    srslog::init();

    // Start the log backend.
    srslog::init();
  }

  ~asn1_e1ap_test()
  {
    // flush logger after each test
    srslog::flush();
  }

#if JSON_OUTPUT
  srsran::dlt_pcap_impl pcap_writer{srsran::PCAP_E1AP_DLT, "E1AP"};
#endif
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

TEST_F(asn1_e1ap_test, when_gnb_cu_up_e1_setup_correct_then_packing_successful)
{
  auto& logger = srslog::fetch_basic_logger("ASN1", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

#if JSON_OUTPUT
  pcap_writer.open("e1ap_e1_setup.pcap");
#endif

  asn1::e1ap::e1ap_pdu_c pdu;
  pdu.set_init_msg();
  pdu.init_msg().value.set(asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types::gnb_cu_up_e1_setup_request);
  pdu.init_msg().proc_code = ASN1_E1AP_ID_GNB_CU_UP_E1_SETUP;
  pdu.init_msg().crit      = crit_opts::reject;

  auto& setup_request             = pdu.init_msg().value.gnb_cu_up_e1_setup_request();
  setup_request->cn_support.value = asn1::e1ap::cn_support_opts::options::both;

  asn1::e1ap::supported_plmns_item_s plmn;
  plmn.plmn_id.from_string("214002");
  setup_request->supported_plmns.push_back(plmn);

  setup_request->gnb_cu_up_id           = 1;
  setup_request->gnb_cu_up_name_present = true;
  setup_request->gnb_cu_up_name.from_string("srs-cu-cp");

  srsran::byte_buffer buffer;
  asn1::bit_ref       bref(buffer);
  ASSERT_EQ(pdu.pack(bref), SRSASN_SUCCESS);

  // TODO: Accept byte buffer in pcap and log.
  std::vector<uint8_t> bytes{buffer.begin(), buffer.end()};
#if JSON_OUTPUT
  pcap_writer.push_pdu(bytes);
#endif

  logger.info(bytes.data(), bytes.size(), "Packed PDU ({} bytes):", bref.distance_bytes());

#if JSON_OUTPUT
  int               unpacked_len = bref.distance_bytes();
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(
      bytes.data(), unpacked_len, "E1AP unpacked ({} B): \n {}", unpacked_len, json_writer1.to_string().c_str());
#endif

#if JSON_OUTPUT
  pcap_writer.close();
#endif
}

TEST_F(asn1_e1ap_test, when_bearer_context_setup_request_correct_then_unpacking_successful)
{
#if JSON_OUTPUT
  pcap_writer.open("e1ap_e1_bearer_context_setup_request.pcap");
#endif

  uint8_t rx_msg[] = {0x00, 0x08, 0x00, 0x69, 0x00, 0x00, 0x07, 0x00, 0x02, 0x00, 0x02, 0x00, 0x09, 0x00, 0x0d, 0x00,
                      0x13, 0x00, 0x00, 0x10, 0xa6, 0xae, 0x39, 0xef, 0xbe, 0x0d, 0x42, 0x4c, 0xd8, 0x5f, 0x4a, 0x9c,
                      0x3a, 0xee, 0x04, 0x14, 0x00, 0x0e, 0x00, 0x05, 0x30, 0x3b, 0x9a, 0xca, 0x00, 0x00, 0x3a, 0x40,
                      0x03, 0x02, 0xf8, 0x99, 0x00, 0x17, 0x00, 0x01, 0x40, 0x00, 0x0f, 0x00, 0x2a, 0x40, 0x00, 0x01,
                      0x00, 0x2a, 0x00, 0x23, 0x00, 0x40, 0x01, 0x00, 0x02, 0x12, 0x30, 0x05, 0xf5, 0xe1, 0x00, 0x01,
                      0xf0, 0xac, 0x15, 0x04, 0x86, 0x00, 0x00, 0x00, 0x36, 0x00, 0x03, 0x01, 0x30, 0x0a, 0x20, 0xef,
                      0x00, 0x00, 0x00, 0x80, 0x00, 0x09, 0x7a, 0x00, 0x4d, 0x40, 0x02, 0x00, 0x00};
  srsran::byte_buffer rx_pdu{rx_msg};

#if JSON_OUTPUT
  pcap_writer.push_pdu(rx_msg);
#endif

  asn1::cbit_ref         bref{rx_pdu};
  asn1::e1ap::e1ap_pdu_c pdu;

  ASSERT_EQ(pdu.unpack(bref), SRSASN_SUCCESS);
  ASSERT_EQ(pdu.init_msg().proc_code, ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);
  ASSERT_EQ(pdu.init_msg().value.type(),
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_setup_request);
  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);

  // Get the message.
  const auto& msg = pdu.init_msg().value.bearer_context_setup_request();

  ASSERT_TRUE(msg->gnb_du_id_present);
  ASSERT_EQ(0, msg->gnb_du_id);

  // Check the SystemBearerContextSetupRequest.
  ASSERT_EQ(msg->sys_bearer_context_setup_request.type().value,
            asn1::e1ap::sys_bearer_context_setup_request_c::types_opts::options::ng_ran_bearer_context_setup_request);

  const auto& bearer_cont = msg->sys_bearer_context_setup_request.ng_ran_bearer_context_setup_request();
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

  pcap_writer.close();
#endif
}

TEST_F(asn1_e1ap_test, when_bearer_context_setup_response_correct_then_unpacking_successful)
{
#if JSON_OUTPUT
  pcap_writer.open("e1ap_e1_bearer_context_setup_response.pcap");
#endif

  uint8_t rx_msg[] = {0x20, 0x08, 0x00, 0x37, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x02, 0x00, 0x09, 0x00, 0x03,
                      0x00, 0x03, 0x40, 0x02, 0x80, 0x00, 0x10, 0x40, 0x23, 0x40, 0x00, 0x01, 0x00, 0x2e, 0x40,
                      0x1c, 0x00, 0x00, 0x01, 0x01, 0xf0, 0xac, 0x15, 0x06, 0x09, 0x00, 0x00, 0x02, 0x83, 0x00,
                      0x06, 0x00, 0x1f, 0xac, 0x15, 0x06, 0x09, 0x80, 0x00, 0x02, 0x83, 0x00, 0x00, 0x80};
  srsran::byte_buffer rx_pdu{rx_msg};

#if JSON_OUTPUT
  pcap_writer.push_pdu(rx_msg);
#endif

  asn1::cbit_ref         bref{rx_pdu};
  asn1::e1ap::e1ap_pdu_c pdu;

  ASSERT_EQ(pdu.unpack(bref), SRSASN_SUCCESS);
  ASSERT_EQ(pdu.init_msg().proc_code, ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);
  ASSERT_EQ(pdu.init_msg().value.type(),
            asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::bearer_context_setup_resp);
  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);

  // Get the message.
  const auto& msg = pdu.successful_outcome().value.bearer_context_setup_resp();

  // Check IDs.
  ASSERT_EQ(msg->gnb_cu_cp_ue_e1ap_id, 9);
  ASSERT_EQ(msg->gnb_cu_up_ue_e1ap_id, 640);

  // Check the SystemBearerContextSetupResponse.
  ASSERT_EQ(msg->sys_bearer_context_setup_resp.type().value,
            asn1::e1ap::sys_bearer_context_setup_resp_c::types_opts::options::ng_ran_bearer_context_setup_resp);

  const auto& bearer_cont = msg->sys_bearer_context_setup_resp.ng_ran_bearer_context_setup_resp();

  ASSERT_FALSE(bearer_cont.pdu_session_res_failed_list_present);
  ASSERT_EQ(bearer_cont.pdu_session_res_setup_list.size(), 1);

  // Get first resource setup list item.
  for (const auto& item : bearer_cont.pdu_session_res_setup_list) {
    ASSERT_EQ(item.pdu_session_id, 1);
  }

  // TODO: unpack and check GTPu tunnel and DRB setup list

#if JSON_OUTPUT
  int               unpacked_len = bref.distance_bytes();
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(
      rx_msg, unpacked_len, "E1AP PDU unpacked ({} B): \n {}", unpacked_len, json_writer1.to_string().c_str());

  pcap_writer.close();
#endif
}
