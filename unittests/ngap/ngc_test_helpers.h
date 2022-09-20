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

#include "srsgnb/asn1/asn1_utils.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/ngap/ngc_factory.h"
#include "test_helpers.h"
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

    msg_notifier = std::make_unique<dummy_ngc_amf_notifier>(nullptr);

    ngc = create_ngc(timers, *msg_notifier);
  }

  timer_manager                           timers;
  std::unique_ptr<dummy_ngc_amf_notifier> msg_notifier;
  std::unique_ptr<ngc_interface>          ngc;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

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

  asn1::ngap::supported_ta_item_s   supported_ta_item   = {};
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

} // namespace srs_cu_cp
} // namespace srsgnb
