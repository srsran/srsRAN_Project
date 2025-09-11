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

#include "cu_up_test_helpers.h"
#include "lib/cu_up/cu_up_impl.h"
#include "lib/e1ap/cu_up/e1ap_cu_up_asn1_helpers.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/pdcp/pdcp_sn_util.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include <arpa/inet.h>
#include <chrono>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace srsran;
using namespace srs_cu_up;
using namespace asn1::e1ap;

/// This implementation returns back to the E1 interface a dummy CU-UP E1 Setup Response message upon the receival of
/// the CU-UP E1 Setup Request message.
class dummy_cu_cp_handler : public e1_connection_client
{
public:
  std::unique_ptr<e1ap_message_notifier>
  handle_cu_up_connection_request(std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier_) override
  {
    class dummy_cu_up_tx_pdu_notifier : public e1ap_message_notifier
    {
    public:
      dummy_cu_up_tx_pdu_notifier(dummy_cu_cp_handler& parent_) : parent(parent_) {}

      void on_new_message(const e1ap_message& msg) override
      {
        if (msg.pdu.type() != asn1::e1ap::e1ap_pdu_c::types::init_msg) {
          return;
        }

        e1ap_message response;
        if (msg.pdu.init_msg().value.type().value ==
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_up_e1_setup_request) {
          // Generate a dummy CU-UP E1 Setup response message and pass it back to the CU-UP.
          response.pdu.set_successful_outcome();
          response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_GNB_CU_UP_E1_SETUP);

          auto& setup_res = response.pdu.successful_outcome().value.gnb_cu_up_e1_setup_resp();
          // Use the same transaction ID as in the request message.
          setup_res->transaction_id = msg.pdu.init_msg().value.gnb_cu_up_e1_setup_request()->transaction_id;
        } else if (msg.pdu.init_msg().value.type().value ==
                   asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::e1_release_request) {
          // Generate a dummy E1 Release response message and pass it back to the CU-UP.
          response.pdu.set_successful_outcome();
          response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_E1_RELEASE);

          auto& release_res = response.pdu.successful_outcome().value.e1_release_resp();
          // Use the same transaction ID as in the request message.
          release_res->transaction_id = msg.pdu.init_msg().value.e1_release_request()->transaction_id;
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

    return std::make_unique<dummy_cu_up_tx_pdu_notifier>(*this);
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

    srslog::fetch_basic_logger("CU-UP").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("GTPU").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("E1AP").set_level(srslog::basic_levels::debug);

    // create worker thread and executor
    worker   = std::make_unique<task_worker>("thread", 128, os_thread_realtime_priority::no_realtime());
    executor = make_task_executor_ptr(*worker);

    exec_pool    = std::make_unique<dummy_cu_up_executor_mapper>(executor.get());
    app_timers   = std::make_unique<timer_manager>(256);
    f1u_gw       = std::make_unique<dummy_f1u_gateway>(f1u_bearer);
    broker       = create_io_broker(io_broker_type::epoll);
    upf_addr_str = "127.0.0.1";

    // Set default UDP configs
    cu_up_udp_cfg.bind_port    = 0;           // Random free port selected by the OS.
    cu_up_udp_cfg.bind_address = "127.0.0.2"; // NG-U bind address
  }

  cu_up_config get_default_cu_up_config()
  {
    // create config
    cu_up_config cfg;

    cfg.qos[uint_to_five_qi(9)] = {};

    cfg.n3_cfg.gtpu_reordering_timer     = std::chrono::milliseconds(0);
    cfg.n3_cfg.gtpu_rate_limiting_period = std::chrono::milliseconds(100);
    cfg.n3_cfg.gtpu_queue_size           = 8192;
    cfg.n3_cfg.gtpu_batch_size           = 256;
    cfg.n3_cfg.gtpu_ignore_ue_ambr       = false;
    cfg.n3_cfg.warn_on_drop              = false;
    cfg.statistics_report_period         = std::chrono::seconds(1);

    return cfg;
  }

  cu_up_dependencies get_default_cu_up_dependencies()
  {
    cu_up_dependencies deps;
    deps.gtpu_pcap      = &dummy_pcap;
    deps.exec_mapper    = exec_pool.get();
    deps.e1_conn_client = &e1ap_client;
    deps.f1u_gateway    = f1u_gw.get();
    ngu_gw              = create_udp_gtpu_gateway(cu_up_udp_cfg, *broker, *executor, *executor);
    deps.ngu_gws.push_back(ngu_gw.get());
    deps.timers = app_timers.get();
    return deps;
  }

  void init(const cu_up_config& cfg, cu_up_dependencies&& deps)
  {
    auto cfg_copy = cfg;
    cu_up         = std::make_unique<srs_cu_up::cu_up>(cfg_copy, std::move(deps));
    cu_up->start();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<task_worker>                 worker;
  std::unique_ptr<task_executor>               executor;
  std::unique_ptr<dummy_cu_up_executor_mapper> exec_pool;

  std::unique_ptr<timer_manager> app_timers;

  dummy_cu_cp_handler                e1ap_client;
  dummy_inner_f1u_bearer             f1u_bearer;
  std::unique_ptr<dummy_f1u_gateway> f1u_gw;
  std::unique_ptr<io_broker>         broker;
  std::unique_ptr<gtpu_gateway>      ngu_gw;
  std::unique_ptr<srs_cu_up::cu_up>  cu_up;
  srslog::basic_logger&              test_logger = srslog::fetch_basic_logger("TEST");

  udp_network_gateway_config cu_up_udp_cfg{};

  null_dlt_pcap dummy_pcap;

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
    cu_up->get_cu_up_manager()->handle_bearer_context_setup_request(bearer_context_setup);
  }

  struct upf_info_t {
    int         sock_fd = -1;
    sockaddr_in upf_addr;
  };

  upf_info_t init_upf()
  {
    int sock_fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0) {
      return {sock_fd, {}};
    }

    int         upf_port = 0; // Random free port selected by the OS: Avoid port conflicts with other tests.
    sockaddr_in upf_addr;
    upf_addr.sin_family      = AF_INET;
    upf_addr.sin_port        = htons(upf_port);
    upf_addr.sin_addr.s_addr = inet_addr(upf_addr_str.c_str());

    int ret = 0;

    ret = ::bind(sock_fd, (sockaddr*)&upf_addr, sizeof(upf_addr));
    if (ret < 0) {
      fmt::print(stderr, "Failed to bind socket to `{}:{}` - {}\n", upf_addr_str, upf_port, ::strerror(errno));
      return {ret, {}};
    }

    // Find out the port that was assigned
    socklen_t upf_addr_len = sizeof(upf_addr);
    ret                    = ::getsockname(sock_fd, (struct sockaddr*)&upf_addr, &upf_addr_len);
    if (ret < 0) {
      fmt::print(stderr, "Failed to read port of socket bound to `{}:0` - {}", upf_addr_str, ::strerror(errno));
      return {ret, {}};
    }
    if (upf_addr_len != sizeof(upf_addr)) {
      fmt::print(stderr, "Mismatching upf_addr_len after ::getsockname()");
      return {-1, {}};
    }
    return {sock_fd, upf_addr};
  }
};

