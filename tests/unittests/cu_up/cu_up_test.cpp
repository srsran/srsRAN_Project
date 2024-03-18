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

#include "cu_up_test_helpers.h"
#include "lib/e1ap/cu_up/e1ap_cu_up_asn1_helpers.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/cu_up/cu_up_factory.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/test_utils.h"
#include <arpa/inet.h>
#include <chrono>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace srsran;
using namespace srs_cu_up;
using namespace asn1::e1ap;

/// This implementation returns back to the E1 interface a dummy CU-UP E1 Setup Response message upon the receival of
/// the CU-UP E1 Setup Request message.
class dummy_cu_cp_handler : public e1ap_connection_client
{
public:
  std::unique_ptr<e1ap_message_notifier>
  handle_cu_up_connection_request(std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier_) override
  {
    class dummy_du_tx_pdu_notifier : public e1ap_message_notifier
    {
    public:
      dummy_du_tx_pdu_notifier(dummy_cu_cp_handler& parent_) : parent(parent_) {}

      void on_new_message(const e1ap_message& msg) override
      {
        if (msg.pdu.type() != asn1::e1ap::e1ap_pdu_c::types::init_msg) {
          return;
        }

        e1ap_message response;
        if (msg.pdu.init_msg().value.type().value ==
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request) {
          // Generate a dummy CU-UP E1 Setup response message and pass it back to the CU-UP.
          response.pdu.set_successful_outcome();
          response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_GNB_CU_UP_E1_SETUP);

          auto& setup_res = response.pdu.successful_outcome().value.gnb_cu_up_e1_setup_resp();
          // Use the same transaction ID as in the request message.
          setup_res->transaction_id         = msg.pdu.init_msg().value.gnb_cu_up_e1_setup_request()->transaction_id;
          setup_res->gnb_cu_cp_name_present = true;
          setup_res->gnb_cu_cp_name.from_string("srsCU-CP");
        } else {
          // do nothing
          return;
        }

        // Send response to DU.
        parent.cu_up_rx_pdu_notifier->on_new_message(response);
      }

    private:
      dummy_cu_cp_handler& parent;
    };

    cu_up_rx_pdu_notifier = std::move(cu_up_rx_pdu_notifier_);

    return std::make_unique<dummy_du_tx_pdu_notifier>(*this);
  }

private:
  std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier;
};

/// Fixture class for CU-UP test
class cu_up_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    srslog::fetch_basic_logger("GTPU").set_level(srslog::basic_levels::debug);

    // create worker thread and executer
    worker   = std::make_unique<task_worker>("thread", 128, os_thread_realtime_priority::no_realtime());
    executor = make_task_executor_ptr(*worker);

    exec_pool    = std::make_unique<dummy_cu_up_executor_pool>(executor.get());
    app_timers   = std::make_unique<timer_manager>(256);
    f1u_gw       = std::make_unique<dummy_f1u_gateway>(f1u_bearer);
    broker       = create_io_broker(io_broker_type::epoll);
    upf_addr_str = "127.0.0.1";
  }

  cu_up_configuration get_default_cu_up_config()
  {
    // create config
    cu_up_configuration cfg;
    cfg.ctrl_executor                = executor.get();
    cfg.ue_exec_pool                 = exec_pool.get();
    cfg.io_ul_executor               = executor.get();
    cfg.e1ap.e1ap_conn_client        = &e1ap_client;
    cfg.f1u_gateway                  = f1u_gw.get();
    cfg.epoll_broker                 = broker.get();
    cfg.timers                       = app_timers.get();
    cfg.qos[uint_to_five_qi(9)]      = {};
    cfg.gtpu_pcap                    = &dummy_pcap;
    cfg.net_cfg.n3_bind_port         = 0; // Random free port selected by the OS.
    cfg.n3_cfg.gtpu_reordering_timer = std::chrono::milliseconds(0);
    cfg.n3_cfg.warn_on_drop          = false;
    cfg.statistics_report_period     = std::chrono::seconds(1);

    return cfg;
  }

  void init(const cu_up_configuration& cfg) { cu_up = create_cu_up(cfg); }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<timer_manager> app_timers;

  dummy_cu_cp_handler                         e1ap_client;
  dummy_inner_f1u_bearer                      f1u_bearer;
  std::unique_ptr<dummy_f1u_gateway>          f1u_gw;
  std::unique_ptr<io_broker>                  broker;
  std::unique_ptr<dummy_cu_up_executor_pool>  exec_pool;
  std::unique_ptr<srs_cu_up::cu_up_interface> cu_up;
  srslog::basic_logger&                       test_logger = srslog::fetch_basic_logger("TEST");

  std::unique_ptr<task_worker>   worker;
  std::unique_ptr<task_executor> executor;
  null_dlt_pcap                  dummy_pcap;

  std::string upf_addr_str;

  void create_drb()
  {
    // Generate BearerContextSetupRequest
    e1ap_message asn1_bearer_context_setup_msg = generate_bearer_context_setup_request(9);

    // Convert to common type
    e1ap_bearer_context_setup_request bearer_context_setup;
    fill_e1ap_bearer_context_setup_request(
        bearer_context_setup, asn1_bearer_context_setup_msg.pdu.init_msg().value.bearer_context_setup_request());

    // Setup bearer
    cu_up->handle_bearer_context_setup_request(bearer_context_setup);
  }
};

