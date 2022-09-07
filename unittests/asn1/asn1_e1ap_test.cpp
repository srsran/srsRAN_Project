/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/asn1/e1ap.h"
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

TEST_F(asn1_e1ap_test, when_gnb_du_up_e1_setup_correct_then_packing_successful)
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