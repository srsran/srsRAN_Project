/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "e2ap_asn1_packer.h"
#include "lib/e2/common/e2_impl.h"
#include "lib/e2/common/e2_subscription_manager_impl.h"
#include "lib/e2/e2sm/e2sm_kpm/e2sm_kpm_asn1_packer.h"
#include "lib/e2/e2sm/e2sm_kpm/e2sm_kpm_impl.h"
#include "lib/e2/e2sm/e2sm_rc/e2sm_rc_asn1_packer.h"
#include "lib/e2/e2sm/e2sm_rc/e2sm_rc_control_action_cu_executor.h"
#include "lib/e2/e2sm/e2sm_rc/e2sm_rc_control_action_du_executor.h"
#include "lib/e2/e2sm/e2sm_rc/e2sm_rc_control_service_impl.h"
#include "lib/e2/e2sm/e2sm_rc/e2sm_rc_impl.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2sm/e2sm_rc_ies.h"
#include "srsran/cu_cp/cu_configurator.h"
#include "srsran/cu_cp/mobility_manager_config.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_cu.h"
#include "srsran/e2/e2_du_factory.h"
#include "srsran/e2/e2ap_configuration_helpers.h"
#include "srsran/e2/e2sm/e2sm.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/e2/gateways/e2_connection_client.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>

namespace srsran {

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.

class dummy_e2_tx_pdu_notifier : public e2_message_notifier
{
public:
  dummy_e2_tx_pdu_notifier(e2_message& last_tx_pdu_, unique_task on_disconnect_) :
    last_tx_pdu(last_tx_pdu_), on_disconnect(std::move(on_disconnect_))
  {
  }
  ~dummy_e2_tx_pdu_notifier() override { on_disconnect(); }

  void on_new_message(const e2_message& msg) override { last_tx_pdu = msg; }

  e2_message& last_tx_pdu;
  unique_task on_disconnect;
};

class dummy_e2_pdu_notifier : public e2_message_notifier
{
public:
  dummy_e2_pdu_notifier(e2_message_handler* handler_) : logger(srslog::fetch_basic_logger("TEST")), handler(handler_) {}

  void attach_handler(e2_message_handler* handler_) { handler = handler_; }
  void on_new_message(const e2_message& msg) override
  {
    switch (msg.pdu.type().value) {
      case asn1::e2ap::e2ap_pdu_c::types_opts::init_msg:
        logger.info("Received a E2AP PDU of type {}.{}",
                    msg.pdu.type().to_string(),
                    msg.pdu.init_msg().value.type().to_string());
        break;
      case asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome:
        logger.info("Received a E2AP PDU of type {}.{}",
                    msg.pdu.type().to_string(),
                    msg.pdu.successful_outcome().value.type().to_string());
        break;
      case asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome:
        logger.info("Received a E2AP PDU of type {}.{}",
                    msg.pdu.type().to_string(),
                    msg.pdu.unsuccessful_outcome().value.type().to_string());
        break;
      case asn1::e2ap::e2ap_pdu_c::types_opts::nulltype:
        logger.info("Received a E2AP PDU of type nulltype");
        break;
    }

    last_e2_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding E2AP PDU");
      handler->handle_message(msg);
    }
  }
  e2_message last_e2_msg;

private:
  srslog::basic_logger& logger;
  e2_message_handler*   handler = nullptr;
};

class dummy_e2ap_pcap : public dlt_pcap
{
public:
  void flush() override {}
  void close() override {}
  bool is_write_enabled() const override { return false; }
  void push_pdu(const_span<uint8_t> pdu) override {}
  void push_pdu(byte_buffer pdu) override {}
};

inline e2_message generate_e2_setup_request(std::string oid)
{
  using namespace asn1::e2ap;
  e2_message  e2_msg;
  init_msg_s& initmsg = e2_msg.pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_E2SETUP);
  e2setup_request_s& setup = initmsg.value.e2setup_request();

  //  Transaction ID
  setup->transaction_id = 1;

  // Global e2 node ID
  auto& gnb_id = setup->global_e2node_id.set_gnb();
  gnb_id.global_gnb_id.plmn_id.from_number(3617847);
  gnb_id.global_gnb_id.gnb_id.gnb_id().from_number(381210353);

  // RAN functions added
  asn1::protocol_ie_single_container_s<ran_function_item_ies_o> ran_func_item;
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
  ran_func_item.value().ran_function_item().ran_function_o_id.resize(30);
  ran_func_item.value().ran_function_item().ran_function_o_id.from_string(oid);
  setup->ran_functions_added.push_back(ran_func_item);

  // E2 node component config
  auto& list = setup->e2node_component_cfg_addition;
  list.resize(1);
  list[0].load_info_obj(ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ITEM);
  e2node_component_cfg_addition_item_s& e2node_cfg_item = list[0].value().e2node_component_cfg_addition_item();
  e2node_cfg_item.e2node_component_interface_type       = e2node_component_interface_type_opts::ng;
  e2node_cfg_item.e2node_component_id.set_e2node_component_interface_type_ng().amf_name.from_string("nginterf");
  e2node_cfg_item.e2node_component_cfg.e2node_component_request_part.from_string("72657170617274");
  e2node_cfg_item.e2node_component_cfg.e2node_component_resp_part.from_string("72657370617274");

  return e2_msg;
}