//////////////////////////////////////////////////////////////////////////////////////
/* E1AP connection handling                                                           */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the E1AP connection

TEST_F(cu_up_test, when_e1ap_connection_established_then_e1ap_connected)
{
  init(get_default_cu_up_config(), get_default_cu_up_dependencies());

  // Connect E1AP
  cu_up->get_cu_up_manager()->on_e1ap_connection_establish();

  // check that E1AP is in connected state
  ASSERT_TRUE(cu_up->get_cu_up_manager()->e1ap_is_connected());
}

//////////////////////////////////////////////////////////////////////////////////////
/* User Data Flow                                                                   */
//////////////////////////////////////////////////////////////////////////////////////
TEST_F(cu_up_test, dl_data_flow)
{
  cu_up_config cfg = get_default_cu_up_config();

  // Initialize UPF simulator on a random port.
  upf_info_t upf_info = init_upf();
  ASSERT_GE(upf_info.sock_fd, 0);
  cfg.n3_cfg.upf_port = ntohs(upf_info.upf_addr.sin_port);

  cu_up_dependencies dependencies = get_default_cu_up_dependencies();

  // Initialize CU-UP
  init(cfg, std::move(dependencies));

  // Create DRB
  create_drb();

  // We send a UL message to set desired buffer size in DL bearer
  // UL message 1
  nru_ul_message nru_msg1     = {};
  const uint8_t  t_pdu_arr1[] = {
      0x80, 0x00, 0x00, 0x45, 0x00, 0x00, 0x54, 0xe8, 0x83, 0x40, 0x00, 0x40, 0x01, 0xfa, 0x00, 0xac, 0x10, 0x00,
      0x03, 0xac, 0x10, 0x00, 0x01, 0x08, 0x00, 0x2c, 0xbe, 0xb4, 0xa4, 0x00, 0x01, 0xd3, 0x45, 0x61, 0x63, 0x00,
      0x00, 0x00, 0x00, 0x1a, 0x20, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
      0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
      0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
  span<const uint8_t> t_pdu_span1  = {t_pdu_arr1};
  byte_buffer         t_pdu_buf1   = byte_buffer::create(t_pdu_span1).value();
  nru_msg1.t_pdu                   = byte_buffer_chain::create(std::move(t_pdu_buf1)).value();
  nru_dl_data_delivery_status ddds = {};
  ddds.desired_buffer_size_for_drb = 1 << 20; // 1MBi RLC SDU size
  nru_msg1.data_delivery_status    = ddds;

  f1u_bearer.handle_pdu(std::move(nru_msg1));

  test_logger.info("Waiting UL PDU. This is required to update DDDS");

  // We wait here for the UL PDU to arrive, to make sure the DDDS has been processed.
  // receive message 1
  std::array<uint8_t, 128> rx_buf;
  int                      ret;
  ret = ::recv(upf_info.sock_fd, rx_buf.data(), rx_buf.size(), 0);

  test_logger.info("Processed UL PDU. DDDS is updated");

  // Now that the disered buffer size is updated, we push DL PDUs
  sockaddr_in cu_up_addr;
  cu_up_addr.sin_family      = AF_INET;
  cu_up_addr.sin_port        = htons(cu_up->get_n3_bind_port().value());
  cu_up_addr.sin_addr.s_addr = inet_addr(cu_up_udp_cfg.bind_address.c_str());

  // DL PDU teid=2, qfi=1
  const uint8_t gtpu_ping_vec[] = {
      0x34, 0xff, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x85, 0x01, 0x00, 0x01, 0x00, 0x45,
      0x00, 0x00, 0x54, 0x9b, 0xfb, 0x00, 0x00, 0x40, 0x01, 0x56, 0x5a, 0xc0, 0xa8, 0x04, 0x01, 0xc0, 0xa8,
      0x03, 0x02, 0x00, 0x00, 0xb8, 0xc0, 0x00, 0x02, 0x00, 0x01, 0x5d, 0x26, 0x77, 0x64, 0x00, 0x00, 0x00,
      0x00, 0xb1, 0xde, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
      0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};

  // send message 1
  ret = sendto(upf_info.sock_fd, gtpu_ping_vec, sizeof(gtpu_ping_vec), 0, (sockaddr*)&cu_up_addr, sizeof(cu_up_addr));
  ASSERT_GE(ret, 0) << "Failed to send message via sock_fd=" << upf_info.sock_fd << " to `"
                    << cu_up_udp_cfg.bind_address << ":" << cu_up->get_n3_bind_port().value() << "` - "
                    << ::strerror(errno);

  // send message 2
  ret = sendto(upf_info.sock_fd, gtpu_ping_vec, sizeof(gtpu_ping_vec), 0, (sockaddr*)&cu_up_addr, sizeof(cu_up_addr));
  ASSERT_GE(ret, 0) << "Failed to send message via sock_fd=" << upf_info.sock_fd << " to `"
                    << cu_up_udp_cfg.bind_address << ":" << cu_up->get_n3_bind_port().value() << "` - "
                    << ::strerror(errno);
  ::close(upf_info.sock_fd);

  // check reception of message 1
  nru_dl_message          sdu1 = f1u_bearer.wait_tx_sdu();
  std::optional<uint32_t> sdu1_pdcp_sn =
      get_pdcp_sn(sdu1.t_pdu, pdcp_sn_size::size18bits, /* is_srb = */ false, test_logger);
  ASSERT_TRUE(sdu1_pdcp_sn.has_value());
  EXPECT_EQ(sdu1_pdcp_sn.value(), 0);

  // check reception of message 2
  nru_dl_message          sdu2 = f1u_bearer.wait_tx_sdu();
  std::optional<uint32_t> sdu2_pdcp_sn =
      get_pdcp_sn(sdu2.t_pdu, pdcp_sn_size::size18bits, /* is_srb = */ false, test_logger);
  ASSERT_TRUE(sdu2_pdcp_sn.has_value());
  EXPECT_EQ(sdu2_pdcp_sn.value(), 1);

  // check nothing else was received
  EXPECT_FALSE(f1u_bearer.have_tx_sdu());
  EXPECT_TRUE(f1u_bearer.tx_discard_sdu_list.empty());
}

TEST_F(cu_up_test, ul_data_flow)
{
  cu_up_config cfg = get_default_cu_up_config();

  //> Test preamble: listen on a free port
  upf_info_t upf_info = init_upf();
  cfg.n3_cfg.upf_port = ntohs(upf_info.upf_addr.sin_port);

  //> Test main part: create CU-UP and transmit data
  cu_up_dependencies dependencies = get_default_cu_up_dependencies();
  init(cfg, std::move(dependencies));

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
  int ret = ::recv(upf_info.sock_fd, rx_buf.data(), rx_buf.size(), 0);
  ASSERT_EQ(ret, exp_len);
  EXPECT_TRUE(std::equal(t_pdu_span1.begin() + f1u_hdr_len, t_pdu_span1.end(), rx_buf.begin() + gtpu_hdr_len));

  // receive message 2
  ret = ::recv(upf_info.sock_fd, rx_buf.data(), rx_buf.size(), 0);
  ASSERT_EQ(ret, exp_len);
  EXPECT_TRUE(std::equal(t_pdu_span2.begin() + f1u_hdr_len, t_pdu_span2.end(), rx_buf.begin() + gtpu_hdr_len));

  ::close(upf_info.sock_fd);
}

TEST_F(cu_up_test, echo_data_flow)
{
  cu_up_config cfg = get_default_cu_up_config();

  //> Test preamble: listen on a free port
  upf_info_t upf_info = init_upf();
  cfg.n3_cfg.upf_port = ntohs(upf_info.upf_addr.sin_port);

  //> Test main part: create CU-UP and transmit data
  cu_up_dependencies dependencies = get_default_cu_up_dependencies();
  init(cfg, std::move(dependencies));

  // Now that the disered buffer size is updated, we push DL PDUs
  sockaddr_in cu_up_addr;
  cu_up_addr.sin_family      = AF_INET;
  cu_up_addr.sin_port        = htons(cu_up->get_n3_bind_port().value());
  cu_up_addr.sin_addr.s_addr = inet_addr(cu_up_udp_cfg.bind_address.c_str());

  // Send GTP-U echo request test message.
  const uint8_t gtpu_echo_req_vec[] = {0x32, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00};

  int ret;
  ret = sendto(
      upf_info.sock_fd, gtpu_echo_req_vec, sizeof(gtpu_echo_req_vec), 0, (sockaddr*)&cu_up_addr, sizeof(cu_up_addr));
  ASSERT_GE(ret, 0) << "Failed to send message via sock_fd=" << upf_info.sock_fd << " to `"
                    << cu_up_udp_cfg.bind_address << ":" << cu_up->get_n3_bind_port().value() << "` - "
                    << ::strerror(errno);

  // Receive GTP-U echo response message and check result.
  constexpr uint16_t           exp_len            = 14;
  std::array<uint8_t, exp_len> gtpu_echo_resp_vec = {
      0x32, 0x02, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x0e, 0x00};

  std::array<uint8_t, 128> rx_buf;
  ret = recv(upf_info.sock_fd, rx_buf.data(), rx_buf.size(), 0);
  ASSERT_EQ(ret, exp_len);
  EXPECT_TRUE(std::equal(gtpu_echo_resp_vec.begin(), gtpu_echo_resp_vec.end(), rx_buf.begin()));

  ::close(upf_info.sock_fd);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
