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

#include "dummy_ric.h"
#include "lib/e2/common/e2ap_asn1_packer.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/e2/e2_factory.h"
#include "srsran/e2/e2ap_configuration_helpers.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/e2/gateways/e2_network_client_factory.h"
#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "srsran/gateways/sctp_network_server_factory.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/timers.h"
#include <chrono>
#include <condition_variable>
#include <gtest/gtest.h>
#include <list>
#include <mutex>
#include <utility>

using namespace srsran;

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

    ric_broker   = create_io_broker(io_broker_type::epoll);
    agent_broker = create_io_broker(io_broker_type::epoll);

    // simulate RIC side
    ric_rx_e2_sniffer = std::make_unique<e2_sniffer>(*this);
    ric_pcap          = std::make_unique<dummy_e2ap_pcap>();
    ric_sctp_gateway_config ric_server_sctp_cfg{{}, *ric_broker, rx_executor, *ric_pcap};
    ric_server_sctp_cfg.sctp.if_name      = "E2";
    ric_server_sctp_cfg.sctp.ppid         = NGAP_PPID;
    ric_server_sctp_cfg.sctp.bind_address = "127.0.0.1";
    ric_server_sctp_cfg.sctp.bind_port    = 0;
    ric_server_sctp_cfg.rx_sniffer        = ric_rx_e2_sniffer.get();

    ric_net_adapter = create_e2_gateway_server(ric_server_sctp_cfg);
    ric             = std::make_unique<near_rt_ric>();
    ric_net_adapter->attach_ric(ric->get_ric_e2_handler());

    std::optional<uint16_t> ric_gw_port = ric_net_adapter->get_listen_port();
    ASSERT_TRUE(ric_gw_port.has_value());
    ASSERT_NE(ric_gw_port.value(), 0);

    // create E2 agent
    sctp_network_connector_config e2agent_config;
    e2agent_config.dest_name       = "NearRT-RIC";
    e2agent_config.if_name         = "E2";
    e2agent_config.connect_address = "127.0.0.1";
    e2agent_config.connect_port    = ric_gw_port.value();
    e2agent_config.bind_address    = "127.0.0.101";
    e2agent_config.bind_port       = 0;

    cfg                  = config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    pcap      = std::make_unique<dummy_e2ap_pcap>();
    e2_client = create_e2_gateway_client(
        e2_sctp_gateway_config{e2agent_config, *agent_broker, rx_executor, *pcap, srslog::fetch_basic_logger("E2")});
    e2_client_wrapper = std::make_unique<e2_connection_client_wrapper>(*e2_client);
    du_metrics        = std::make_unique<dummy_e2_du_metrics>();
    du_meas_provider  = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_packer       = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    e2sm_iface        = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *du_meas_provider);
    e2sm_mngr         = std::make_unique<e2sm_manager>(test_logger);
    e2sm_mngr->add_e2sm_service("1.3.6.1.4.1.53148.1.2.2.2", std::move(e2sm_iface));
    e2_subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*e2sm_mngr);
    factory              = timer_factory{timers, task_exec};
    e2agent_notifier     = std::make_unique<dummy_e2_agent_mng>();
    e2ap = create_e2(cfg, *e2agent_notifier, factory, *e2_client_wrapper, *e2_subscription_mngr, *e2sm_mngr, task_exec);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  class e2_msg_notifier_wrapper : public e2_message_notifier
  {
  public:
    using E2MsgCallback = std::function<void(const e2_message&)>;
    e2_msg_notifier_wrapper(e2_message_notifier& notifier_, unique_task on_disconnect_, E2MsgCallback callback) :
      notifier(notifier_), on_disconnect(std::move(on_disconnect_)), on_message_callback(std::move(callback))
    {
    }
    ~e2_msg_notifier_wrapper() override { on_disconnect(); }

    void on_new_message(const e2_message& msg) override
    {
      if (on_message_callback) {
        on_message_callback(msg);
      }
      notifier.on_new_message(msg);
    }

    e2_message_notifier& notifier;
    unique_task          on_disconnect;
    E2MsgCallback        on_message_callback;
  };

  class e2_connection_client_wrapper : public e2_connection_client
  {
  public:
    e2_connection_client_wrapper(e2_connection_client& e2_client_) : e2_client(e2_client_) {}

    void on_e2_message_rx(const e2_message& msg)
    {
      std::lock_guard<std::mutex> lock(rx_mutex);
      e2_msg_queue.push_back(msg);
      rx_cvar.notify_one();
    }

    expected<e2_message> get_last_e2_msg(std::chrono::milliseconds timeout_ms = std::chrono::milliseconds(5000))
    {
      // wait until E2 msg is received
      std::unique_lock<std::mutex> lock(rx_mutex);
      if (rx_cvar.wait_for(lock, timeout_ms, [this] { return !e2_msg_queue.empty(); })) {
        e2_message msg = std::move(e2_msg_queue.front());
        e2_msg_queue.pop_front();
        return msg;
      }
      return make_unexpected(default_error_t{});
    }

    std::unique_ptr<e2_message_notifier>
    handle_e2_connection_request(std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier_) override
    {
      e2_rx_msg_notifier = std::move(e2_rx_pdu_notifier_);
      return e2_client.handle_e2_connection_request(std::make_unique<e2_msg_notifier_wrapper>(
          *e2_rx_msg_notifier,
          [this]() { e2_rx_msg_notifier.reset(); },
          [this](const e2_message& msg) { this->on_e2_message_rx(msg); }));
    }

  private:
    e2_connection_client&                e2_client;
    std::unique_ptr<e2_message_notifier> e2_rx_msg_notifier;
    std::unique_ptr<e2_message_notifier> e2_tx_msg_notifier;

    // sniff last rx msg
    std::mutex              rx_mutex;
    std::condition_variable rx_cvar;
    std::list<e2_message>   e2_msg_queue;
  };

  class e2_sniffer : public e2_message_notifier
  {
  public:
    e2_sniffer(e2ap_network_adapter_test& parent_) : logger(srslog::fetch_basic_logger("E2")) {}

    /// E2 message handler functions.
    void on_new_message(const e2_message& msg) override
    {
      {
        std::lock_guard<std::mutex> lock(rx_mutex);
        e2_msg_queue.push_back(msg);
      }
      rx_cvar.notify_one();
    }

    expected<e2_message> get_last_e2_msg(std::chrono::milliseconds timeout_ms = std::chrono::milliseconds(5000))
    {
      // wait until E2 msg is received
      std::unique_lock<std::mutex> lock(rx_mutex);
      if (rx_cvar.wait_for(lock, timeout_ms, [this] { return !e2_msg_queue.empty(); })) {
        e2_message msg = std::move(e2_msg_queue.front());
        e2_msg_queue.pop_front();
        return msg;
      }
      return make_unexpected(default_error_t{});
    }

  private:
    srslog::basic_logger&   logger;
    std::mutex              rx_mutex;
    std::condition_variable rx_cvar;
    std::list<e2_message>   e2_msg_queue;
  };

  inline_task_executor                              rx_executor;
  std::unique_ptr<io_broker>                        ric_broker;
  std::unique_ptr<io_broker>                        agent_broker;
  std::unique_ptr<sctp_network_association_factory> assoc_factory;

  // dummy RIC
  std::unique_ptr<near_rt_ric>          ric;
  std::unique_ptr<dummy_e2ap_pcap>      ric_pcap;
  std::unique_ptr<e2ap_asn1_packer>     ric_packer;
  std::unique_ptr<e2_connection_server> ric_net_adapter;
  std::unique_ptr<e2_sniffer>           ric_rx_e2_sniffer;

  // E2 agent
  e2ap_configuration                            cfg;
  timer_factory                                 factory;
  timer_manager                                 timers;
  std::unique_ptr<e2ap_e2agent_notifier>        e2agent_notifier;
  std::unique_ptr<e2_connection_client>         e2_client;
  std::unique_ptr<e2_connection_client_wrapper> e2_client_wrapper;
  std::unique_ptr<e2_message_notifier>          e2_tx_channel;
  manual_task_worker                            task_exec{128};
  std::unique_ptr<dummy_e2ap_pcap>              pcap;
  std::unique_ptr<e2_subscription_manager>      e2_subscription_mngr;
  std::unique_ptr<e2sm_handler>                 e2sm_packer;
  std::unique_ptr<e2sm_manager>                 e2sm_mngr;
  std::unique_ptr<e2_du_metrics_interface>      du_metrics;
  std::unique_ptr<e2sm_kpm_meas_provider>       du_meas_provider;
  std::unique_ptr<e2sm_interface>               e2sm_iface;
  std::unique_ptr<e2_interface>                 e2ap;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test successful e2 setup procedure
