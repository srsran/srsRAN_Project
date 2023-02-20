/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/e2/common/e2ap_asn1_packer.h"
#include "srsgnb/asn1/e2ap/e2ap.h"
#include "srsgnb/e2/e2.h"
#include "srsgnb/e2/e2_factory.h"
#include "srsgnb/gateways/network_gateway.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>

namespace srsran {

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_e2_pdu_notifier : public e2_message_notifier
{
public:
  dummy_e2_pdu_notifier(e2_message_handler* handler_) : logger(srslog::fetch_basic_logger("TEST")), handler(handler_){};

  void attach_handler(e2_message_handler* handler_) { handler = handler_; };
  void on_new_message(const e2_message& msg) override
  {
    logger.info("Received a E2AP PDU of type {}", msg.pdu.type().to_string());

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

/// Fixture class for E2AP
class e2_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    msg_notifier = std::make_unique<dummy_e2_pdu_notifier>(nullptr);

    e2 = create_e2(timers, *msg_notifier);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<e2_interface>          e2;
  timer_manager                          timers;
  std::unique_ptr<dummy_e2_pdu_notifier> msg_notifier;
  srslog::basic_logger&                  test_logger = srslog::fetch_basic_logger("TEST");
};

e2_message generate_e2_setup_request()
{
  using namespace asn1::e2ap;
  e2_message  e2_msg;
  init_msg_s& initmsg = e2_msg.pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_E2SETUP);
  e2setup_request_s& setup = initmsg.value.e2setup_request();

  //  Transaction ID
  setup->transaction_id.crit        = asn1::crit_opts::reject;
  setup->transaction_id.value.value = 1;

  // Global e2 node ID
  setup->global_e2node_id.crit = asn1::crit_opts::reject;
  auto& gnb_id                 = setup->global_e2node_id.value.set_gnb();
  gnb_id.global_g_nb_id.plmn_id.from_number(3617847);
  gnb_id.global_g_nb_id.gnb_id.gnb_id().from_number(381210353);

  // RAN functions added
  setup->ra_nfunctions_added.crit = asn1::crit_opts::reject;
  setup->ra_nfunctions_added.id   = ASN1_E2AP_ID_RA_NFUNCTIONS_ADDED;
  asn1::protocol_ie_single_container_s<ra_nfunction_item_ies_o> ran_func_item;
  ran_func_item.load_info_obj(ASN1_E2AP_ID_RA_NFUNCTION_ITEM);
  ran_func_item.value().ra_nfunction_item().ran_function_id = 0;
  ran_func_item.value().ra_nfunction_item().ran_function_definition.from_string(
      "20C04F52414E2D4532534D2D4B504D0000054F494431323305004B504D206D6F6E69746F720860283861AAE33F0060000101070050657269"
      "6F646963207265706F727401051401011D004F2D4455204D6561737572656D656E7420436F6E7461696E657220666F722074686520354743"
      "20636F6E6E6563746564206465706C6F796D656E74010101010001021D004F2D4455204D6561737572656D656E7420436F6E7461696E6572"
      "20666F72207468652045504320636F6E6E6563746564206465706C6F796D656E74010101010001031E804F2D43552D4350204D6561737572"
      "656D656E7420436F6E7461696E657220666F72207468652035474320636F6E6E6563746564206465706C6F796D656E74010101010001041E"
      "804F2D43552D4350204D6561737572656D656E7420436F6E7461696E657220666F72207468652045504320636F6E6E656374656420646570"
      "6C6F796D656E74010101010001051E804F2D43552D5550204D6561737572656D656E7420436F6E7461696E657220666F7220746865203547"
      "4320636F6E6E6563746564206465706C6F796D656E74010101010001061E804F2D43552D5550204D6561737572656D656E7420436F6E7461"
      "696E657220666F72207468652045504320636F6E6E6563746564206465706C6F796D656E7401010101");
  ran_func_item.value().ra_nfunction_item().ran_function_oid.resize(1);
  setup->ra_nfunctions_added.value.push_back(ran_func_item);

  // E2 node component config
  setup->e2node_component_cfg_addition.crit = asn1::crit_opts::reject;
  auto& list                                = setup->e2node_component_cfg_addition.value;
  list.resize(1);
  list[0].load_info_obj(ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ITEM);
  e2node_component_cfg_addition_item_s& e2node_cfg_item = list[0].value().e2node_component_cfg_addition_item();
  e2node_cfg_item.e2node_component_interface_type       = e2node_component_interface_type_opts::ng;
  e2node_cfg_item.e2node_component_id.set_e2node_component_interface_type_ng().amf_name.from_string("nginterf");
  e2node_cfg_item.e2node_component_cfg.e2node_component_request_part.from_string("72657170617274");
  e2node_cfg_item.e2node_component_cfg.e2node_component_resp_part.from_string("72657370617274");

  return e2_msg;
}

e2_setup_request_message generate_e2_setup_request_message()
{
  e2_setup_request_message e2_msg            = {};
  e2_msg.request->transaction_id.value.value = 1;
  return e2_msg;
}

e2_message generate_e2_setup_response(unsigned transaction_id)
{
  e2_message e2_setup_response = {};
  e2_setup_response.pdu.set_successful_outcome();
  e2_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  auto& setup                       = e2_setup_response.pdu.successful_outcome().value.e2setup_resp();
  setup->transaction_id.value.value = transaction_id;

  setup->global_ric_id.value.plmn_id.from_number(131014);
  setup->global_ric_id.value.ric_id.from_number(699598);
  return e2_setup_response;
}

/// Dummy handler just printing the received PDU.
class dummy_e2_message_handler : public e2_message_handler
{
public:
  dummy_e2_message_handler() : logger(srslog::fetch_basic_logger("TEST")){};
  void handle_message(const e2_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a E2AP PDU of type {}", msg.pdu.type().to_string());
  }

  e2_message last_msg;

private:
  srslog::basic_logger& logger;
};

/// Dummy PDU handler
class dummy_network_gateway_data_handler : public srsran::sctp_network_gateway_data_handler
{
public:
  dummy_network_gateway_data_handler(){};
  void handle_pdu(const byte_buffer& pdu) override { last_pdu = pdu.copy(); }

  byte_buffer last_pdu;
};
} // namespace srsran
