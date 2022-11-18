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

#include "test_helpers.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/e1_interface/common/e1_common.h"
#include "srsgnb/e1_interface/cu_cp/e1_cu_cp.h"
#include "srsgnb/e1_interface/cu_cp/e1_cu_cp_factory.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_cu_cp {

/// Fixture class for E1AP
class e1_cu_cp_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    msg_notifier             = std::make_unique<dummy_e1_pdu_notifier>(nullptr);
    cu_up_processor_notifier = std::make_unique<dummy_e1_cu_up_processor_notifier>();

    e1 = create_e1(timers, *msg_notifier, *cu_up_processor_notifier);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<e1_interface>                      e1;
  timer_manager                                      timers;
  std::unique_ptr<dummy_e1_pdu_notifier>             msg_notifier;
  std::unique_ptr<dummy_e1_cu_up_processor_notifier> cu_up_processor_notifier;
  srslog::basic_logger&                              test_logger = srslog::fetch_basic_logger("TEST");
};

cu_cp_e1_setup_request_message generate_cu_cp_e1_setup_request_message()
{
  cu_cp_e1_setup_request_message e1_setup_request  = {};
  e1_setup_request.request->gnb_cu_cp_name_present = true;
  e1_setup_request.request->gnb_cu_cp_name.value.from_string("srsCU-CP");

  return e1_setup_request;
}

asn1::e1ap::supported_plmns_item_s generate_supported_plmns_item(unsigned nrcell_id)
{
  asn1::e1ap::supported_plmns_item_s supported_plmns_item = {};
  supported_plmns_item.plmn_id.from_string("208991");

  asn1::e1ap::slice_support_item_s slice_support_item = {};
  slice_support_item.snssai.sst.from_number(1);
  supported_plmns_item.slice_support_list.push_back(slice_support_item);

  asn1::e1ap::nr_cgi_support_item_s nr_cgi_support_item;
  nr_cgi_support_item.nr_cgi.plmn_id.from_string("208991");
  nr_cgi_support_item.nr_cgi.nr_cell_id.from_number(nrcell_id);
  supported_plmns_item.nr_cgi_support_list.push_back(nr_cgi_support_item);

  supported_plmns_item.qo_s_params_support_list_present = false;

  return supported_plmns_item;
}

e1_message generate_cu_cp_e1_setup_respose(unsigned transaction_id)
{
  e1_message e1_setup_response = {};
  e1_setup_response.pdu.set_successful_outcome();
  e1_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_G_NB_CU_CP_E1_SETUP);

  auto& setup_resp                   = e1_setup_response.pdu.successful_outcome().value.gnb_cu_cp_e1_setup_resp();
  setup_resp->transaction_id.value   = transaction_id;
  setup_resp->gnb_cu_up_id.value     = 1;
  setup_resp->gnb_cu_up_name_present = true;
  setup_resp->gnb_cu_up_name.value.from_string("srsCU-UP");
  setup_resp->cn_support.value = asn1::e1ap::cn_support_opts::c_minus5gc;

  setup_resp->supported_plmns.id   = ASN1_E1AP_ID_SUPPORTED_PLMNS;
  setup_resp->supported_plmns.crit = asn1::crit_opts::reject;

  setup_resp->supported_plmns.value.push_back(generate_supported_plmns_item(12345678));

  setup_resp->gnb_cu_up_capacity_present = false;

  return e1_setup_response;
}

e1_message generate_cu_cp_e1_setup_failure_message(unsigned transaction_id)
{
  e1_message e1_setup_failure = {};

  e1_setup_failure.pdu.set_unsuccessful_outcome();
  e1_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_G_NB_CU_CP_E1_SETUP);

  auto& setup_fail                 = e1_setup_failure.pdu.unsuccessful_outcome().value.gnb_cu_cp_e1_setup_fail();
  setup_fail->transaction_id.value = transaction_id;
  setup_fail->cause.value.set_radio_network();
  setup_fail->cause.value.radio_network() = asn1::e1ap::cause_radio_network_opts::options::unspecified;
  setup_fail->time_to_wait_present        = false;
  setup_fail->crit_diagnostics_present    = false;
  // add critical diagnostics

  return e1_setup_failure;
}

e1_message generate_cu_cp_e1_setup_failure_message_with_time_to_wait(unsigned                   transaction_id,
                                                                     asn1::e1ap::time_to_wait_e time_to_wait)
{
  e1_message e1_setup_failure = generate_cu_cp_e1_setup_failure_message(transaction_id);

  auto& setup_fail                 = e1_setup_failure.pdu.unsuccessful_outcome().value.gnb_cu_cp_e1_setup_fail();
  setup_fail->time_to_wait_present = true;
  setup_fail->time_to_wait.value   = time_to_wait;

  return e1_setup_failure;
}

