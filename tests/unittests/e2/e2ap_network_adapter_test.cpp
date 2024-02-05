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

#include "../gateways/test_helpers.h"
#include "apps/gnb/adapters/e2ap_adapter.h"
#include "lib/e2/common/e2ap_asn1_packer.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "srsran/e2/e2_factory.h"
#include "srsran/e2/e2ap_configuration_helpers.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/test_utils.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>

using namespace srsran;

class dummy_ric_e2 : public e2_message_handler, public e2_event_handler
{
public:
  dummy_ric_e2(e2_message_notifier& e2_pdu_notifier_) :
    logger(srslog::fetch_basic_logger("E2-RIC")), pdu_notifier(e2_pdu_notifier_){};

  /// E2_event_ handler functions.
  void handle_connection_loss() override{};

  /// E2 message handler functions.
  void handle_message(const e2_message& msg) override
  {
    logger.info("RIC received msg.");
    last_e2_msg = msg;
  };

  // RIC sends msg to E2 Agent
  void send_msg(const e2_message& msg)
  {
    logger.info("RIC sends msg.");
    pdu_notifier.on_new_message(msg);
  };

  void send_setup_response()
  {
    logger.info("RIC sends Setup Response msg.");
    e2_message e2_setup_response = {};
    e2_setup_response.pdu.set_successful_outcome();
    e2_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);

    auto& setup                           = e2_setup_response.pdu.successful_outcome().value.e2setup_resp();
    setup->transaction_id                 = last_e2_msg.pdu.init_msg().value.e2setup_request()->transaction_id;
    setup->ra_nfunctions_accepted_present = true;
    asn1::protocol_ie_single_container_s<asn1::e2ap::ra_nfunction_id_item_ies_o> ran_func_item;
    ran_func_item.value().ra_nfunction_id_item().ran_function_id       = e2sm_kpm_asn1_packer::ran_func_id;
    ran_func_item.value().ra_nfunction_id_item().ran_function_revision = 0;
    setup->ra_nfunctions_accepted.value.push_back(ran_func_item);
    setup->global_ric_id.value.plmn_id.from_number(1);
    setup->global_ric_id.value.ric_id.from_number(1);

    // fill the required part with dummy data
    setup->e2node_component_cfg_addition_ack.value.resize(1);
    asn1::e2ap::e2node_component_cfg_addition_ack_item_s& e2node_component_cfg_addition_ack_item =
        setup->e2node_component_cfg_addition_ack.value[0].value().e2node_component_cfg_addition_ack_item();
    e2node_component_cfg_addition_ack_item.e2node_component_interface_type =
        asn1::e2ap::e2node_component_interface_type_e::e2node_component_interface_type_opts::e1;
    e2node_component_cfg_addition_ack_item.e2node_component_id.set_e2node_component_interface_type_e1().gnb_cu_cp_id =
        123;
    e2node_component_cfg_addition_ack_item.e2node_component_cfg_ack.upd_outcome =
        asn1::e2ap::e2node_component_cfg_ack_s::upd_outcome_opts::success;

    send_msg(e2_setup_response);
  };

  e2_message last_e2_msg;

private:
  srslog::basic_logger& logger;
  e2_message_notifier&  pdu_notifier;
};

class e2_decorator : public e2_message_handler, public e2_event_handler
{
public:
  e2_decorator(e2_interface& decorated_iface_) :
    logger(srslog::fetch_basic_logger("E2")),
    decorated_e2_mgs_handler(decorated_iface_),
    decorated_e2_event_handler(decorated_iface_){};

  /// E2_event_ handler functions.
  void handle_connection_loss() override { decorated_e2_event_handler.handle_connection_loss(); };

  /// E2 message handler functions.
  void handle_message(const e2_message& msg) override
  {
    logger.info("E2 received msg.");
    last_e2_msg = msg;
    decorated_e2_mgs_handler.handle_message(msg);
  };

  e2_message last_e2_msg;

private:
  srslog::basic_logger& logger;
  e2_message_handler&   decorated_e2_mgs_handler;
  e2_event_handler&     decorated_e2_event_handler;
};

class e2ap_network_adapter_test : public ::testing::Test
{
protected:
  void tick()
  {
    timers.tick();
    task_exec.run_pending_tasks();
  }

  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    epoll_broker = create_io_broker(io_broker_type::epoll);

    // simulate RIC side
    sctp_network_gateway_config ric_config;
    ric_config.bind_address = "127.0.0.1";
    ric_config.bind_port    = 0;
    ric_config.reuse_addr   = true;

    ric_pcap        = std::make_unique<dummy_e2ap_pcap>();
    ric_net_adapter = std::make_unique<e2ap_network_adapter>(*epoll_broker, *ric_pcap);
    ric_gw          = create_sctp_network_gateway({ric_config, *ric_net_adapter, *ric_net_adapter});
    ric_net_adapter->bind_and_listen(std::move(ric_gw));
    ric_e2_iface = std::make_unique<dummy_ric_e2>(*ric_net_adapter);
    ric_net_adapter->connect_e2ap(ric_e2_iface.get(), ric_e2_iface.get());

