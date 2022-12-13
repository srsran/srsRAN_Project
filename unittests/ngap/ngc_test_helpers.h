/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/cu_cp/ue_manager.h"
#include "test_helpers.h"
#include "srsgnb/asn1/asn1_utils.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/ngap/ngc_factory.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_cu_cp {

/// Fixture class for NGC
class ngc_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    msg_notifier          = std::make_unique<dummy_ngc_amf_notifier>(nullptr);
    rrc_ue_notifier       = std::make_unique<dummy_ngc_rrc_ue_notifier>();
    ngc_ue_task_scheduler = std::make_unique<dummy_ngc_ue_task_scheduler>(timers);
    e1_control_notifier   = std::make_unique<dummy_ngc_e1_control_notifier>();
    f1c_control_notifier  = std::make_unique<dummy_ngc_f1c_control_notifier>();

    ngc = create_ngc(*ngc_ue_task_scheduler, ue_mng, *msg_notifier, *e1_control_notifier, *f1c_control_notifier);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  timer_manager                                   timers;
  ue_manager                                      ue_mng;
  std::unique_ptr<dummy_ngc_amf_notifier>         msg_notifier;
  std::unique_ptr<dummy_ngc_rrc_ue_notifier>      rrc_ue_notifier;
  std::unique_ptr<dummy_ngc_ue_task_scheduler>    ngc_ue_task_scheduler;
  std::unique_ptr<dummy_ngc_e1_control_notifier>  e1_control_notifier;
  std::unique_ptr<dummy_ngc_f1c_control_notifier> f1c_control_notifier;
  std::unique_ptr<ngc_interface>                  ngc;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

// NG Application Protocol (NGSetupRequest)
//     NGAP-PDU: initiatingMessage (0)
//         initiatingMessage
//             procedureCode: id-NGSetup (21)
//             criticality: reject (0)
//             value
//                 NGSetupRequest
//                     protocolIEs: 4 items
//                         Item 0: id-GlobalRANNodeID
//                             ProtocolIE-Field
//                                 id: id-GlobalRANNodeID (27)
//                                 criticality: reject (0)
//                                 value
//                                     GlobalRANNodeID: globalGNB-ID (0)
//                                         globalGNB-ID
//                                             pLMNIdentity: 00f110
//                                                 Mobile Country Code (MCC): Unknown (1)
//                                                 Mobile Network Code (MNC): Unknown (01)
//                                             gNB-ID: gNB-ID (0)
//                                                 gNB-ID: 00066c [bit length 22, 2 LSB pad bits, 0000 0000  0000 0110
//                                                 0110 11.. decimal value 411]
//                         Item 1: id-RANNodeName
//                             ProtocolIE-Field
//                                 id: id-RANNodeName (82)
//                                 criticality: ignore (1)
//                                 value
//                                     RANNodeName: srsgnb01
//                         Item 2: id-SupportedTAList
//                             ProtocolIE-Field
//                                 id: id-SupportedTAList (102)
//                                 criticality: reject (0)
//                                 value
//                                     SupportedTAList: 1 item
//                                         Item 0
//                                             SupportedTAItem
//                                                 tAC: 7 (0x000007)
//                                                 broadcastPLMNList: 1 item
//                                                     Item 0
//                                                         BroadcastPLMNItem
//                                                             pLMNIdentity: 00f110
//                                                                 Mobile Country Code (MCC): Unknown (1)
//                                                                 Mobile Network Code (MNC): Unknown (01)
//                                                             tAISliceSupportList: 1 item
//                                                                 Item 0
//                                                                     SliceSupportItem
//                                                                         s-NSSAI
//                                                                             sST: 01
//                         Item 3: id-DefaultPagingDRX
//                             ProtocolIE-Field
//                                 id: id-DefaultPagingDRX (21)
//                                 criticality: ignore (1)
//                                 value
//                                     PagingDRX: v256 (3)
const static uint8_t ng_setup_request_packed[] = {
    0x00, 0x15, 0x00, 0x33, 0x00, 0x00, 0x04, 0x00, 0x1b, 0x00, 0x08, 0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x06, 0x6c,
    0x00, 0x52, 0x40, 0x0a, 0x03, 0x80, 0x73, 0x72, 0x73, 0x67, 0x6e, 0x62, 0x30, 0x31, 0x00, 0x66, 0x00, 0x0d, 0x00,
    0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x08, 0x00, 0x15, 0x40, 0x01, 0x60};