e1_message generate_cu_up_e1_setup_request_base()
{
  e1_message e1_setup_request_base = {};

  e1_setup_request_base.pdu.set_init_msg();
  e1_setup_request_base.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_G_NB_CU_UP_E1_SETUP);

  auto& setup_req                   = e1_setup_request_base.pdu.init_msg().value.gnb_cu_up_e1_setup_request();
  setup_req->transaction_id.value   = 99;
  setup_req->gnb_cu_up_id.value     = 1;
  setup_req->gnb_cu_up_name_present = true;
  setup_req->gnb_cu_up_name.value.from_string("srsCU-UP");
  setup_req->cn_support.value = asn1::e1ap::cn_support_opts::c_minus5gc;

  return e1_setup_request_base;
}

e1_message generate_valid_cu_up_e1_setup_request()
{
  e1_message e1_setup_request     = generate_cu_up_e1_setup_request_base();
  auto&      setup_req            = e1_setup_request.pdu.init_msg().value.gnb_cu_up_e1_setup_request();
  setup_req->supported_plmns.id   = ASN1_E1AP_ID_SUPPORTED_PLMNS;
  setup_req->supported_plmns.crit = asn1::crit_opts::reject;

  setup_req->supported_plmns.value.push_back(generate_supported_plmns_item(12345678));

  return e1_setup_request;
}

e1_message generate_cu_up_e1_setup_respose(unsigned transaction_id)
{
  e1_message e1_setup_response = {};
  e1_setup_response.pdu.set_successful_outcome();
  e1_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_G_NB_CU_CP_E1_SETUP);

  auto& setup_resp                   = e1_setup_response.pdu.successful_outcome().value.gnb_cu_up_e1_setup_resp();
  setup_resp->transaction_id.value   = transaction_id;
  setup_resp->gnb_cu_cp_name_present = true;
  setup_resp->gnb_cu_cp_name.value.from_string("srsCU-CP");

  return e1_setup_response;
}

e1_message generate_bearer_context_setup_response_msg(unsigned int cu_cp_ue_e1_id, unsigned int cu_up_ue_e1_id)
{
  e1_message bearer_context_setup_response = {};

  bearer_context_setup_response.pdu.set_successful_outcome();
  bearer_context_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);

  auto& bearer_context_setup_resp =
      bearer_context_setup_response.pdu.successful_outcome().value.bearer_context_setup_resp();
  bearer_context_setup_resp->gnb_cu_cp_ue_e1_ap_id.value        = cu_cp_ue_e1_id;
  bearer_context_setup_resp->gnb_cu_up_ue_e1_ap_id.value        = cu_up_ue_e1_id;
  bearer_context_setup_resp->sys_bearer_context_setup_resp.id   = ASN1_E1AP_ID_SYS_BEARER_CONTEXT_SETUP_RESP;
  bearer_context_setup_resp->sys_bearer_context_setup_resp.crit = asn1::crit_opts::ignore;
  bearer_context_setup_resp->sys_bearer_context_setup_resp.value.set_ng_ran_bearer_context_setup_resp();

  auto& ng_ran_bearer_context_setup_resp =
      bearer_context_setup_resp->sys_bearer_context_setup_resp.value.ng_ran_bearer_context_setup_resp();

  asn1::e1ap::pdu_session_res_setup_item_s pdu_session_res_setup_item = {};
  pdu_session_res_setup_item.pdu_session_id                           = 1;

  pdu_session_res_setup_item.ng_dl_up_tnl_info.set_gtp_tunnel();
  auto& gtp_tunnel = pdu_session_res_setup_item.ng_dl_up_tnl_info.gtp_tunnel();
  gtp_tunnel.transport_layer_address.from_number(2887058953);
  gtp_tunnel.gtp_teid.from_string("00000283");

  asn1::e1ap::drb_setup_item_ng_ran_s drb_setup_item_ng_ran = {};
  drb_setup_item_ng_ran.drb_id                              = 4;

  asn1::e1ap::up_params_item_s up_params_item = {};
  up_params_item.up_tnl_info.set_gtp_tunnel();
  auto& gtp_tunnel2 = up_params_item.up_tnl_info.gtp_tunnel();
  gtp_tunnel2.transport_layer_address.from_number(2887058953);
  gtp_tunnel2.gtp_teid.from_string("80000283");
  up_params_item.cell_group_id = 0;
  drb_setup_item_ng_ran.ul_up_transport_params.push_back(up_params_item);

  asn1::e1ap::qo_s_flow_item_s qo_s_flow_item = {};
  qo_s_flow_item.qo_s_flow_id                 = 1;
  drb_setup_item_ng_ran.flow_setup_list.push_back(qo_s_flow_item);

  pdu_session_res_setup_item.drb_setup_list_ng_ran.push_back(drb_setup_item_ng_ran);

  ng_ran_bearer_context_setup_resp.pdu_session_res_setup_list.value.push_back(pdu_session_res_setup_item);

  ng_ran_bearer_context_setup_resp.pdu_session_res_failed_list_present = false;

  return bearer_context_setup_response;
}