inline e2_message
generate_ric_control_request_style2_action6(srslog::basic_logger&                      logger,
                                            const std::vector<rrm_policy_ratio_group>& rrm_policy_ratio_list_def)
{
  using namespace asn1::e2ap;

  e2_message  e2_msg;
  init_msg_s& initmsg = e2_msg.pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_RIC_CTRL);

  ric_ctrl_request_s& ric_control_request           = initmsg.value.ric_ctrl_request();
  ric_control_request->ric_call_process_id_present  = false;
  ric_control_request->ric_ctrl_ack_request_present = true;
  ric_control_request->ric_ctrl_ack_request.value   = ric_ctrl_ack_request_e::options::ack;

  ric_control_request->ran_function_id                 = 3;
  ric_control_request->ric_request_id.ric_requestor_id = 3;
  ric_control_request->ric_request_id.ric_instance_id  = 0;

  asn1::e2sm::e2sm_rc_ctrl_hdr_s ctrl_hdr;
  ctrl_hdr.ric_ctrl_hdr_formats.set_ctrl_hdr_format1();
  ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1().ric_ctrl_decision_present = false;
  ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1().ric_style_type            = 2;
  ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1().ric_ctrl_action_id        = 6;
  ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1().ue_id.set_gnb_du_ue_id();
  ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1().ue_id.gnb_du_ue_id().gnb_cu_ue_f1ap_id = 4;

  srsran::byte_buffer ctrl_hdr_buff;
  asn1::bit_ref       bref_msg(ctrl_hdr_buff);
  if (ctrl_hdr.pack(bref_msg) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack E2SM RC control header\n");
  }

  (void)ric_control_request->ric_ctrl_hdr.resize(ctrl_hdr_buff.length());
  std::copy(ctrl_hdr_buff.begin(), ctrl_hdr_buff.end(), ric_control_request->ric_ctrl_hdr.begin());

  asn1::e2sm::e2sm_rc_ctrl_msg_s ctrl_msg;
  ctrl_msg.ric_ctrl_msg_formats.set_ctrl_msg_format1();
  asn1::e2sm::e2sm_rc_ctrl_msg_format1_s& ctrl_msg_f1 = ctrl_msg.ric_ctrl_msg_formats.ctrl_msg_format1();

  unsigned nof_policy_groups = rrm_policy_ratio_list_def.size();
  // 1 - RRM Policy Ratio List.
  ctrl_msg_f1.ran_p_list.resize(nof_policy_groups);
  for (unsigned g = 0; g < nof_policy_groups; ++g) {
    const auto& rrm_policy_ratio_grp   = rrm_policy_ratio_list_def[g];
    unsigned    nof_members            = rrm_policy_ratio_grp.policy_members_list.size();
    auto&       rrm_policy_ratio_list  = ctrl_msg_f1.ran_p_list[g];
    rrm_policy_ratio_list.ran_param_id = 1;
    rrm_policy_ratio_list.ran_param_value_type.set_ran_p_choice_list().ran_param_list.list_of_ran_param.resize(1);
    rrm_policy_ratio_list.ran_param_value_type.ran_p_choice_list()
        .ran_param_list.list_of_ran_param[0]
        .seq_of_ran_params.resize(1);

    // 2 - RRM Policy Ratio Group.
    auto& rrm_policy_ratio_list_group = rrm_policy_ratio_list.ran_param_value_type.ran_p_choice_list()
                                            .ran_param_list.list_of_ran_param[0]
                                            .seq_of_ran_params[0];
    rrm_policy_ratio_list_group.ran_param_id = 2;
    rrm_policy_ratio_list_group.ran_param_value_type.set_ran_p_choice_structure()
        .ran_param_structure.seq_of_ran_params.resize(4); // policy, min, max, ded.
    // 3 - RRM Policy.
    auto& rrm_policy = rrm_policy_ratio_list_group.ran_param_value_type.ran_p_choice_structure()
                           .ran_param_structure.seq_of_ran_params[0];
    rrm_policy.ran_param_id = 3;
    rrm_policy.ran_param_value_type.set_ran_p_choice_structure().ran_param_structure.seq_of_ran_params.resize(1);
    // 5 - RRM Policy Member List.
    auto& rrm_policy_member_list =
        rrm_policy.ran_param_value_type.ran_p_choice_structure().ran_param_structure.seq_of_ran_params.back();
    rrm_policy_member_list.ran_param_id = 5;
    rrm_policy_member_list.ran_param_value_type.set_ran_p_choice_list().ran_param_list.list_of_ran_param.resize(
        1); // 1 policy member
    rrm_policy_member_list.ran_param_value_type.ran_p_choice_list()
        .ran_param_list.list_of_ran_param[0]
        .seq_of_ran_params.resize(nof_members);

    for (unsigned i = 0; i < nof_members; ++i) {
      auto& policy_member_def = rrm_policy_ratio_grp.policy_members_list[i];
      // 6 - RRM Policy Member.
      auto& rrm_policy_member = rrm_policy_member_list.ran_param_value_type.ran_p_choice_list()
                                    .ran_param_list.list_of_ran_param[0]
                                    .seq_of_ran_params[i];
      rrm_policy_member.ran_param_id = 6;
      rrm_policy_member.ran_param_value_type.set_ran_p_choice_structure().ran_param_structure.seq_of_ran_params.resize(
          2); // plmn, nssai
      // 7 - PLMN Identity.
      auto& plmn =
          rrm_policy_member.ran_param_value_type.ran_p_choice_structure().ran_param_structure.seq_of_ran_params[0];
      plmn.ran_param_id = 7;
      plmn.ran_param_value_type.set_ran_p_choice_elem_false();
      plmn.ran_param_value_type.ran_p_choice_elem_false().ran_param_value_present = true;
      asn1::unbounded_octstring<true> plmn_id_asn;
      plmn_id_asn.from_bytes(policy_member_def.plmn_id.to_bytes());
      plmn.ran_param_value_type.ran_p_choice_elem_false().ran_param_value.set_value_oct_s() = plmn_id_asn;
      //  8 - S-NSSAI.
      auto& nssai =
          rrm_policy_member.ran_param_value_type.ran_p_choice_structure().ran_param_structure.seq_of_ran_params[1];
      nssai.ran_param_id = 8;
      nssai.ran_param_value_type.set_ran_p_choice_structure().ran_param_structure.seq_of_ran_params.resize(
          2); // sst, sd
      // 9 - SST.
      auto& sst        = nssai.ran_param_value_type.ran_p_choice_structure().ran_param_structure.seq_of_ran_params[0];
      sst.ran_param_id = 9;
      sst.ran_param_value_type.set_ran_p_choice_elem_false();
      sst.ran_param_value_type.ran_p_choice_elem_false().ran_param_value_present = true;
      sst.ran_param_value_type.ran_p_choice_elem_false().ran_param_value.set_value_oct_s().from_number(
          policy_member_def.s_nssai.sst.value());
      // 10 - SD.
      auto& sd        = nssai.ran_param_value_type.ran_p_choice_structure().ran_param_structure.seq_of_ran_params[1];
      sd.ran_param_id = 10;
      sd.ran_param_value_type.set_ran_p_choice_elem_false();
      sd.ran_param_value_type.ran_p_choice_elem_false().ran_param_value_present = true;
      sd.ran_param_value_type.ran_p_choice_elem_false().ran_param_value.set_value_oct_s().from_number(
          policy_member_def.s_nssai.sd.value());
    }

    // 11 - Min PRB Policy Ratio.
    auto& min_prb = rrm_policy_ratio_list_group.ran_param_value_type.ran_p_choice_structure()
                        .ran_param_structure.seq_of_ran_params[1];
    min_prb.ran_param_id = 11;
    min_prb.ran_param_value_type.set_ran_p_choice_elem_false();
    min_prb.ran_param_value_type.ran_p_choice_elem_false().ran_param_value_present = true;
    min_prb.ran_param_value_type.ran_p_choice_elem_false().ran_param_value.set_value_int() =
        *rrm_policy_ratio_grp.minimum_ratio;
    // 12 - Max PRB Policy Ratio.
    auto& max_prb = rrm_policy_ratio_list_group.ran_param_value_type.ran_p_choice_structure()
                        .ran_param_structure.seq_of_ran_params[2];
    max_prb.ran_param_id = 12;
    max_prb.ran_param_value_type.set_ran_p_choice_elem_false();
    max_prb.ran_param_value_type.ran_p_choice_elem_false().ran_param_value_present = true;
    max_prb.ran_param_value_type.ran_p_choice_elem_false().ran_param_value.set_value_int() =
        *rrm_policy_ratio_grp.maximum_ratio;
    // 13 - Dedicated PRB Policy Ratio.
    auto& ded_prb = rrm_policy_ratio_list_group.ran_param_value_type.ran_p_choice_structure()
                        .ran_param_structure.seq_of_ran_params[3];
    ded_prb.ran_param_id = 13;
    ded_prb.ran_param_value_type.set_ran_p_choice_elem_false();
    ded_prb.ran_param_value_type.ran_p_choice_elem_false().ran_param_value_present = true;
    ded_prb.ran_param_value_type.ran_p_choice_elem_false().ran_param_value.set_value_int() =
        *rrm_policy_ratio_grp.dedicated_ratio;
  }

  srsran::byte_buffer ctrl_msg_buff;
  asn1::bit_ref       bref_msg1(ctrl_msg_buff);
  if (ctrl_msg.pack(bref_msg1) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack E2SM RC control message\n");
  }

  (void)ric_control_request->ric_ctrl_msg.resize(ctrl_msg_buff.length());
  std::copy(ctrl_msg_buff.begin(), ctrl_msg_buff.end(), ric_control_request->ric_ctrl_msg.begin());
  return e2_msg;
}