//////////////////////////////////////////////////////////////////////////////////////
/* E1AP connection handling                                                           */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the E1AP connection
TEST_F(cu_up_test, when_e1ap_connection_established_then_e1ap_connected)
{
  init(get_default_cu_up_config());

  // Connect E1AP
  cu_up->on_e1ap_connection_establish();

  // check that E1AP is in connected state
  ASSERT_TRUE(cu_up->e1ap_is_connected());
}

//////////////////////////////////////////////////////////////////////////////////////
/* User Data Flow                                                                   */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_up_test, dl_data_flow)
{
  cu_up_configuration cfg = get_default_cu_up_config();
  test_logger.debug("Using network_interface_config: {}", cfg.net_cfg);
  init(cfg);

  create_drb();

  int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  ASSERT_GE(sock_fd, 0);

  sockaddr_in cu_up_addr;
  cu_up_addr.sin_family      = AF_INET;
  cu_up_addr.sin_port        = htons(cu_up->get_n3_bind_port().value());
  cu_up_addr.sin_addr.s_addr = inet_addr(cfg.net_cfg.n3_bind_addr.c_str());

  // teid=2, qfi=1
  const uint8_t gtpu_ping_vec[] = {
      0x34, 0xff, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x85, 0x01, 0x00, 0x01, 0x00, 0x45,
      0x00, 0x00, 0x54, 0x9b, 0xfb, 0x00, 0x00, 0x40, 0x01, 0x56, 0x5a, 0xc0, 0xa8, 0x04, 0x01, 0xc0, 0xa8,
      0x03, 0x02, 0x00, 0x00, 0xb8, 0xc0, 0x00, 0x02, 0x00, 0x01, 0x5d, 0x26, 0x77, 0x64, 0x00, 0x00, 0x00,
      0x00, 0xb1, 0xde, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
      0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};

  int ret = 0;

  // send message 1
  ret = sendto(sock_fd, gtpu_ping_vec, sizeof(gtpu_ping_vec), 0, (sockaddr*)&cu_up_addr, sizeof(cu_up_addr));
  ASSERT_GE(ret, 0) << "Failed to send message via sock_fd=" << sock_fd << " to `" << cfg.net_cfg.n3_bind_addr << ":"
                    << cu_up->get_n3_bind_port().value() << "` - " << strerror(errno);

  // send message 2
  ret = sendto(sock_fd, gtpu_ping_vec, sizeof(gtpu_ping_vec), 0, (sockaddr*)&cu_up_addr, sizeof(cu_up_addr));
  ASSERT_GE(ret, 0) << "Failed to send message via sock_fd=" << sock_fd << " to `" << cfg.net_cfg.n3_bind_addr << ":"
                    << cu_up->get_n3_bind_port().value() << "` - " << strerror(errno);

  close(sock_fd);

  // check reception of message 1
  pdcp_tx_pdu sdu1 = f1u_bearer.wait_tx_sdu();
  ASSERT_TRUE(sdu1.pdcp_sn.has_value());
  EXPECT_EQ(sdu1.pdcp_sn.value(), 0);

  // check reception of message 2
  pdcp_tx_pdu sdu2 = f1u_bearer.wait_tx_sdu();
  ASSERT_TRUE(sdu2.pdcp_sn.has_value());
  EXPECT_EQ(sdu2.pdcp_sn.value(), 1);

  // check nothing else was received
  EXPECT_FALSE(f1u_bearer.have_tx_sdu());
  EXPECT_TRUE(f1u_bearer.tx_discard_sdu_list.empty());
}

