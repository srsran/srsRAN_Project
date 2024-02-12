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

#include "apps/gnb/adapters/e2_gateway_remote_connector.h"
#include "lib/e2/common/e2ap_asn1_packer.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "srsran/e2/e2_factory.h"
#include "srsran/e2/e2ap_configuration_helpers.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/test_utils.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>

using namespace srsran;

/// This test is an integration test between:
/// * E2AP (including ASN1 packer and E2 setup procedure)
/// * SCTP network gateway
/// * IO broker
class dummy_e2ap_network_adapter : public e2_message_notifier,
                                   public e2_message_handler,
                                   public sctp_network_gateway_control_notifier,
                                   public network_gateway_data_notifier
{
public:
  dummy_e2ap_network_adapter(const sctp_network_gateway_config& nw_config_) :
    nw_config(nw_config_),
    epoll_broker(create_io_broker(io_broker_type::epoll)),
    gw(create_sctp_network_gateway({nw_config, *this, *this})),
    packer(*gw, *this, pcap)
  {
    gw->create_and_connect();
    bool success = epoll_broker->register_fd(gw->get_socket_fd(), [this](int fd) { gw->receive(); });
    if (!success) {
      report_fatal_error("Failed to register E2 (SCTP) network gateway at IO broker. socket_fd={}",
                         gw->get_socket_fd());
    }
  }

  ~dummy_e2ap_network_adapter() {}

  void connect_e2ap(e2_interface* e2ap_) { e2ap = e2ap_; }

  e2_message last_e2_msg;

private:
  // E2AP calls interface to send (unpacked) E2AP PDUs
  void on_new_message(const e2_message& msg) override
  {
    last_e2_msg = msg;
    packer.handle_message(msg);
  }

  // SCTP network gateway calls interface to inject received PDUs (ASN1 packed)
  void on_new_pdu(byte_buffer pdu) override { packer.handle_packed_pdu(pdu); }

  // The packer calls this interface to inject unpacked E2AP PDUs
  void handle_message(const e2_message& msg) override
  {
    last_e2_msg = msg;
    e2ap->handle_message(msg);
  }

  /// \brief Simply log those events for now
  void on_connection_loss() override { test_logger.info("on_connection_loss"); }
  void on_connection_established() override { test_logger.info("on_connection_established"); }

  /// We require a network gateway and a packer
  const sctp_network_gateway_config&    nw_config;
  std::unique_ptr<io_broker>            epoll_broker;
  std::unique_ptr<sctp_network_gateway> gw;
  e2ap_asn1_packer                      packer;
  e2_interface*                         e2ap = nullptr;
  dummy_e2ap_pcap                       pcap;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

class e2ap_integration_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg                  = srsran::config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    sctp_network_gateway_config nw_config;
    nw_config.connection_name = "NearRT-RIC";
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
    e2_subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*adapter, *e2sm_mngr);
    factory              = timer_factory{timers, ctrl_worker};
    e2ap                 = create_e2(cfg, factory, *adapter, *e2_subscription_mngr, *e2sm_mngr);
    pcap                 = std::make_unique<dummy_e2ap_pcap>();
    adapter->connect_e2ap(e2ap.get());
  }

  e2ap_configuration                          cfg;
  timer_factory                               factory;
  timer_manager                               timers;
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
  srslog::basic_logger&                       test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test successful e2 setup procedure
TEST_F(e2ap_integration_test, when_e2_setup_response_received_then_ric_connected)
{
  // Action 1: Launch E2 setup procedure
  test_logger.info("Launching E2 setup procedure...");
  async_task<e2_setup_response_message>         t = e2ap->start_initial_e2_setup_routine();
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Status: received E2 Setup Request.
  ASSERT_EQ(adapter->last_e2_msg.pdu.type().value, asn1::e2ap::e2_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(adapter->last_e2_msg.pdu.init_msg().value.type().value,
            asn1::e2ap::e2_ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  ASSERT_EQ(adapter->last_e2_msg.pdu.type().value, asn1::e2ap::e2_ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(adapter->last_e2_msg.pdu.init_msg().value.type().value,
            asn1::e2ap::e2_ap_elem_procs_o::successful_outcome_c::types_opts::e2setup_resp);
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

    cfg                  = srsran::config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;
    cfg.gnb_id           = 123;

    sctp_network_gateway_config nw_config;
    nw_config.connection_name = "NearRT-RIC";
    nw_config.connect_address = "127.0.0.1";
    nw_config.connect_port    = 36421;
    nw_config.bind_address    = "127.0.0.101";
    nw_config.bind_port       = 0;

    epoll_broker          = create_io_broker(io_broker_type::epoll);
    factory               = timer_factory{timers, ctrl_worker};
    pcap                  = std::make_unique<dummy_e2ap_pcap>();
    du_metrics            = std::make_unique<dummy_e2_du_metrics>();
    f1ap_ue_id_mapper     = std::make_unique<dummy_f1ap_ue_id_translator>();
    e2_client             = std::make_unique<e2_gateway_remote_connector>(*epoll_broker, nw_config, *pcap);
    du_param_configurator = std::make_unique<dummy_du_configurator>();
    e2ap                  = create_e2_entity(
        cfg, e2_client.get(), *du_metrics, *f1ap_ue_id_mapper, *du_param_configurator, factory, ctrl_worker);
  }

  e2ap_configuration                           cfg;
  std::unique_ptr<io_broker>                   epoll_broker;
  timer_manager                                timers;
  manual_task_worker                           ctrl_worker{128};
  timer_factory                                factory;
  std::unique_ptr<dummy_e2ap_pcap>             pcap;
  std::unique_ptr<e2_du_metrics_interface>     du_metrics;
  std::unique_ptr<dummy_f1ap_ue_id_translator> f1ap_ue_id_mapper;
  std::unique_ptr<du_configurator>             du_param_configurator;
  std::unique_ptr<e2_gateway_remote_connector> e2_client;
  std::unique_ptr<e2_interface>                e2ap;
  srslog::basic_logger&                        test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test successful e2 setup procedure
TEST_F(e2ap_gw_connector_integration_test, when_e2_setup_response_received_then_ric_connected)
{
  // Action 1: Launch E2 setup procedure
  test_logger.info("Launching E2 setup procedure...");
  e2ap->start();

  std::this_thread::sleep_for(std::chrono::seconds(3));

  this->tick();
}