inline e2_message generate_ric_control_request(srslog::basic_logger& logger,
                                               int64_t               style_type,
                                               uint64_t              action_id,
                                               uint64_t              param_id,
                                               uint64_t              param_value)
{
  using namespace asn1::e2ap;
  e2_message  e2_msg;
  init_msg_s& initmsg = e2_msg.pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_RIC_CTRL);

  ric_ctrl_request_s& ric_control_request           = initmsg.value.ric_ctrl_request();
  ric_control_request->ric_call_process_id_present  = false;
  ric_control_request->ric_ctrl_ack_request_present = true;
  ric_control_request->ric_ctrl_ack_request.value   = ric_ctrl_ack_request_e::options::ack;

  ric_control_request->ran_function_id                 = 3;
  ric_control_request->ric_request_id.ric_instance_id  = 3;
  ric_control_request->ric_request_id.ric_requestor_id = 0;

  asn1::e2sm::e2sm_rc_ctrl_hdr_s ctrl_hdr;
  ctrl_hdr.ric_ctrl_hdr_formats.set_ctrl_hdr_format1();

  ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1().ric_style_type     = style_type;
  ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1().ric_ctrl_action_id = action_id;
  ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1().ue_id.set_gnb_ue_id();
  ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1().ue_id.gnb_du_ue_id().gnb_cu_ue_f1ap_id = 4;

  srsran::byte_buffer ctrl_hdr_buff;
  asn1::bit_ref       bref_msg(ctrl_hdr_buff);
  if (ctrl_hdr.pack(bref_msg) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack E2SM RC control header\n");
  }

  (void)ric_control_request->ric_ctrl_hdr.resize(ctrl_hdr_buff.length());
  std::copy(ctrl_hdr_buff.begin(), ctrl_hdr_buff.end(), ric_control_request->ric_ctrl_hdr.begin());

  asn1::e2sm::e2sm_rc_ctrl_msg_s ctrl_msg;
  ctrl_msg.ric_ctrl_msg_formats.set_ctrl_msg_format1();
  ctrl_msg.ric_ctrl_msg_formats.ctrl_msg_format1().ran_p_list.resize(1);
  ctrl_msg.ric_ctrl_msg_formats.ctrl_msg_format1().ran_p_list[0].ran_param_id = param_id;

  ctrl_msg.ric_ctrl_msg_formats.ctrl_msg_format1()
      .ran_p_list[0]
      .ran_param_value_type.set_ran_p_choice_elem_false()
      .ran_param_value_present = true;

  ctrl_msg.ric_ctrl_msg_formats.ctrl_msg_format1()
      .ran_p_list[0]
      .ran_param_value_type.ran_p_choice_elem_false()
      .ran_param_value.set_value_int() = param_value;

  srsran::byte_buffer ctrl_msg_buff;
  asn1::bit_ref       bref_msg1(ctrl_msg_buff);
  if (ctrl_msg.pack(bref_msg1) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack E2SM RC control message\n");
  }

  (void)ric_control_request->ric_ctrl_msg.resize(ctrl_msg_buff.length());
  std::copy(ctrl_msg_buff.begin(), ctrl_msg_buff.end(), ric_control_request->ric_ctrl_msg.begin());
  return e2_msg;
}

class dummy_e2_du_metrics : public e2_du_metrics_interface
{
public:
  void connect_e2_du_meas_provider(std::unique_ptr<e2_du_metrics_notifier> meas_provider) override
  {
    e2_meas_provider = std::move(meas_provider);
  }

private:
  std::unique_ptr<scheduler_metrics_notifier> e2_meas_provider;
};

class dummy_f1ap_ue_id_translator : public srs_du::f1ap_ue_id_translator
{
public:
  // F1AP UE ID translator functions.
  gnb_cu_ue_f1ap_id_t get_gnb_cu_ue_f1ap_id(const du_ue_index_t& ue_index) override
  {
    gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(ue_index);
    return gnb_cu_ue_f1ap_id;
  }

  gnb_cu_ue_f1ap_id_t get_gnb_cu_ue_f1ap_id(const gnb_du_ue_f1ap_id_t& gnb_du_ue_f1ap_id) override
  {
    gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(gnb_du_ue_f1ap_id_to_uint(gnb_du_ue_f1ap_id));
    return gnb_cu_ue_f1ap_id;
  }

  gnb_du_ue_f1ap_id_t get_gnb_du_ue_f1ap_id(const du_ue_index_t& ue_index) override
  {
    gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(ue_index);
    return gnb_du_ue_f1ap_id;
  }
  gnb_du_ue_f1ap_id_t get_gnb_du_ue_f1ap_id(const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id) override
  {
    gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(gnb_cu_ue_f1ap_id_to_uint(gnb_cu_ue_f1ap_id));
    return gnb_du_ue_f1ap_id;
  }

  du_ue_index_t get_ue_index(const gnb_du_ue_f1ap_id_t& gnb_du_ue_f1ap_id) override
  {
    du_ue_index_t du_ue_index = to_du_ue_index(gnb_du_ue_f1ap_id_to_uint(gnb_du_ue_f1ap_id));
    return du_ue_index;
  }

