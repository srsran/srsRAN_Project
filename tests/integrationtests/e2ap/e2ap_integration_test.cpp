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

#include "lib/e2/common/e2_impl.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "tests/unittests/e2/common/e2ap_asn1_packer.h"
#include "srsran/e2/e2ap_configuration_helpers.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/e2/gateways/e2_network_client_factory.h"
#include "srsran/gateways/sctp_network_client_factory.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>

using namespace srsran;

/// This test is an integration test between:
/// * E2AP (E2 setup procedure)
/// * SCTP network client
/// * IO broker
/// It runs with an external nearRT-RIC.
class my_dummy_sctp_association_sdu_notifier : public sctp_association_sdu_notifier
{
public:
  my_dummy_sctp_association_sdu_notifier(sctp_association_sdu_notifier& parent_) : parent(parent_) {}
  bool on_new_sdu(byte_buffer pdu) override { return parent.on_new_sdu(std::move(pdu)); }

private:
  sctp_association_sdu_notifier& parent;
};

class dummy_e2ap_network_adapter : public e2_message_notifier,
                                   public sctp_network_gateway_control_notifier,
                                   public sctp_association_sdu_notifier
{
public:
  dummy_e2ap_network_adapter(const sctp_network_connector_config& nw_config_) :
    nw_config(nw_config_),
    epoll_broker(create_io_broker(io_broker_type::epoll)),
    gw(create_sctp_network_client({nw_config, *epoll_broker, executor}))
  {
    sctp_sender = gw->connect_to("RIC",
                                 nw_config.connect_address,
                                 nw_config.connect_port,
                                 std::make_unique<my_dummy_sctp_association_sdu_notifier>(*this));
    if (sctp_sender == nullptr) {
      test_logger.error("Failed to establish E2 connection to Near-RT RIC on {}:{}.",
                        nw_config.connect_address,
                        nw_config.connect_port);
      return;
    }
  }

  void connect_e2ap(e2_interface* e2ap_) { e2ap = e2ap_; }

  e2_message last_tx_e2_msg;
  e2_message last_rx_e2_msg;

private:
  // E2AP calls interface to send (unpacked) E2AP PDUs
  void on_new_message(const e2_message& msg) override
  {
    last_tx_e2_msg = msg;
    // pack E2AP PDU into SCTP SDU.
    byte_buffer   tx_sdu{byte_buffer::fallback_allocation_tag{}};
    asn1::bit_ref bref(tx_sdu);
    if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
      test_logger.error("Failed to pack E2AP PDU");
      return;
    }

    // Forward packed Tx PDU to SCTP gateway.
    sctp_sender->on_new_sdu(std::move(tx_sdu));
  }

  // SCTP network gateway calls interface to inject received PDUs (ASN1 packed)
  bool on_new_sdu(byte_buffer pdu) override
  {
    // Unpack E2AP PDU.
    asn1::cbit_ref bref(pdu);
    e2_message     msg;
    if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
      test_logger.error("Couldn't unpack E2AP PDU");
      return false;
    }

    // Forward unpacked Rx PDU to the DU.
    last_rx_e2_msg = msg;
    e2ap->handle_message(msg);
    return true;
  }

  /// \brief Simply log those events for now
  void on_connection_loss() override { test_logger.info("on_connection_loss"); }
  void on_connection_established() override { test_logger.info("on_connection_established"); }

  /// We require a network gateway and a packer
  const sctp_network_connector_config&           nw_config;
  inline_task_executor                           executor;
  std::unique_ptr<io_broker>                     epoll_broker;
  std::unique_ptr<sctp_network_client>           gw;
  e2_interface*                                  e2ap = nullptr;
  std::unique_ptr<sctp_association_sdu_notifier> sctp_sender;
  srslog::basic_logger&                          test_logger = srslog::fetch_basic_logger("TEST");
};

class my_dummy_e2_pdu_tx_notifier : public e2_message_notifier
{
public:
  my_dummy_e2_pdu_tx_notifier(e2_message_notifier& e2_tx_notifier_, unique_task on_disconnect_) :
    e2_tx_notifier(e2_tx_notifier_), on_disconnect(std::move(on_disconnect_))
  {
  }
  ~my_dummy_e2_pdu_tx_notifier() override { on_disconnect(); }

  void on_new_message(const e2_message& msg) override { e2_tx_notifier.on_new_message(msg); }

  e2_message_notifier& e2_tx_notifier;
  unique_task          on_disconnect;
};

class dummy_e2_con_client final : public e2_connection_client
{
public:
  dummy_e2_con_client(e2_message_notifier& notif) : e2_tx_notifier(notif) {}

  std::unique_ptr<e2_message_notifier>
  handle_e2_connection_request(std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier_) override
  {
    e2_rx_pdu_notifier = std::move(e2_rx_pdu_notifier_);
    return std::make_unique<my_dummy_e2_pdu_tx_notifier>(e2_tx_notifier, []() {});
  }

private:
  e2_message_notifier&                 e2_tx_notifier;
  std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier;
};

class e2ap_integration_test : public ::testing::Test
{
protected:
  void tick()
  {
    timers.tick();
    ctrl_worker.run_pending_tasks();
  }

  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg                  = config_helpers::make_default_e2ap_config();
    cfg.gnb_du_id        = int_to_gnb_du_id(1);
    cfg.e2sm_kpm_enabled = true;

    sctp_network_connector_config nw_config;
    nw_config.dest_name       = "NearRT-RIC";
    nw_config.if_name         = "E2";
    nw_config.connect_address = "127.0.0.1";
    nw_config.connect_port    = 36421;
    nw_config.bind_address    = "127.0.0.101";
    nw_config.bind_port       = 0;

