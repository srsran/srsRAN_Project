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

#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "lib/ngap/ngap_asn1_helpers.h"
#include "lib/ngap/ngap_asn1_packer.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "srsran/ngap/ngap_configuration_helpers.h"
#include "srsran/ngap/ngap_factory.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/test_utils.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// This test is an integration test between:
/// * NGAP (including ASN1 packer and NG setup procedure)
/// * SCTP network gateway
/// * IO broker
class ngap_network_adapter : public ngap_message_notifier,
                             public ngap_message_handler,
                             public sctp_network_gateway_control_notifier,
                             public network_gateway_data_notifier
{
public:
  ngap_network_adapter(const sctp_network_gateway_config& nw_config_) :
    nw_config(nw_config_),
    epoll_broker(create_io_broker(io_broker_type::epoll)),
    gw(create_sctp_network_gateway({nw_config, *this, *this})),
    packer(*gw, *this, *this, pcap)
  {
    gw->create_and_connect();
    bool success = epoll_broker->register_fd(gw->get_socket_fd(), [this](int fd) { gw->receive(); });
    if (!success) {
      report_fatal_error("Failed to register N2 (SCTP) network gateway at IO broker. socket_fd={}",
                         gw->get_socket_fd());
    }
  }

  ~ngap_network_adapter() {}

  void connect_ngap(ngap_interface* ngap_) { ngap = ngap_; }

private:
  // NGAP calls interface to send (unpacked) NGAP PDUs
  void on_new_message(const ngap_message& msg) override { packer.handle_message(msg); }

  // SCTP network gateway calls interface to inject received PDUs (ASN1 packed)
  void on_new_pdu(byte_buffer pdu) override { packer.handle_packed_pdu(pdu); }

  // The packer calls this interface to inject unpacked NGAP PDUs
  void handle_message(const ngap_message& msg) override { ngap->handle_message(msg); }

  /// \brief Simply log those events for now
  void on_connection_loss() override { test_logger.info("on_connection_loss"); }
  void on_connection_established() override { test_logger.info("on_connection_established"); }

  /// We require a network gateway and a packer
  const sctp_network_gateway_config&    nw_config;
  std::unique_ptr<io_broker>            epoll_broker;
  std::unique_ptr<sctp_network_gateway> gw;
  ngap_asn1_packer                      packer;
  ngap_interface*                       ngap = nullptr;
  null_dlt_pcap                         pcap;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

class ngap_integration_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg.gnb_id        = 411;
    cfg.ran_node_name = "srsgnb01";
    cfg.plmn          = "00101";
    cfg.tac           = 7;
    s_nssai_t slice_cfg;
    slice_cfg.sst = 1;
    cfg.slice_configurations.push_back(slice_cfg);

    sctp_network_gateway_config nw_config;
    nw_config.connection_name   = "AMF";
    nw_config.connect_address   = "10.12.1.105";
    nw_config.connect_port      = 38412;
    nw_config.bind_address      = "10.8.1.10";
    nw_config.bind_port         = 0;
    nw_config.non_blocking_mode = true;
    adapter                     = std::make_unique<ngap_network_adapter>(nw_config);

    ngap_ue_task_scheduler = std::make_unique<dummy_ngap_ue_task_scheduler>(timers, ctrl_worker);

    ngap = create_ngap(
        cfg, ngap_ue_creation_notifier, cu_cp_paging_notifier, *ngap_ue_task_scheduler, ue_mng, *adapter, ctrl_worker);
    adapter->connect_ngap(ngap.get());
  }

  ngap_configuration                            cfg;
  ue_configuration                              ue_config;
  up_resource_manager_cfg                       up_config;
  timer_manager                                 timers;
  manual_task_worker                            ctrl_worker{128};
  ue_manager                                    ue_mng{ue_config, up_config, timers, ctrl_worker};
  dummy_ngap_cu_cp_ue_creation_notifier         ngap_ue_creation_notifier{ue_mng};
  dummy_ngap_cu_cp_paging_notifier              cu_cp_paging_notifier;
  std::unique_ptr<dummy_ngap_ue_task_scheduler> ngap_ue_task_scheduler;
  std::unique_ptr<ngap_network_adapter>         adapter;
  std::unique_ptr<ngap_interface>               ngap;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

ngap_ng_setup_request generate_ng_setup_request(ngap_configuration ngap_cfg)
{
  ngap_ng_setup_request request_msg = {};

  ngap_ng_setup_request request;

  // fill global ran node id
  request.global_ran_node_id.gnb_id  = ngap_cfg.gnb_id;
  request.global_ran_node_id.plmn_id = ngap_cfg.plmn;
  // fill ran node name
  request.ran_node_name = ngap_cfg.ran_node_name;
  // fill supported ta list
  // TODO: add support for more items
  ngap_supported_ta_item supported_ta_item;

  ngap_broadcast_plmn_item broadcast_plmn_item;
  broadcast_plmn_item.plmn_id = ngap_cfg.plmn;

  for (const auto& slice_config : ngap_cfg.slice_configurations) {
    slice_support_item_t slice_support_item;
    slice_support_item.s_nssai.sst = slice_config.sst;
    if (slice_config.sd.has_value()) {
      slice_support_item.s_nssai.sd = slice_config.sd.value();
    }
    broadcast_plmn_item.tai_slice_support_list.push_back(slice_support_item);
  }

  supported_ta_item.broadcast_plmn_list.push_back(broadcast_plmn_item);
  supported_ta_item.tac = ngap_cfg.tac;

  request.supported_ta_list.push_back(supported_ta_item);

  // fill paging drx
  request.default_paging_drx = 256;

  return request_msg;
}

/// Test successful ng setup procedure
TEST_F(ngap_integration_test, when_ng_setup_response_received_then_amf_connected)
{
  // Action 1: Launch NG setup procedure
  ngap_configuration    ngap_cfg    = srsran::config_helpers::make_default_ngap_config();
  ngap_ng_setup_request request_msg = generate_ng_setup_request(ngap_cfg);

  test_logger.info("Launching NG setup procedure...");
  async_task<ngap_ng_setup_result>         t = ngap->handle_ng_setup_request(request_msg);
  lazy_task_launcher<ngap_ng_setup_result> t_launcher(t);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  std::this_thread::sleep_for(std::chrono::seconds(3));
}