  du_ue_index_t get_ue_index(const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id) override
  {
    du_ue_index_t du_ue_index = to_du_ue_index(gnb_cu_ue_f1ap_id_to_uint(gnb_cu_ue_f1ap_id));
    return du_ue_index;
  }
};

class dummy_e2sm_kpm_du_meas_provider : public e2sm_kpm_meas_provider
{
public:
  dummy_e2sm_kpm_du_meas_provider() {}
  std::vector<std::string> get_supported_metric_names(e2sm_kpm_metric_level_enum level) override
  {
    return supported_metrics;
  }
  virtual bool is_cell_supported(const asn1::e2sm::cgi_c& cell_global_id) override { return true; }
  virtual bool is_ue_supported(const asn1::e2sm::ue_id_c& ueid) override { return true; }
  virtual bool is_test_cond_supported(const asn1::e2sm::test_cond_type_c& test_cond_type) override { return true; }

  virtual bool get_ues_matching_test_conditions(const asn1::e2sm::matching_cond_list_l& matching_cond_list,
                                                std::vector<asn1::e2sm::ue_id_c>&       ues) override
  {
    return get_ues_matching_cond(ues);
  }

  virtual bool
  get_ues_matching_test_conditions(const asn1::e2sm::matching_ue_cond_per_sub_list_l& matching_ue_cond_list,
                                   std::vector<asn1::e2sm::ue_id_c>&                  ues) override
  {
    return get_ues_matching_cond(ues);
  }

  virtual bool is_metric_supported(const asn1::e2sm::meas_type_c&   meas_type,
                                   const asn1::e2sm::meas_label_s&  label,
                                   const e2sm_kpm_metric_level_enum level,
                                   const bool&                      cell_scope) override
  {
    if (std::find(supported_metrics.begin(), supported_metrics.end(), meas_type.meas_name().to_string()) !=
        supported_metrics.end()) {
      return true;
    } else {
      return false;
    }
  }
  /// \return Returns True if measurement collection was successful
  virtual bool get_meas_data(const asn1::e2sm::meas_type_c&               meas_type,
                             const asn1::e2sm::label_info_list_l          label_info_list,
                             const std::vector<asn1::e2sm::ue_id_c>&      ues,
                             const std::optional<asn1::e2sm::cgi_c>       cell_global_id,
                             std::vector<asn1::e2sm::meas_record_item_c>& items) override
  {
    if (ues.size() == 0) {
      // E2 Node level measurements
      asn1::e2sm::meas_record_item_c meas_record_item;
      if (meas_type.meas_name().to_string() == "DRB.RlcSduDelayDl") {
        if (meas_values_float.size()) {
          meas_record_item.set_real();
          meas_record_item.real().value = meas_values_float[0];
        } else {
          meas_record_item.set_real();
          meas_record_item.real().value = 0.15625;
        }
      } else {
        if (meas_values_int.size()) {
          meas_record_item.set_integer() = meas_values_int[0];
        } else {
          meas_record_item.set_integer() = 1;
        }
      }
      items.push_back(meas_record_item);
      return true;
    }
    // UE level measurements
    for (auto& ue_id : ues) {
      uint32_t                        ueid_  = ue_id.gnb_du_ue_id().gnb_cu_ue_f1ap_id;
      std::vector<uint32_t>::iterator it     = std::find(ue_ids.begin(), ue_ids.end(), ueid_);
      uint32_t                        ue_idx = std::distance(ue_ids.begin(), it);

      asn1::e2sm::meas_record_item_c meas_record_item;
      if (ue_idx < presence.size()) {
        if (presence[ue_idx]) {
          if (meas_type.meas_name().to_string() == "DRB.RlcSduDelayDl") {
            if (meas_values_float.size()) {
              meas_record_item.set_real();
              meas_record_item.real().value = meas_values_float[ue_idx];
            } else {
              meas_record_item.set_real();
              meas_record_item.real().value = 0.15625;
            }
          } else {
            if (meas_values_int.size()) {
              meas_record_item.set_integer() = meas_values_int[ue_idx];
            } else {
              meas_record_item.set_integer() = 1;
            }
          }
        } else {
          meas_record_item.set_no_value();
        }
      } else {
        // no info on presence nor meas_value, by default return value 1002
        meas_record_item.set_integer() = 1002;
      }
      items.push_back(meas_record_item);
    }

    if (items.size()) {
      return true;
    }

    return false;
  }

  void push_measurements_int(std::vector<uint32_t> presence_,
                             std::vector<uint32_t> cond_satisfied_,
                             std::vector<uint32_t> meas_values_int_)
  {
    presence        = presence_;
    cond_satisfied  = cond_satisfied_;
    meas_values_int = meas_values_int_;
  }

  void push_measurements_float(std::vector<uint32_t> presence_,
                               std::vector<uint32_t> cond_satisfied_,
                               std::vector<float>    meas_values_float_)
  {
    presence          = presence_;
    cond_satisfied    = cond_satisfied_;
    meas_values_float = meas_values_float_;
  }

  void set_ue_ids(std::vector<uint32_t> ue_ids_) { ue_ids = ue_ids_; }

private:
  bool get_ues_matching_cond(std::vector<asn1::e2sm::ue_id_c>& ues)
  {
    for (unsigned ue_idx = 0; ue_idx < presence.size(); ue_idx++) {
      if (presence[ue_idx] and cond_satisfied[ue_idx]) {
        asn1::e2sm::ue_id_c        ueid;
        asn1::e2sm::ue_id_gnb_du_s ueid_gnb_du{};
        ueid_gnb_du.gnb_cu_ue_f1ap_id = ue_ids[ue_idx];
        ueid_gnb_du.ran_ue_id_present = false;
        ueid.set_gnb_du_ue_id()       = ueid_gnb_du;
        ues.push_back(ueid);
      }
    }
    if (ues.size()) {
      return true;
    }
    return false;
  }

  std::vector<std::string> supported_metrics =
      {"CQI", "RSRP", "RSRQ", "DRB.UEThpDl", "DRB.UEThpUl", "DRB.RlcSduDelayDl"};
  std::vector<uint32_t> presence          = {1};
  std::vector<uint32_t> cond_satisfied    = {1};
  std::vector<float>    meas_values_float = {0.15625};
  std::vector<uint32_t> meas_values_int   = {1};
  std::vector<uint32_t> ue_ids            = {0};
};

class dummy_e2_subscription_mngr : public e2_subscription_manager
{
public:
  dummy_e2_subscription_mngr() : logger(srslog::fetch_basic_logger("TEST")) {}
  e2_subscribe_reponse_message handle_subscription_setup(const asn1::e2ap::ric_sub_request_s& request) override
  {
    last_subscription = request;
    logger.info("Received a subscription request with action list size {}",
                last_subscription->ric_sub_details.ric_action_to_be_setup_list.size());
    e2_subscribe_reponse_message msg;
    get_subscription_result(msg);
    return msg;
  }