    adapter          = std::make_unique<dummy_e2ap_network_adapter>(nw_config);
    du_metrics       = std::make_unique<dummy_e2_du_metrics>();
    du_meas_provider = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_packer      = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    e2sm_iface       = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *du_meas_provider);
    e2sm_mngr        = std::make_unique<e2sm_manager>(test_logger);
    e2sm_mngr->add_e2sm_service("1.3.6.1.4.1.53148.1.2.2.2", std::move(e2sm_iface));
    e2_subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*e2sm_mngr);
    factory              = timer_factory{timers, ctrl_worker};
    e2_client            = std::make_unique<dummy_e2_con_client>(*adapter);
    e2agent_notifier     = std::make_unique<dummy_e2_agent_mng>();
    e2ap                 = std::make_unique<e2_impl>(
        test_logger, cfg, *e2agent_notifier, factory, *e2_client, *e2_subscription_mngr, *e2sm_mngr, ctrl_worker);
    pcap = std::make_unique<dummy_e2ap_pcap>();
    adapter->connect_e2ap(e2ap.get());
  }

  e2ap_configuration                          cfg = {};
  timer_factory                               factory;
  timer_manager                               timers;
  std::unique_ptr<e2ap_e2agent_notifier>      e2agent_notifier;
  std::unique_ptr<dummy_e2ap_network_adapter> adapter;
  manual_task_worker                          ctrl_worker{128};
  std::unique_ptr<dummy_e2ap_pcap>            pcap;
  std::unique_ptr<e2_subscription_manager>    e2_subscription_mngr;
  std::unique_ptr<e2sm_handler>               e2sm_packer;
  std::unique_ptr<e2_du_metrics_interface>    du_metrics;
  std::unique_ptr<e2sm_kpm_meas_provider>     du_meas_provider;
  std::unique_ptr<e2sm_manager>               e2sm_mngr;
  std::unique_ptr<e2sm_interface>             e2sm_iface;
  std::unique_ptr<e2_interface>               e2ap;
  std::unique_ptr<dummy_e2_con_client>        e2_client;
  srslog::basic_logger&                       test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test successful e2 setup procedure
TEST_F(e2ap_integration_test, when_e2_setup_response_received_then_ric_connected)
{
  report_fatal_error_if_not(e2ap->handle_e2_tnl_connection_request(), "Unable to establish connection to RIC");

  // Action 1: Launch E2 setup procedure
  test_logger.info("Launching E2 setup procedure...");
  async_task<e2_setup_response_message>         t = e2ap->start_initial_e2_setup_routine();
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Status: received E2 Setup Request.
  ASSERT_EQ(adapter->last_tx_e2_msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(adapter->last_tx_e2_msg.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  ASSERT_EQ(adapter->last_rx_e2_msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(adapter->last_rx_e2_msg.pdu.successful_outcome().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::successful_outcome_c::types_opts::e2setup_resp);
  e2ap->handle_e2_disconnection_request();
}

class e2ap_gw_connector_integration_test : public ::testing::Test
{
protected:
  void tick()
  {
    timers.tick();
    ctrl_worker.run_pending_tasks();
  }

  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg                  = config_helpers::make_default_e2ap_config();
    cfg.gnb_id           = {123, 22};
    cfg.gnb_du_id        = int_to_gnb_du_id(1);
    cfg.e2sm_kpm_enabled = true;

    sctp_network_connector_config nw_config;
    nw_config.dest_name       = "NearRT-RIC";
    nw_config.if_name         = "E2";
    nw_config.connect_address = "127.0.0.1";
    nw_config.connect_port    = 36421;
    nw_config.bind_address    = "127.0.0.101";
    nw_config.bind_port       = 0;

    epoll_broker      = create_io_broker(io_broker_type::epoll);
    factory           = timer_factory{timers, ctrl_worker};
    pcap              = std::make_unique<dummy_e2ap_pcap>();
    du_metrics        = std::make_unique<dummy_e2_du_metrics>();
    f1ap_ue_id_mapper = std::make_unique<dummy_f1ap_ue_id_translator>();
    e2_client         = create_e2_gateway_client(
        e2_sctp_gateway_config{nw_config, *epoll_broker, rx_executor, *pcap, srslog::fetch_basic_logger("E2")});
    du_param_configurator = std::make_unique<dummy_du_configurator>();
    e2agent               = create_e2_du_agent(
        cfg, *e2_client, &(*du_metrics), &(*f1ap_ue_id_mapper), &(*du_param_configurator), factory, ctrl_worker);
  }

  e2ap_configuration                           cfg = {};
  inline_task_executor                         rx_executor;
  std::unique_ptr<io_broker>                   epoll_broker;
  timer_manager                                timers;
  manual_task_worker                           ctrl_worker{128};
  timer_factory                                factory;
  std::unique_ptr<dummy_e2ap_pcap>             pcap;
  std::unique_ptr<e2_du_metrics_interface>     du_metrics;
  std::unique_ptr<dummy_f1ap_ue_id_translator> f1ap_ue_id_mapper;
  std::unique_ptr<srs_du::du_configurator>     du_param_configurator;
  std::unique_ptr<e2_connection_client>        e2_client;
  std::unique_ptr<e2_agent>                    e2agent;
  srslog::basic_logger&                        test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test successful e2 setup procedure, runs with an external nearRT-RIC.
TEST_F(e2ap_gw_connector_integration_test, when_e2_setup_response_received_then_ric_connected)
{
  // Action 1: Launch E2 setup procedure
  test_logger.info("Launching E2 setup procedure...");
  e2agent->start();

  std::this_thread::sleep_for(std::chrono::seconds(1));

  this->tick();
  e2agent->stop();
}