TEST_F(e2ap_network_adapter_test, when_e2_setup_response_received_then_ric_connected)
{
  report_fatal_error_if_not(e2ap->handle_e2_tnl_connection_request(), "Unable to establish connection to RIC");

  // Action 1: Launch E2 setup procedure (sends E2 Setup Request to RIC).
  test_logger.info("Launching E2 setup procedure...");
  async_task<e2_setup_response_message>         t1 = e2ap->start_initial_e2_setup_routine();
  lazy_task_launcher<e2_setup_response_message> t1_launcher(t1);
  ASSERT_FALSE(t1.ready());

  // Status: RIC received E2 Setup Request.
  auto last_e2_msg = ric_rx_e2_sniffer->get_last_e2_msg();
  ASSERT_TRUE(last_e2_msg.has_value());
  ASSERT_EQ(last_e2_msg.value().pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(last_e2_msg.value().pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Action 2: RIC sends E2 Setup Request Response.
  test_logger.info("Injecting E2SetupResponse");
  e2_message e2_setup_response = {};
  e2_setup_response.pdu.set_successful_outcome();
  e2_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  auto& setup                           = e2_setup_response.pdu.successful_outcome().value.e2setup_resp();
  setup->transaction_id                 = last_e2_msg.value().pdu.init_msg().value.e2setup_request()->transaction_id;
  setup->ran_functions_accepted_present = true;
  asn1::protocol_ie_single_container_s<asn1::e2ap::ran_function_id_item_ies_o> ran_func_item;
  ran_func_item.value().ran_function_id_item().ran_function_id       = e2sm_kpm_asn1_packer::ran_func_id;
  ran_func_item.value().ran_function_id_item().ran_function_revision = 0;
  setup->ran_functions_accepted.push_back(ran_func_item);
  setup->global_ric_id.plmn_id.from_number(1);
  setup->global_ric_id.ric_id.from_number(1);
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

  ric->send_msg(0, e2_setup_response);

  // Status: E2 Agent received E2 Setup Request Response.
  auto last_e2_msg2 = e2_client_wrapper->get_last_e2_msg();
  ASSERT_TRUE(last_e2_msg2.has_value());
  ASSERT_EQ(last_e2_msg2.value().pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(last_e2_msg2.value().pdu.successful_outcome().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::successful_outcome_c::types_opts::e2setup_resp);

  async_task<void>         t2 = e2ap->handle_e2_disconnection_request();
  lazy_task_launcher<void> t2_launcher(t2);
  while (not t2.ready()) {
    this->tick();
  }

  test_logger.info("Test finished.");
}
