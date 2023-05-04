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

#pragma once

#include "lib/e2/common/e2ap_asn1_packer.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_factory.h"
#include "srsran/gateways/network_gateway.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/timers.h"
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

inline e2_message generate_e2_setup_request()
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

class dummy_e2_subscriber : public e2_subscriber
{
public:
  dummy_e2_subscriber() : logger(srslog::fetch_basic_logger("TEST")){};
  e2_subscribe_reponse_message handle_subscription_setup(const asn1::e2ap::ricsubscription_request_s& request) override
  {
    last_subscription = request;
    logger.info("Received a subscription request with action list size {}",
                last_subscription->ricsubscription_details.value.ric_action_to_be_setup_list.size());
    e2_subscribe_reponse_message msg;
    get_subscription_result(msg);
    return msg;
  }

private:
  void get_subscription_result(e2_subscribe_reponse_message& msg)
  {
    msg.success    = true;
    msg.request_id = last_subscription->ri_crequest_id.value;

    logger.info("Sending subscription result for Request instance ID {}", msg.request_id.ric_instance_id);
    unsigned action_list_size = last_subscription->ricsubscription_details.value.ric_action_to_be_setup_list.size();
    msg.admitted_list.resize(action_list_size);
    for (unsigned i = 0; i < action_list_size; i++) {
      auto& item = last_subscription->ricsubscription_details.value.ric_action_to_be_setup_list[i]
                       .value()
                       .ri_caction_to_be_setup_item();
      msg.admitted_list[i].value().ri_caction_admitted_item().ric_action_id = item.ric_action_id;
    }
  }

private:
  asn1::e2ap::ricsubscription_request_s last_subscription;
  srslog::basic_logger&                 logger;
};

inline e2_message generate_e2_setup_request_message()
{
  e2_message e2_msg = {};
  e2_msg.pdu.set_init_msg();
  e2_msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  e2_msg.pdu.init_msg().value.e2setup_request()->transaction_id.value.value = 1;
  e2_msg.pdu.init_msg().value.e2setup_request() = generate_e2_setup_request().pdu.init_msg().value.e2setup_request();

  return e2_msg;
}

inline e2_message generate_e2_setup_response(unsigned transaction_id)
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

inline e2_message generate_e2_setup_failure(unsigned transaction_id)
{
  e2_message e2_setup_failure = {};
  e2_setup_failure.pdu.set_unsuccessful_outcome();
  e2_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  auto& setup                       = e2_setup_failure.pdu.unsuccessful_outcome().value.e2setup_fail();
  setup->transaction_id.value.value = transaction_id;
  setup->cause.value.set_misc();
  return e2_setup_failure;
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

/// Fixture class for E2AP
class e2_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    msg_notifier = std::make_unique<dummy_e2_pdu_notifier>(nullptr);
    subscriber   = std::make_unique<dummy_e2_subscriber>();
    e2           = create_e2(timer_factory{timers, task_worker}, *msg_notifier, *subscriber);
    gw           = std::make_unique<dummy_network_gateway_data_handler>();
    packer       = std::make_unique<srsran::e2ap_asn1_packer>(*gw, *e2);
    msg_notifier->attach_handler(&(*packer));
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
  std::unique_ptr<dummy_network_gateway_data_handler> gw;
  std::unique_ptr<e2_interface>                       e2;
  std::unique_ptr<srsran::e2ap_asn1_packer>           packer;
  std::unique_ptr<dummy_e2_subscriber>                subscriber;
  timer_manager                                       timers;
  manual_task_worker                                  task_worker{64};
  std::unique_ptr<dummy_e2_pdu_notifier>              msg_notifier;
  srslog::basic_logger&                               test_logger = srslog::fetch_basic_logger("TEST");
};
} // namespace srsran