  e2_subscribe_delete_response_message
  handle_subscription_delete(const asn1::e2ap::ric_sub_delete_request_s& msg) override
  {
    e2_subscribe_delete_response_message outcome;
    return outcome;
  }

  void start_subscription(const asn1::e2ap::ric_request_id_s& ric_request_id,
                          uint16_t                            ran_func_id,
                          e2_event_manager&                   ev_mng,
                          e2_message_notifier&                tx_pdu_notifier) override
  {
  }

  void stop_subscription(const asn1::e2ap::ric_request_id_s&         ric_request_id,
                         e2_event_manager&                           ev_mng,
                         const asn1::e2ap::ric_sub_delete_request_s& msg) override
  {
  }

  void add_e2sm_service(std::string oid, std::unique_ptr<e2sm_interface> e2sm_iface) override {}

  e2sm_interface* get_e2sm_interface(std::string oid) override { return nullptr; }

  void add_ran_function_oid(uint16_t ran_func_id, std::string oid) override {}

  void stop() override {}

private:
  void get_subscription_result(e2_subscribe_reponse_message& msg)
  {
    msg.success    = true;
    msg.request_id = last_subscription->ric_request_id;

    logger.info("Sending subscription result for Request instance ID {}", msg.request_id.ric_instance_id);
    unsigned action_list_size = last_subscription->ric_sub_details.ric_action_to_be_setup_list.size();
    msg.admitted_list.resize(action_list_size);
    for (unsigned i = 0; i < action_list_size; i++) {
      auto& item =
          last_subscription->ric_sub_details.ric_action_to_be_setup_list[i].value().ric_action_to_be_setup_item();
      msg.admitted_list[i].value().ric_action_admitted_item().ric_action_id = item.ric_action_id;
    }
  }

private:
  asn1::e2ap::ric_sub_request_s last_subscription;
  srslog::basic_logger&         logger;
};

inline e2_message generate_e2_setup_request_message(std::string oid)
{
  e2_message e2_msg = {};
  e2_msg.pdu.set_init_msg();
  e2_msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  e2_msg.pdu.init_msg().value.e2setup_request()->transaction_id = 1;
  e2_msg.pdu.init_msg().value.e2setup_request() = generate_e2_setup_request(oid).pdu.init_msg().value.e2setup_request();

  return e2_msg;
}

inline e2_message generate_e2_setup_response(unsigned transaction_id)
{
  e2_message e2_setup_response = {};
  e2_setup_response.pdu.set_successful_outcome();
  e2_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  auto& setup                           = e2_setup_response.pdu.successful_outcome().value.e2setup_resp();
  setup->transaction_id                 = transaction_id;
  setup->ran_functions_accepted_present = true;
  asn1::protocol_ie_single_container_s<asn1::e2ap::ran_function_id_item_ies_o> ran_func_item;
  ran_func_item.value().ran_function_id_item().ran_function_id       = 1;
  ran_func_item.value().ran_function_id_item().ran_function_revision = 0;
  setup->ran_functions_accepted.push_back(ran_func_item);
  setup->global_ric_id.plmn_id.from_number(131014);
  setup->global_ric_id.ric_id.from_number(699598);
  // fill the required part with dummy data
  setup->e2node_component_cfg_addition_ack.resize(1);
  asn1::e2ap::e2node_component_cfg_addition_ack_item_s& e2node_component_cfg_addition_ack_item =
      setup->e2node_component_cfg_addition_ack[0].value().e2node_component_cfg_addition_ack_item();
  e2node_component_cfg_addition_ack_item.e2node_component_interface_type =
      asn1::e2ap::e2node_component_interface_type_e::e2node_component_interface_type_opts::e1;
  e2node_component_cfg_addition_ack_item.e2node_component_id.set_e2node_component_interface_type_e1().gnb_cu_up_id =
      123;
  e2node_component_cfg_addition_ack_item.e2node_component_cfg_ack.upd_outcome =
      asn1::e2ap::e2node_component_cfg_ack_s::upd_outcome_opts::success;
  return e2_setup_response;
}

inline e2_message generate_e2_setup_failure(unsigned transaction_id)
{
  e2_message e2_setup_failure = {};
  e2_setup_failure.pdu.set_unsuccessful_outcome();
  e2_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  auto& setup           = e2_setup_failure.pdu.unsuccessful_outcome().value.e2setup_fail();
  setup->transaction_id = transaction_id;
  setup->cause.set_misc();
  return e2_setup_failure;
}

inline asn1::e2sm::ue_id_gnb_du_s generate_ueid_gnb_du(uint32_t ue_idx)
{
  asn1::e2sm::ue_id_gnb_du_s ueid;
  ueid.gnb_cu_ue_f1ap_id = ue_idx;
  ueid.ran_ue_id_present = false;
  return ueid;
}

inline asn1::e2ap::ric_action_to_be_setup_item_s
generate_e2sm_kpm_ric_action(asn1::e2sm::e2sm_kpm_action_definition_s& action_def)
{
  asn1::e2ap::ric_action_to_be_setup_item_s ric_action;
  ric_action.ric_action_type               = asn1::e2ap::ric_action_type_opts::report;
  ric_action.ric_action_id                 = 0;
  ric_action.ric_subsequent_action_present = false;

  byte_buffer   buf;
  asn1::bit_ref bref(buf);
  if (action_def.pack(bref) != asn1::SRSASN_SUCCESS) {
    return {};
  }

  (void)ric_action.ric_action_definition.resize(buf.length());
  std::copy(buf.begin(), buf.end(), ric_action.ric_action_definition.begin());
  return ric_action;
}

inline e2_message generate_e2sm_kpm_subscription_request(asn1::e2ap::ric_action_to_be_setup_item_s& ric_action)
{
  asn1::e2ap::ric_sub_request_s ric_subscript_reqs;
  ric_subscript_reqs->ran_function_id                 = e2sm_kpm_asn1_packer::ran_func_id;
  ric_subscript_reqs->ric_request_id.ric_requestor_id = 1;
  ric_subscript_reqs->ric_request_id.ric_instance_id  = 0;
  ric_subscript_reqs->ric_sub_details.ric_action_to_be_setup_list.resize(1);
  ric_subscript_reqs->ric_sub_details.ric_action_to_be_setup_list[0]->ric_action_to_be_setup_item() = ric_action;
  asn1::e2sm::e2sm_kpm_event_trigger_definition_s e2sm_kpm_event_trigger_def;
  e2sm_kpm_event_trigger_def.event_definition_formats.event_definition_format1().report_period = 1000;

  byte_buffer   buf;
  asn1::bit_ref bref(buf);
  if (e2sm_kpm_event_trigger_def.pack(bref) != asn1::SRSASN_SUCCESS) {
    return {};
  }

  (void)ric_subscript_reqs->ric_sub_details.ric_event_trigger_definition.resize(buf.length());
  std::copy(buf.begin(), buf.end(), ric_subscript_reqs->ric_sub_details.ric_event_trigger_definition.begin());

  e2_message e2_msg;
  e2_msg.pdu.set_init_msg();
  e2_msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_RIC_SUB);
  e2_msg.pdu.init_msg().value.ric_sub_request() = ric_subscript_reqs;
  return e2_msg;
}