TEST_F(cu_up_test, ul_data_flow)
{
  cu_up_configuration cfg = get_default_cu_up_config();

  //> Test preamble: listen on a free port

  int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  ASSERT_GE(sock_fd, 0);

  int         upf_port = 0; // Random free port selected by the OS: Avoid port conflicts with other tests.
  sockaddr_in upf_addr;
  upf_addr.sin_family      = AF_INET;
  upf_addr.sin_port        = htons(upf_port);
  upf_addr.sin_addr.s_addr = inet_addr(upf_addr_str.c_str());

  int ret = 0;

  ret = bind(sock_fd, (sockaddr*)&upf_addr, sizeof(upf_addr));
  ASSERT_GE(ret, 0) << "Failed to bind socket to `" << upf_addr_str << ":" << upf_port << "` - " << strerror(errno);

  // Find out the port that was assigned
  socklen_t upf_addr_len = sizeof(upf_addr);
  ret                    = getsockname(sock_fd, (struct sockaddr*)&upf_addr, &upf_addr_len);
  ASSERT_EQ(upf_addr_len, sizeof(upf_addr)) << "Mismatching upf_addr_len after getsockname()";
  ASSERT_GE(ret, 0) << "Failed to read port of socket bound to `" << upf_addr_str << ":0` - " << strerror(errno);

  //> Test main part: create CU-UP and transmit data

  cfg.net_cfg.upf_port = ntohs(upf_addr.sin_port);
  test_logger.debug("Using network_interface_config: {}", cfg.net_cfg);
  init(cfg);

  create_drb();

  // send message 1
  const uint8_t t_pdu_arr1[] = {
      0x80, 0x00, 0x00, 0x45, 0x00, 0x00, 0x54, 0xe8, 0x83, 0x40, 0x00, 0x40, 0x01, 0xfa, 0x00, 0xac, 0x10, 0x00,
      0x03, 0xac, 0x10, 0x00, 0x01, 0x08, 0x00, 0x2c, 0xbe, 0xb4, 0xa4, 0x00, 0x01, 0xd3, 0x45, 0x61, 0x63, 0x00,
      0x00, 0x00, 0x00, 0x1a, 0x20, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
      0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
      0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
  span<const uint8_t> t_pdu_span1 = {t_pdu_arr1};
  byte_buffer         t_pdu_buf1  = byte_buffer::create(t_pdu_span1).value();
  nru_ul_message      nru_msg1    = {};
  nru_msg1.t_pdu                  = byte_buffer_chain::create(std::move(t_pdu_buf1)).value();
  f1u_bearer.handle_pdu(std::move(nru_msg1));

  // send message 2
  const uint8_t t_pdu_arr2[] = {
      0x80, 0x00, 0x01, 0x45, 0x00, 0x00, 0x54, 0xe8, 0x83, 0x40, 0x00, 0x40, 0x01, 0xfa, 0x00, 0xac, 0x10, 0x00,
      0x03, 0xac, 0x10, 0x00, 0x01, 0x08, 0x00, 0x2c, 0xbe, 0xb4, 0xa4, 0x00, 0x01, 0xd3, 0x45, 0x61, 0x63, 0x00,
      0x00, 0x00, 0x00, 0x1a, 0x20, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
      0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
      0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
  span<const uint8_t> t_pdu_span2 = {t_pdu_arr2};
  byte_buffer         t_pdu_buf2  = byte_buffer::create(t_pdu_span2).value();
  nru_ul_message      nru_msg2    = {};
  nru_msg2.t_pdu                  = byte_buffer_chain::create(std::move(t_pdu_buf2)).value();
  f1u_bearer.handle_pdu(std::move(nru_msg2));

  std::array<uint8_t, 128> rx_buf;

  uint16_t exp_len      = 100;
  uint16_t f1u_hdr_len  = 3;
  uint16_t gtpu_hdr_len = 16;

  // receive message 1
  ret = recv(sock_fd, rx_buf.data(), rx_buf.size(), 0);
  ASSERT_EQ(ret, exp_len);
  EXPECT_TRUE(std::equal(t_pdu_span1.begin() + f1u_hdr_len, t_pdu_span1.end(), rx_buf.begin() + gtpu_hdr_len));

  // receive message 2
  ret = recv(sock_fd, rx_buf.data(), rx_buf.size(), 0);
  ASSERT_EQ(ret, exp_len);
  EXPECT_TRUE(std::equal(t_pdu_span2.begin() + f1u_hdr_len, t_pdu_span2.end(), rx_buf.begin() + gtpu_hdr_len));

  close(sock_fd);
}
