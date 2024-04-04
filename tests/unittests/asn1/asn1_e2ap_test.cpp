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

#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/pcap/dlt_pcap.h"
#include <gtest/gtest.h>

using namespace asn1;
using namespace srsran;

#define JSON_OUTPUT 0

class asn1_e2ap_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ASN1").set_hex_dump_max_size(-1);

    test_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    // Start the log backend.
    srslog::init();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

#if JSON_OUTPUT
  task_worker               worker{"pcap_worker", 16};
  task_worker_executor      exec{worker};
  std::unique_ptr<dlt_pcap> pcap_writer = create_e2ap_pcap("/tmp/e2ap.pcap", exec);
#else
  std::unique_ptr<dlt_pcap> pcap_writer = create_null_dlt_pcap();
#endif
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

TEST_F(asn1_e2ap_test, when_e2_setup_correct_then_packing_successful)
{
  auto& logger = srslog::fetch_basic_logger("ASN1", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

  asn1::e2ap::e2ap_pdu_c pdu;
  pdu.set_init_msg();
  pdu.init_msg().value.set(asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types::e2setup_request);
  pdu.init_msg().proc_code = ASN1_E2AP_ID_E2SETUP;
  pdu.init_msg().crit      = crit_opts::reject;

  asn1::e2ap::e2setup_request_s& setup_request = pdu.init_msg().value.e2setup_request();
  setup_request->transaction_id                = 1;

  auto& gnb_id = setup_request->global_e2node_id.set_gnb();
  gnb_id.global_gnb_id.plmn_id.from_number(3617847);
  gnb_id.global_gnb_id.gnb_id.gnb_id().from_number(381210353);

  // RAN functions added.
  setup_request->ran_functions_added.resize(1);
  auto& ran_func_item = setup_request->ran_functions_added[0];
  ran_func_item.load_info_obj(ASN1_E2AP_ID_RAN_FUNCTION_ITEM);
  ran_func_item.value().ran_function_item().ran_function_id = 0;
  ran_func_item.value().ran_function_item().ran_function_definition.from_string(
      "20C04F52414E2D4532534D2D4B504D0000054F494431323305004B504D206D6F6E69746F720860283861AAE33F0060000101070050657269"
      "6F646963207265706F727401051401011D004F2D4455204D6561737572656D656E7420436F6E7461696E657220666F722074686520354743"
      "20636F6E6E6563746564206465706C6F796D656E74010101010001021D004F2D4455204D6561737572656D656E7420436F6E7461696E6572"
      "20666F72207468652045504320636F6E6E6563746564206465706C6F796D656E74010101010001031E804F2D43552D4350204D6561737572"
      "656D656E7420436F6E7461696E657220666F72207468652035474320636F6E6E6563746564206465706C6F796D656E74010101010001041E"
      "804F2D43552D4350204D6561737572656D656E7420436F6E7461696E657220666F72207468652045504320636F6E6E656374656420646570"
      "6C6F796D656E74010101010001051E804F2D43552D5550204D6561737572656D656E7420436F6E7461696E657220666F7220746865203547"
      "4320636F6E6E6563746564206465706C6F796D656E74010101010001061E804F2D43552D5550204D6561737572656D656E7420436F6E7461"
      "696E657220666F72207468652045504320636F6E6E6563746564206465706C6F796D656E7401010101");
  ran_func_item.value().ran_function_item().ran_function_o_id.resize(1);

  // E2 node component config.
  auto& list = setup_request->e2node_component_cfg_addition;
  list.resize(1);
  list[0].load_info_obj(ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ITEM);
  auto& e2node_cfg_item                           = list[0].value().e2node_component_cfg_addition_item();
  e2node_cfg_item.e2node_component_interface_type = asn1::e2ap::e2node_component_interface_type_opts::ng;
  e2node_cfg_item.e2node_component_id.set_e2node_component_interface_type_ng().amf_name.from_string("nginterf");
  e2node_cfg_item.e2node_component_cfg.e2node_component_request_part.from_string("72657170617274");
  e2node_cfg_item.e2node_component_cfg.e2node_component_resp_part.from_string("72657370617274");

  srsran::byte_buffer buffer;
  asn1::bit_ref       bref(buffer);
  ASSERT_EQ(pdu.pack(bref), SRSASN_SUCCESS);

  // TODO: Accept byte buffer in pcap and log.
  std::vector<uint8_t> bytes{buffer.begin(), buffer.end()};
  pcap_writer->push_pdu(bytes);

  logger.info(bytes.data(), bytes.size(), "Packed PDU ({} bytes):", bref.distance_bytes());

#if JSON_OUTPUT
  int               unpacked_len = bref.distance_bytes();
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(
      bytes.data(), unpacked_len, "E1AP unpacked ({} B): \n {}", unpacked_len, json_writer1.to_string().c_str());
#endif
}

TEST_F(asn1_e2ap_test, ric_control_request)
{
  uint8_t e2ap_msg[] = {0x00, 0x04, 0x00, 0x76, 0x00, 0x00, 0x05, 0x00, 0x1d, 0x00, 0x05, 0x00, 0x03, 0xfd, 0x00, 0x00,
                        0x00, 0x05, 0x00, 0x02, 0x00, 0x03, 0x00, 0x16, 0x00, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x05, 0x00, 0x17, 0x00, 0x46, 0x45,
                        0x00, 0x00, 0x01, 0x00, 0x00, 0x60, 0x00, 0x00, 0x40, 0x00, 0x03, 0x00, 0x02, 0x44, 0x00, 0x00,
                        0x00, 0x04, 0x60, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x06, 0x2a, 0x00, 0x05, 0x30, 0x30, 0x31,
                        0x30, 0x31, 0x00, 0x07, 0x44, 0x00, 0x01, 0x00, 0x08, 0x2a, 0x00, 0x01, 0x31, 0x00, 0x09, 0x2a,
                        0x00, 0x01, 0x30, 0x00, 0x0a, 0x28, 0x80, 0x01, 0x05, 0x00, 0x0b, 0x28, 0x80, 0x01, 0x19, 0x00,
                        0x0c, 0x28, 0x80, 0x01, 0x64, 0x00, 0x15, 0x00, 0x01, 0x40};
  // 00040076000005001d00050003fd00000005000200030016001110000000000000f1100000000102000005001700464500000100006000004000030002440000000460000040000100062a00053030313031000744000100082a00013100092a000130000a28800105000b28800119000c288001640015000140
  byte_buffer buffer = byte_buffer::create(e2ap_msg).value();

  asn1::cbit_ref         bref{buffer};
  asn1::e2ap::e2ap_pdu_c pdu;
  ASSERT_EQ(pdu.unpack(bref), SRSASN_SUCCESS);
  ASSERT_EQ(pdu.type(), asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(pdu.init_msg().value.type().value, asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::ric_ctrl_request);
  asn1::e2ap::ric_ctrl_request_s& ric_ctrl = pdu.init_msg().value.ric_ctrl_request();
  ASSERT_EQ(ric_ctrl->ran_function_id, 3);

  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);
}

TEST_F(asn1_e2ap_test, ric_control_failure_message)
{
  uint8_t e2ap_msg[] = {0x40, 0x04, 0x00, 0x17, 0x00, 0x00, 0x03, 0x00, 0x1d, 0x00, 0x05, 0x00, 0x03, 0xfd,
                        0x00, 0x00, 0x00, 0x05, 0x00, 0x02, 0x00, 0x03, 0x00, 0x01, 0x40, 0x01, 0x56};
  // 40040017000003001d00050003fd00000005000200030001400156
  byte_buffer buffer = byte_buffer::create(e2ap_msg).value();

  asn1::cbit_ref         bref{buffer};
  asn1::e2ap::e2ap_pdu_c pdu;
  ASSERT_EQ(pdu.unpack(bref), SRSASN_SUCCESS);
  ASSERT_EQ(pdu.type(), asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);

  ASSERT_EQ(test_pack_unpack_consistency(pdu), SRSASN_SUCCESS);
}