inline e2_message generate_e2_ind_msg(byte_buffer& ind_hdr_bytes, byte_buffer& ind_msg_bytes)
{
  e2_indication_message e2_ind;
  e2_ind.request_id.ric_requestor_id                 = 1;
  e2_ind.request_id.ric_instance_id                  = 0;
  e2_ind.indication->ric_request_id.ric_requestor_id = 1;
  e2_ind.indication->ric_request_id.ric_instance_id  = 0;
  e2_ind.indication->ran_function_id                 = e2sm_kpm_asn1_packer::ran_func_id;
  e2_ind.indication->ric_action_id                   = 4;
  e2_ind.indication->ric_ind_sn_present              = true;
  e2_ind.indication->ric_ind_sn                      = 1234;
  e2_ind.indication->ric_ind_type.value              = asn1::e2ap::ric_ind_type_e::ric_ind_type_opts::report;
  e2_ind.indication->ric_call_process_id_present     = false;

  // put RIC indication content into message
  (void)e2_ind.indication->ric_ind_msg.resize(ind_msg_bytes.length());
  std::copy(ind_msg_bytes.begin(), ind_msg_bytes.end(), e2_ind.indication->ric_ind_msg.begin());
  (void)e2_ind.indication->ric_ind_hdr.resize(ind_hdr_bytes.length());
  std::copy(ind_hdr_bytes.begin(), ind_hdr_bytes.end(), e2_ind.indication->ric_ind_hdr.begin());

  e2_message e2_msg;
  e2_msg.pdu.set_init_msg();
  e2_msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_RIC_IND);
  e2_msg.pdu.init_msg().value.ric_ind() = e2_ind.indication;
  return e2_msg;
}

/// Dummy handler just printing the received PDU.
class dummy_e2_message_handler : public e2_message_handler
{
public:
  dummy_e2_message_handler() : logger(srslog::fetch_basic_logger("TEST")) {}
  void handle_message(const e2_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a E2AP PDU of type {}", msg.pdu.type().to_string());
  }

  e2_message last_msg;

private:
  srslog::basic_logger& logger;
};

/// Dummy handler just printing the received PDU.
class dummy_e2_agent_mng : public e2ap_e2agent_notifier
{
public:
  dummy_e2_agent_mng() : logger(srslog::fetch_basic_logger("TEST")) {}
  void on_e2_disconnection() override { logger.info("E2 connection closed."); }

private:
  srslog::basic_logger& logger;
};

/// Dummy PDU handler
class dummy_sctp_association_sdu_notifier : public sctp_association_sdu_notifier
{
public:
  dummy_sctp_association_sdu_notifier() = default;
  bool on_new_sdu(byte_buffer pdu) override
  {
    last_pdu = pdu.copy();
    return true;
  }
  byte_buffer last_pdu;
};

class dummy_e2_adapter : public e2_message_handler, public e2_event_handler
{
public:
  dummy_e2_adapter() : logger(srslog::fetch_basic_logger("E2")) {}

  void connect_e2ap(e2_interface* e2ap_) { e2ap = e2ap_; }
  void handle_message(const e2_message& msg) override { e2ap->handle_message(msg); }
  void handle_connection_loss() override { e2ap->handle_connection_loss(); }

private:
  srslog::basic_logger& logger;
  e2_interface*         e2ap = nullptr;
};

class dummy_e2sm_handler : public e2sm_handler
{
  e2sm_action_definition handle_packed_e2sm_action_definition(const srsran::byte_buffer& buf) override
  {
    e2sm_action_definition action_def;
    action_def.service_model = e2sm_service_model_t::KPM;
    asn1::e2sm::e2sm_kpm_action_definition_s& e2sm_kpm_action_definition =
        std::get<asn1::e2sm::e2sm_kpm_action_definition_s>(action_def.action_definition);
    e2sm_kpm_action_definition.ric_style_type = 3;
    e2sm_kpm_action_definition.action_definition_formats.set_action_definition_format3();
    e2sm_kpm_action_definition.action_definition_formats.action_definition_format3().meas_cond_list.resize(1);
    e2sm_kpm_action_definition.action_definition_formats.action_definition_format3()
        .meas_cond_list[0]
        .meas_type.set_meas_name()
        .from_string("RSRP");
    e2sm_kpm_action_definition.action_definition_formats.action_definition_format3().granul_period = 10;
    return action_def;
  }
  e2sm_ric_control_request handle_packed_ric_control_request(const asn1::e2ap::ric_ctrl_request_s& req) override
  {
    e2sm_ric_control_request ric_control_request = {};
    return ric_control_request;
  }
  e2_ric_control_response pack_ric_control_response(const e2sm_ric_control_response& e2sm_response) override
  {
    e2_ric_control_response ric_control_response = {};
    return ric_control_response;
  }
  e2sm_event_trigger_definition handle_packed_event_trigger_definition(const srsran::byte_buffer& buf) override
  {
    e2sm_event_trigger_definition e2sm_event_trigger_def;
    e2sm_event_trigger_def.report_period = 10;
    return e2sm_event_trigger_def;
  }
  asn1::unbounded_octstring<true> pack_ran_function_description() override { return {}; }
};

class dummy_e2_connection_client final : public e2_connection_client
{
public:
  e2_message last_tx_e2_pdu;

  std::unique_ptr<e2_message_notifier>
  handle_e2_connection_request(std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier_) override
  {
    e2_rx_pdu_notifier = std::move(e2_rx_pdu_notifier_);
    return std::make_unique<dummy_e2_tx_pdu_notifier>(last_tx_e2_pdu, [this]() { e2_rx_pdu_notifier.reset(); });
  }

private:
  std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier;
};

class dummy_e2_mobility_notifier : public srs_cu_cp::mobility_manager_cu_cp_notifier
{
public:
  virtual async_task<srs_cu_cp::cu_cp_intra_cu_handover_response>
  on_intra_cu_handover_required(const srs_cu_cp::cu_cp_intra_cu_handover_request& request,
                                srs_cu_cp::du_index_t                             source_du_index,
                                srs_cu_cp::du_index_t                             target_du_index) override
  {
    return launch_async([](coro_context<async_task<srs_cu_cp::cu_cp_intra_cu_handover_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(srs_cu_cp::cu_cp_intra_cu_handover_response{true});
    });
  }
};