e1_message generate_bearer_context_setup_failure_msg(unsigned int cu_cp_ue_e1_id, unsigned int cu_up_ue_e1_id)
{
  e1_message bearer_context_setup_failure = {};

  bearer_context_setup_failure.pdu.set_unsuccessful_outcome();
  bearer_context_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);

  auto& bearer_context_setup_fail =
      bearer_context_setup_failure.pdu.unsuccessful_outcome().value.bearer_context_setup_fail();
  bearer_context_setup_fail->gnb_cu_cp_ue_e1_ap_id.value = cu_cp_ue_e1_id;
  bearer_context_setup_fail->gnb_cu_up_ue_e1_ap_id.value = cu_up_ue_e1_id;
  bearer_context_setup_fail->cause.value.set_radio_network();
  bearer_context_setup_fail->cause.value.radio_network() = asn1::e1ap::cause_radio_network_opts::options::unspecified;

  return bearer_context_setup_failure;
}

e1_message generate_bearer_context_modification_response_msg(unsigned int cu_cp_ue_e1_id, unsigned int cu_up_ue_e1_id)
{
  e1_message bearer_context_modification_response = {};

  bearer_context_modification_response.pdu.set_successful_outcome();
  bearer_context_modification_response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);

  auto& bearer_context_mod_resp =
      bearer_context_modification_response.pdu.successful_outcome().value.bearer_context_mod_resp();
  bearer_context_mod_resp->gnb_cu_cp_ue_e1_ap_id.value         = cu_cp_ue_e1_id;
  bearer_context_mod_resp->gnb_cu_up_ue_e1_ap_id.value         = cu_up_ue_e1_id;
  bearer_context_mod_resp->sys_bearer_context_mod_resp_present = true;
  bearer_context_mod_resp->sys_bearer_context_mod_resp.value.set_ng_ran_bearer_context_mod_resp();

  auto& ng_ran_bearer_context_mod_resp =
      bearer_context_mod_resp->sys_bearer_context_mod_resp.value.ng_ran_bearer_context_mod_resp();
  ng_ran_bearer_context_mod_resp.pdu_session_res_modified_list_present = true;

  asn1::e1ap::pdu_session_res_modified_item_s pdu_session_res_modified_item = {};
  asn1::e1ap::drb_modified_item_ng_ran_s      drb_modified_item_ng_ran;
  drb_modified_item_ng_ran.drb_id             = 4;
  asn1::e1ap::up_params_item_s up_params_item = {};
  up_params_item.up_tnl_info.set_gtp_tunnel();
  auto& gtp_tunnel = up_params_item.up_tnl_info.gtp_tunnel();
  gtp_tunnel.transport_layer_address.from_number(2887058953);
  gtp_tunnel.gtp_teid.from_string("80000283");
  up_params_item.cell_group_id = 0;
  drb_modified_item_ng_ran.ul_up_transport_params.push_back(up_params_item);
  pdu_session_res_modified_item.drb_modified_list_ng_ran.push_back(drb_modified_item_ng_ran);
  ng_ran_bearer_context_mod_resp.pdu_session_res_modified_list.value.push_back(pdu_session_res_modified_item);

  return bearer_context_modification_response;
}

e1_message generate_bearer_context_modification_failure_msg(unsigned int cu_cp_ue_e1_id, unsigned int cu_up_ue_e1_id)
{
  e1_message bearer_context_modification_failure = {};

  bearer_context_modification_failure.pdu.set_unsuccessful_outcome();
  bearer_context_modification_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);

  auto& bearer_context_mod_fail =
      bearer_context_modification_failure.pdu.unsuccessful_outcome().value.bearer_context_mod_fail();
  bearer_context_mod_fail->gnb_cu_cp_ue_e1_ap_id.value = cu_cp_ue_e1_id;
  bearer_context_mod_fail->gnb_cu_up_ue_e1_ap_id.value = cu_up_ue_e1_id;
  bearer_context_mod_fail->cause.value.set_radio_network();
  bearer_context_mod_fail->cause.value.radio_network() = asn1::e1ap::cause_radio_network_opts::options::unspecified;

  return bearer_context_modification_failure;
}

e1_message generate_bearer_context_release_complete_msg(unsigned int cu_cp_ue_e1_id, unsigned int cu_up_ue_e1_id)
{
  e1_message bearer_ctxt_rel_complete_msg = {};
  bearer_ctxt_rel_complete_msg.pdu.set_successful_outcome();
  bearer_ctxt_rel_complete_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE);
  auto& rel_complete_msg =
      bearer_ctxt_rel_complete_msg.pdu.successful_outcome().value.bearer_context_release_complete();
  rel_complete_msg->gnb_cu_cp_ue_e1_ap_id.value = cu_cp_ue_e1_id;
  rel_complete_msg->gnb_cu_up_ue_e1_ap_id.value = cu_up_ue_e1_id;

  return bearer_ctxt_rel_complete_msg;
}

} // namespace srs_cu_cp
} // namespace srsgnb
