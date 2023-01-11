/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/f1ap/cu_cp/f1ap_asn1_helpers.h"
#include <gtest/gtest.h>

using namespace asn1;
using namespace srsgnb;
using namespace srs_cu_cp;

#define JSON_OUTPUT 1

class f1ap_cu_msg_filler_test : public ::testing::Test
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

  void verify_f1ap_ue_context_modification(const asn1::f1ap::ue_context_mod_request_s& msg)
  {
    // set required fields to have full PDU
    asn1::f1ap::f1ap_pdu_c pdu;
    pdu.set_init_msg();
    pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
    pdu.init_msg().value.ue_context_mod_request()                           = msg;
    pdu.init_msg().value.ue_context_mod_request()->gnb_cu_ue_f1ap_id->value = 1;
    pdu.init_msg().value.ue_context_mod_request()->gnb_du_ue_f1ap_id->value = 2;

    // verify succesful packing
    srsgnb::byte_buffer tx_buffer;
    asn1::bit_ref       bref(tx_buffer);
    ASSERT_EQ(pdu.pack(bref), asn1::SRSASN_SUCCESS);

    std::vector<uint8_t> bytes{tx_buffer.begin(), tx_buffer.end()};
#if JSON_OUTPUT
    asn1::json_writer json_writer1;
    pdu.to_json(json_writer1);
    test_logger.info(
        bytes.data(), bytes.size(), "F1AP PDU unpacked ({} B): \n {}", bytes.size(), json_writer1.to_string().c_str());
#endif

    ASSERT_EQ(test_pack_unpack_consistency(pdu), asn1::SRSASN_SUCCESS);
  }

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

cu_cp_ue_context_modification_request generate_ue_context_mod_request()
{
  cu_cp_ue_context_modification_request msg           = {};
  cu_cp_drb_setup_message               drb_setup_msg = {};
  drb_setup_msg.drb_id                                = uint_to_drb_id(1);
  drb_setup_msg.rlc                                   = rlc_mode::am;

  up_transport_layer_info gtp_tunnel = {transport_layer_address{"127.0.0.1"}, int_to_gtp_teid(1)};
  drb_setup_msg.gtp_tunnels.push_back(gtp_tunnel);

  qos_flow_setup_request_item mapped_flow = {};
  mapped_flow.qos_flow_id                 = uint_to_qos_flow_id(1);
  drb_setup_msg.qos_flows_mapped_to_drb.emplace(mapped_flow.qos_flow_id, mapped_flow);

  msg.cu_cp_drb_setup_msgs.emplace(drb_setup_msg.drb_id, drb_setup_msg);

  return msg;
}

// Test correct filling and generation of F1AP ASN1 messages for UE context modficication request.
TEST_F(f1ap_cu_msg_filler_test, when_context_mod_req_valid_then_valid_asn1_msg_generated)
{
  cu_cp_ue_context_modification_request msg = generate_ue_context_mod_request();

  asn1::f1ap::ue_context_mod_request_s f1ap_ue_context_mod_request;
  fill_f1ap_ue_context_modification_request(f1ap_ue_context_mod_request, msg);

  // pack full F1AP PDU and verify correctnes
  verify_f1ap_ue_context_modification(f1ap_ue_context_mod_request);
}