class dummy_du_configurator : public srs_du::du_configurator
{
public:
  dummy_du_configurator() {}
  async_task<srs_du::du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(srs_du::du_mac_sched_control_config reconf) override
  {
    config = reconf;
    return launch_async([](coro_context<async_task<srs_du::du_mac_sched_control_config_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(srs_du::du_mac_sched_control_config_response{true, true, true});
    });
  }
  srs_du::du_param_config_response handle_operator_config_request(const srs_du::du_param_config_request& req) override
  {
    return srs_du::du_param_config_response{};
  }
  void handle_si_pdu_update(const srs_du::du_si_pdu_update_request& req) override {}

  srs_du::du_mac_sched_control_config config;
};

/// Dummy implementation of the CU configurator interface.
class dummy_cu_configurator : public cu_configurator
{
public:
  dummy_cu_configurator() {}

  srs_cu_cp::ue_index_t get_ue_index(const srs_cu_cp::amf_ue_id_t& amf_ue_id,
                                     const srs_cu_cp::guami_t&     guami,
                                     const gnb_cu_ue_f1ap_id_t&    gnb_cu_ue_f1ap_id) const override
  {
    return srs_cu_cp::uint_to_ue_index(1);
  }

  srs_cu_cp::du_index_t get_du_index(const srs_cu_cp::ue_index_t& ue_index) const override
  {
    return srs_cu_cp::uint_to_du_index(1);
  }

  srs_cu_cp::du_index_t get_du_index(const nr_cell_global_id_t& nr_cgi) const override
  {
    return srs_cu_cp::uint_to_du_index(1);
  }

  pci_t get_pci(const nr_cell_global_id_t& nr_cgi) const override { return pci_t(1); }

  async_task<srs_cu_cp::cu_cp_intra_cu_handover_response>
  trigger_handover(const srs_cu_cp::du_index_t&                      source_du_index,
                   const srs_cu_cp::cu_cp_intra_cu_handover_request& handover_req) override
  {
    return launch_async([](coro_context<async_task<srs_cu_cp::cu_cp_intra_cu_handover_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(srs_cu_cp::cu_cp_intra_cu_handover_response{true});
    });
  }
};

/// Fixture class for E2AP
class e2_base
{
protected:
  void tick()
  {
    timers.tick();
    task_worker.run_pending_tasks();
  }
  e2ap_configuration                                          cfg = {};
  timer_factory                                               factory;
  timer_manager                                               timers;
  std::unique_ptr<e2ap_e2agent_notifier>                      agent_notifier;
  std::unique_ptr<dummy_sctp_association_sdu_notifier>        gw;
  std::unique_ptr<dummy_e2ap_pcap>                            pcap;
  std::unique_ptr<e2ap_asn1_packer>                           packer;
  std::unique_ptr<e2sm_interface>                             e2sm_kpm_iface;
  std::unique_ptr<e2sm_interface>                             e2sm_rc_iface;
  std::unique_ptr<e2sm_control_service>                       e2sm_rc_control_service_style2;
  std::unique_ptr<e2sm_control_service>                       e2sm_rc_control_service_style3;
  std::unique_ptr<e2sm_control_action_executor>               rc_control_action_2_6_executor;
  std::unique_ptr<e2sm_control_action_executor>               rc_control_action_3_1_executor;
  std::unique_ptr<srs_cu_cp::mobility_manager_cu_cp_notifier> mobility_notifier;
  std::unique_ptr<e2sm_handler>                               e2sm_kpm_packer;
  std::unique_ptr<e2sm_rc_asn1_packer>                        e2sm_rc_packer;
  std::unique_ptr<dummy_du_configurator>                      du_rc_param_configurator;
  std::unique_ptr<cu_configurator>                            cu_rc_param_configurator;
  std::unique_ptr<e2_subscription_manager>                    e2_subscription_mngr;
  std::unique_ptr<e2_du_metrics_interface>                    du_metrics;
  std::unique_ptr<srs_du::f1ap_ue_id_translator>              f1ap_ue_id_mapper;
  std::unique_ptr<dummy_e2sm_kpm_du_meas_provider>            du_meas_provider;
  manual_task_worker                                          task_worker{64};
  std::unique_ptr<dummy_e2_pdu_notifier>                      msg_notifier;
  std::unique_ptr<dummy_e2_connection_client>                 e2_client;
  std::unique_ptr<e2sm_manager>                               e2sm_mngr;
  std::unique_ptr<e2_interface>                               e2;
  std::unique_ptr<e2_agent>                                   e2agent;
  srslog::basic_logger&                                       test_logger = srslog::fetch_basic_logger("TEST");
};

class e2_test_base : public e2_base, public ::testing::Test
{};

class e2_test_base_with_pcap : public e2_base, public testing::TestWithParam<dlt_pcap*>
{};

class e2_test : public e2_test_base
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    e2_client            = std::make_unique<dummy_e2_connection_client>();
    e2_subscription_mngr = std::make_unique<dummy_e2_subscription_mngr>();
    du_metrics           = std::make_unique<dummy_e2_du_metrics>();
    factory              = timer_factory{timers, task_worker};
    e2sm_mngr            = std::make_unique<e2sm_manager>(test_logger);
    agent_notifier       = std::make_unique<dummy_e2_agent_mng>();
    e2                   = std::make_unique<e2_impl>(
        test_logger, cfg, *agent_notifier, factory, *e2_client, *e2_subscription_mngr, *e2sm_mngr, task_worker);
    // Packer allows to inject packed message into E2 interface.
    gw     = std::make_unique<dummy_sctp_association_sdu_notifier>();
    pcap   = std::make_unique<dummy_e2ap_pcap>();
    packer = std::make_unique<e2ap_asn1_packer>(*gw, *e2, *pcap);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

class e2_entity_test : public e2_test_base
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg                  = config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    e2_client                = std::make_unique<dummy_e2_connection_client>();
    du_metrics               = std::make_unique<dummy_e2_du_metrics>();
    f1ap_ue_id_mapper        = std::make_unique<dummy_f1ap_ue_id_translator>();
    factory                  = timer_factory{timers, task_worker};
    du_rc_param_configurator = std::make_unique<dummy_du_configurator>();
    e2agent                  = create_e2_du_agent(
        cfg, *e2_client, &(*du_metrics), &(*f1ap_ue_id_mapper), &(*du_rc_param_configurator), factory, task_worker);
    // Packer allows to inject packed message into E2 interface.
    gw     = std::make_unique<dummy_sctp_association_sdu_notifier>();
    pcap   = std::make_unique<dummy_e2ap_pcap>();
    packer = std::make_unique<e2ap_asn1_packer>(*gw, e2agent->get_e2_interface(), *pcap);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