    optional<uint16_t> ric_gw_port = ric_net_adapter->get_listen_port();
    ASSERT_TRUE(ric_gw_port.has_value());
    ASSERT_NE(ric_gw_port.value(), 0);

    // create E2 agent
    sctp_network_gateway_config e2agent_config;
    e2agent_config.connect_address = ric_config.bind_address;
    e2agent_config.connect_port    = ric_gw_port.value();
    e2agent_config.bind_address    = "127.0.0.101";
    e2agent_config.bind_port       = 0;

    cfg                  = srsran::config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    pcap        = std::make_unique<dummy_e2ap_pcap>();
    net_adapter = std::make_unique<e2ap_network_adapter>(*epoll_broker, *pcap);
    e2_agent_gw = create_sctp_network_gateway({e2agent_config, *net_adapter, *net_adapter});
    net_adapter->connect_gateway(std::move(e2_agent_gw));
    du_metrics       = std::make_unique<dummy_e2_du_metrics>();
    du_meas_provider = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_packer      = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    e2sm_iface       = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *du_meas_provider);
    e2sm_mngr        = std::make_unique<e2sm_manager>(test_logger);
    e2sm_mngr->add_e2sm_service("1.3.6.1.4.1.53148.1.2.2.2", std::move(e2sm_iface));
    e2_subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*net_adapter, *e2sm_mngr);
    factory              = timer_factory{timers, task_exec};
    e2ap          = create_e2_with_task_exec(cfg, factory, *net_adapter, *e2_subscription_mngr, *e2sm_mngr, task_exec);
    e2ap_rx_probe = std::make_unique<e2_decorator>(*e2ap);
    net_adapter->connect_e2ap(e2ap_rx_probe.get(), e2ap_rx_probe.get());
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();

    net_adapter->disconnect_gateway();
    ric_net_adapter->disconnect_gateway();
  }

  std::unique_ptr<io_broker> epoll_broker;
  // dummy RIC
  std::unique_ptr<dummy_ric_e2>         ric_e2_iface;
  std::unique_ptr<dummy_e2ap_pcap>      ric_pcap;
  std::unique_ptr<e2ap_asn1_packer>     ric_packer;
  std::unique_ptr<e2ap_network_adapter> ric_net_adapter;
  std::unique_ptr<sctp_network_gateway> ric_gw;

  // E2 agent
  e2ap_configuration                       cfg;
  timer_factory                            factory;
  timer_manager                            timers;
  std::unique_ptr<e2ap_network_adapter>    net_adapter;
  manual_task_worker                       task_exec{128};
  std::unique_ptr<sctp_network_gateway>    e2_agent_gw;
  std::unique_ptr<dummy_e2ap_pcap>         pcap;
  std::unique_ptr<e2_subscription_manager> e2_subscription_mngr;
  std::unique_ptr<e2sm_handler>            e2sm_packer;
  std::unique_ptr<e2sm_manager>            e2sm_mngr;
  std::unique_ptr<e2_du_metrics_interface> du_metrics;
  std::unique_ptr<e2sm_kpm_meas_provider>  du_meas_provider;
  std::unique_ptr<e2sm_interface>          e2sm_iface;
  std::unique_ptr<e2_interface>            e2ap;
  std::unique_ptr<e2_decorator>            e2ap_rx_probe;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test successful e2 setup procedure
TEST_F(e2ap_network_adapter_test, when_e2_setup_response_received_then_ric_connected)
{
  // Action 1: Launch E2 setup procedure (sends E2 Setup Request to RIC).
  test_logger.info("Launching E2 setup procedure...");
  e2ap->start();

  std::this_thread::sleep_for(std::chrono::seconds(1));
  // Status: RIC received E2 Setup Request.
  ASSERT_EQ(ric_e2_iface->last_e2_msg.pdu.type().value, asn1::e2ap::e2_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(ric_e2_iface->last_e2_msg.pdu.init_msg().value.type().value,
            asn1::e2ap::e2_ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Action 2: RIC sends E2 Setup Request Response.
  test_logger.info("Injecting E2SetupResponse");
  ric_e2_iface->send_setup_response();

  tick();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Status: E2 Agent received E2 Setup Request Response.
  ASSERT_EQ(e2ap_rx_probe->last_e2_msg.pdu.type().value, asn1::e2ap::e2_ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(e2ap_rx_probe->last_e2_msg.pdu.successful_outcome().value.type().value,
            asn1::e2ap::e2_ap_elem_procs_o::successful_outcome_c::types_opts::e2setup_resp);

  tick();
  test_logger.info("Test finished.");
}