ng_setup_request_message generate_ng_setup_request_message()
{
  ng_setup_request_message request_msg = {};
  request_msg.msg                      = {};
  request_msg.msg->global_ran_node_id.value.set_global_gnb_id();
  request_msg.msg->global_ran_node_id.value.global_gnb_id().gnb_id.set_gnb_id();
  request_msg.msg->global_ran_node_id.value.global_gnb_id().gnb_id.gnb_id().from_number(411);
  request_msg.msg->global_ran_node_id.value.global_gnb_id().plmn_id.from_string("00f110");

  request_msg.msg->ran_node_name_present = true;
  request_msg.msg->ran_node_name.value.from_string("srsgnb01");

  request_msg.msg->supported_ta_list.id   = ASN1_NGAP_ID_SUPPORTED_TA_LIST;
  request_msg.msg->supported_ta_list.crit = asn1::crit_opts::reject;

  asn1::ngap::supported_ta_item_s supported_ta_item = {};
  supported_ta_item.tac.from_number(7);
  asn1::ngap::broadcast_plmn_item_s broadcast_plmn_item = {};
  broadcast_plmn_item.plmn_id.from_string("00f110");
  asn1::ngap::slice_support_item_s slice_support_item = {};
  slice_support_item.s_nssai.sst.from_number(1);
  broadcast_plmn_item.tai_slice_support_list.push_back(slice_support_item);
  supported_ta_item.broadcast_plmn_list.push_back(broadcast_plmn_item);

  request_msg.msg->supported_ta_list.value.push_back(supported_ta_item);

  request_msg.msg->default_paging_drx.value.value = asn1::ngap::paging_drx_opts::v256;

  return request_msg;
}

ngc_message generate_ng_setup_response_message()
{
  ngc_message ng_setup_response = {};

  ng_setup_response.pdu.set_successful_outcome();
  ng_setup_response.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& setup_res = ng_setup_response.pdu.successful_outcome().value.ng_setup_resp();
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

  return ng_setup_response;
}

ngc_message generate_ng_setup_failure_message()
{
  ngc_message ng_setup_failure = {};

  ng_setup_failure.pdu.set_unsuccessful_outcome();
  ng_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& setup_fail = ng_setup_failure.pdu.unsuccessful_outcome().value.ng_setup_fail();
  setup_fail->cause.value.set_radio_network();
  setup_fail->cause.value.radio_network() = asn1::ngap::cause_radio_network_opts::options::unspecified;
  setup_fail->time_to_wait_present        = false;
  setup_fail->crit_diagnostics_present    = false;
  // add critical diagnostics

  return ng_setup_failure;
}

ngc_message generate_ng_setup_failure_message_with_time_to_wait(asn1::ngap::time_to_wait_e time_to_wait)
{
  ngc_message ng_setup_failure = generate_ng_setup_failure_message();

  auto& setup_fail                 = ng_setup_failure.pdu.unsuccessful_outcome().value.ng_setup_fail();
  setup_fail->time_to_wait_present = true;
  setup_fail->time_to_wait.value   = time_to_wait;

  return ng_setup_failure;
}

const uint32_t          nas_pdu_len = 4; // Dummy length used for testing (content is not important)
ngap_initial_ue_message generate_initial_ue_message()
{
  ngap_initial_ue_message msg;
  msg.cu_cp_ue_id = cu_cp_ue_id_t::min;
  msg.nas_pdu.resize(nas_pdu_len);
  msg.establishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_sig;
  // msg.nr_cgi = {};
  msg.tac = 7;
  return msg;
}