class e2_test_subscriber : public e2_test_base
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();
    cfg                  = config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    factory          = timer_factory{timers, task_worker};
    msg_notifier     = std::make_unique<dummy_e2_pdu_notifier>(nullptr);
    e2_client        = std::make_unique<dummy_e2_connection_client>();
    e2sm_kpm_packer  = std::make_unique<dummy_e2sm_handler>();
    du_metrics       = std::make_unique<dummy_e2_du_metrics>();
    du_meas_provider = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_kpm_iface   = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_kpm_packer, *du_meas_provider);
    e2sm_mngr        = std::make_unique<e2sm_manager>(test_logger);
    e2sm_mngr->add_e2sm_service("1.3.6.1.4.1.53148.1.2.2.2", std::move(e2sm_kpm_iface));
    e2sm_mngr->add_supported_ran_function(1, "1.3.6.1.4.1.53148.1.2.2.2");
    e2_subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*e2sm_mngr);
    e2_subscription_mngr->add_ran_function_oid(1, "1.3.6.1.4.1.53148.1.2.2.2");
    agent_notifier = std::make_unique<dummy_e2_agent_mng>();
    e2             = std::make_unique<e2_impl>(
        test_logger, cfg, *agent_notifier, factory, *e2_client, *e2_subscription_mngr, *e2sm_mngr, task_worker);
    // Packer allows to inject packed message into E2 interface.
    gw     = std::make_unique<dummy_sctp_association_sdu_notifier>();
    pcap   = std::make_unique<dummy_e2ap_pcap>();
    packer = std::make_unique<e2ap_asn1_packer>(*gw, *e2, *pcap);

    report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to create dummy SCTP connection");
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

class e2_test_setup : public e2_test_base
{
  void SetUp() override
  {
    cfg                  = config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    factory                        = timer_factory{timers, task_worker};
    e2_client                      = std::make_unique<dummy_e2_connection_client>();
    du_metrics                     = std::make_unique<dummy_e2_du_metrics>();
    du_meas_provider               = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_kpm_packer                = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    e2sm_kpm_iface                 = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_kpm_packer, *du_meas_provider);
    e2sm_rc_packer                 = std::make_unique<e2sm_rc_asn1_packer>();
    mobility_notifier              = std::make_unique<dummy_e2_mobility_notifier>();
    du_rc_param_configurator       = std::make_unique<dummy_du_configurator>();
    cu_rc_param_configurator       = std::make_unique<dummy_cu_configurator>();
    e2sm_rc_iface                  = std::make_unique<e2sm_rc_impl>(test_logger, *e2sm_rc_packer);
    e2sm_rc_control_service_style2 = std::make_unique<e2sm_rc_control_service>(2);
    e2sm_rc_control_service_style3 = std::make_unique<e2sm_rc_control_service>(3);
    f1ap_ue_id_mapper              = std::make_unique<dummy_f1ap_ue_id_translator>();
    rc_control_action_2_6_executor =
        std::make_unique<e2sm_rc_control_action_2_6_du_executor>(*du_rc_param_configurator, *f1ap_ue_id_mapper);
    rc_control_action_3_1_executor =
        std::make_unique<e2sm_rc_control_action_3_1_cu_executor>(*cu_rc_param_configurator);
    e2sm_rc_control_service_style2->add_e2sm_rc_control_action_executor(std::move(rc_control_action_2_6_executor));
    e2sm_rc_control_service_style3->add_e2sm_rc_control_action_executor(std::move(rc_control_action_3_1_executor));
    e2sm_rc_packer->add_e2sm_control_service(e2sm_rc_control_service_style2.get());
    e2sm_rc_packer->add_e2sm_control_service(e2sm_rc_control_service_style3.get());
    e2sm_rc_iface->add_e2sm_control_service(std::move(e2sm_rc_control_service_style2));
    e2sm_rc_iface->add_e2sm_control_service(std::move(e2sm_rc_control_service_style3));
    e2sm_mngr = std::make_unique<e2sm_manager>(test_logger);
    e2sm_mngr->add_e2sm_service("1.3.6.1.4.1.53148.1.2.2.2", std::move(e2sm_kpm_iface));
    e2sm_mngr->add_e2sm_service("1.3.6.1.4.1.53148.1.1.2.3", std::move(e2sm_rc_iface));
    e2sm_mngr->add_supported_ran_function(3, "1.3.6.1.4.1.53148.1.1.2.3");
    e2_subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*e2sm_mngr);
    agent_notifier       = std::make_unique<dummy_e2_agent_mng>();
    e2                   = std::make_unique<e2_impl>(
        test_logger, cfg, *agent_notifier, factory, *e2_client, *e2_subscription_mngr, *e2sm_mngr, task_worker);
    // Packer allows to inject packed message into E2 interface.
    gw     = std::make_unique<dummy_sctp_association_sdu_notifier>();
    pcap   = std::make_unique<dummy_e2ap_pcap>();
    packer = std::make_unique<e2ap_asn1_packer>(*gw, *e2, *pcap);
  }
  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

class dummy_e2_interface : public e2_interface
{
public:
  dummy_e2_interface() : logger(srslog::fetch_basic_logger("TEST")) {}

  // e2_message_handler interface
  void handle_message(const e2_message& msg) override
  {
    logger.info("Received E2 message of type {}", msg.pdu.type().to_string());
    last_msg = msg;
  }

  // e2_event_handler interface
  void handle_connection_loss() override { logger.info("E2 connection lost"); }

  // e2_connection_manager interface
  bool handle_e2_tnl_connection_request() override
  {
    logger.info("E2 TNL connection request received");
    return true;
  }

  async_task<void> handle_e2_disconnection_request() override
  {
    logger.info("E2 disconnection request received");
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  async_task<e2_setup_response_message> handle_e2_setup_request(e2_setup_request_message& request) override
  {
    logger.info("E2 setup request received");
    return launch_async([](coro_context<async_task<e2_setup_response_message>>& ctx) {
      CORO_BEGIN(ctx);
      e2_setup_response_message response;
      response.success = true;
      CORO_RETURN(response);
    });
  }

  async_task<e2_setup_response_message> start_initial_e2_setup_routine() override
  {
    logger.info("Starting initial E2 setup routine");
    return launch_async([](coro_context<async_task<e2_setup_response_message>>& ctx) {
      CORO_BEGIN(ctx);
      e2_setup_response_message response;
      response.success = true;
      CORO_RETURN(response);
    });
  }

  // e2_interface interface
  void start() override { logger.info("Starting dummy E2 interface"); }

  void stop() override { logger.info("Stopping dummy E2 interface"); }

  e2_message last_msg;

private:
  srslog::basic_logger& logger;
};

} // namespace srsran