ngc_message generate_downlink_nas_transport_message()
{
  ngc_message dl_nas_transport = {};

  dl_nas_transport.pdu.set_init_msg();
  dl_nas_transport.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_DL_NAS_TRANSPORT);

  auto& dl_nas_transport_msg                 = dl_nas_transport.pdu.init_msg().value.dl_nas_transport();
  dl_nas_transport_msg->amf_ue_ngap_id.value = 3;
  dl_nas_transport_msg->ran_ue_ngap_id.value = cu_cp_ue_id_to_uint(cu_cp_ue_id_t::min);
  dl_nas_transport_msg->nas_pdu.value.resize(nas_pdu_len);

  return dl_nas_transport;
}

ngap_ul_nas_transport_message generate_ul_nas_transport_message()
{
  ngap_ul_nas_transport_message ul_nas_transport;
  ul_nas_transport.cu_cp_ue_id = uint_to_cu_cp_ue_id(0);
  ul_nas_transport.nas_pdu.resize(nas_pdu_len);
  // ul_nas_transport.nr_cgi = {};

  return ul_nas_transport;
}

ngc_message generate_initial_context_setup_request_base()
{
  ngc_message ngc_msg = {};

  ngc_msg.pdu.set_init_msg();
  ngc_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);

  auto& init_context_setup_req                 = ngc_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->amf_ue_ngap_id.value = 3;
  init_context_setup_req->ran_ue_ngap_id.value = cu_cp_ue_id_to_uint(cu_cp_ue_id_t::min);

  init_context_setup_req->guami->plmn_id.from_string("02f899");
  init_context_setup_req->guami->amf_region_id.from_number(128);
  init_context_setup_req->guami->amf_set_id.from_number(1);
  init_context_setup_req->guami->amf_pointer.from_number(1);

  init_context_setup_req->nas_pdu_present = true;
  init_context_setup_req->nas_pdu.value.from_string(
      "7e02c4f6c22f017e0042010177000bf202f8998000410000001054070002f8990000011500210201005e01b6");

  init_context_setup_req->allowed_nssai->resize(1);

  asn1::ngap::allowed_nssai_item_s allowed_nssai;
  allowed_nssai.s_nssai.sst.from_number(1);
  allowed_nssai.s_nssai.sd_present = true;
  allowed_nssai.s_nssai.sd.from_string("db2700");

  init_context_setup_req->allowed_nssai->push_back(allowed_nssai);

  return ngc_msg;
}

ngc_message generate_valid_initial_context_setup_request_message()
{
  ngc_message ngc_msg = generate_initial_context_setup_request_base();

  auto& init_context_setup_req = ngc_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->ue_security_cap->nrencryption_algorithms.from_number(57344);
  init_context_setup_req->ue_security_cap->nrintegrity_protection_algorithms.from_number(57344);
  init_context_setup_req->ue_security_cap->eutr_aencryption_algorithms.from_number(57344);
  init_context_setup_req->ue_security_cap->eutr_aintegrity_protection_algorithms.from_number(57344);

  init_context_setup_req->security_key.value.from_string(
      "6d008a88bca9bb81190ff8c552cf8680069821e3b50f5f29c45d2632323edb2d");

  return ngc_msg;
}

ngc_message generate_invalid_initial_context_setup_request_message()
{
  ngc_message ngc_msg = generate_initial_context_setup_request_base();

  // NIA0 is not allowed
  auto& init_context_setup_req = ngc_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->ue_security_cap->nrencryption_algorithms.from_number(0);
  init_context_setup_req->ue_security_cap->nrintegrity_protection_algorithms.from_number(0);
  init_context_setup_req->ue_security_cap->eutr_aencryption_algorithms.from_number(0);
  init_context_setup_req->ue_security_cap->eutr_aintegrity_protection_algorithms.from_number(0);

  init_context_setup_req->security_key.value.from_string(
      "6d008a88bca9bb81190ff8c552cf8680069821e3b50f5f29c45d2632323edb2d");

  return ngc_msg;
}

} // namespace srs_cu_cp
} // namespace srsgnb
