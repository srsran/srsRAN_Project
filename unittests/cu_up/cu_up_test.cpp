/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_test_helpers.h"
#include "srsgnb/cu_up/cu_up_factory.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/test_utils.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace srsgnb;
using namespace srs_cu_up;
using namespace asn1::e1ap;

class dummy_e1_notifier : public e1_message_notifier
{
  void on_new_message(const e1_message& msg) override
  {
    // do nothing
  }
};

/// Fixture class for CU-UP test
class cu_up_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // create worker thread and executer
    task_worker                    task_worker("thread", 1, false, os_thread_realtime_priority::MAX_PRIO);
    std::unique_ptr<task_executor> task_executor = make_task_executor(task_worker);

    dummy_e1_notifier e1_message_notifier;
    dummy_f1u_gateway f1u_gw;

    // create config
    cu_up_configuration cfg;
    cfg.cu_up_executor = task_executor.get();
    cfg.e1_notifier    = &e1_message_notifier;
    cfg.f1u_gateway    = &f1u_gw;
    cfg.gtp_bind_addr  = "127.0.0.1";
    cfg.upf_addr       = "127.0.1.100";

    // create and start DUT
    cu_up = create_cu_up(cfg);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<srs_cu_up::cu_up_interface> cu_up;
  srslog::basic_logger&                       test_logger = srslog::fetch_basic_logger("TEST");
};

//////////////////////////////////////////////////////////////////////////////////////
/* E1 connection handling                                                           */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the E1 connection
TEST_F(cu_up_test, when_e1_connection_established_then_e1_connected)
{
  // Connect E1
  cu_up->on_e1_connection_establish();

  // check that E1 is in connected state
  ASSERT_TRUE(cu_up->e1_is_connected());
}

//////////////////////////////////////////////////////////////////////////////////////
/* User Data Flow                                                                   */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_up_test, dl_data_flow)
{
  int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  ASSERT_GE(sock_fd, 0);

  sockaddr_in cu_up_addr;
  cu_up_addr.sin_family      = AF_INET;
  cu_up_addr.sin_port        = htons(2152);
  cu_up_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  //  std::array<uint8_t, 14> msg0 = {
  //      0x00, // Version  |PT | * | E | S |PN
  //      0x00, // Message Type
  //      0x00, // Length (upper)
  //      0x00, // Length (lower)
  //      0x00, // TEID (1st Octet)
  //      0x00, // TEID (2nd Octet)
  //      0x00, // TEID (3rd Octet)
  //      0x00, // TEID (4th Octet)
  //      0x00, // Seq Number (upper)
  //      0x00, // Seq Number (lower)
  //      0x11, // N-PDU
  //      0x22, // N-PDU
  //      0x33, // N-PDU
  //      0x44  // N-PDU
  //  };

  const uint8_t gtpu_ping_vec[] = {
      0x30, 0xff, 0x00, 0x54, 0x00, 0x00, 0x00, 0x01, 0x45, 0x00, 0x00, 0x54, 0xe8, 0x83, 0x40, 0x00, 0x40, 0x01, 0xfa,
      0x00, 0xac, 0x10, 0x00, 0x03, 0xac, 0x10, 0x00, 0x01, 0x08, 0x00, 0x2c, 0xbe, 0xb4, 0xa4, 0x00, 0x01, 0xd3, 0x45,
      0x61, 0x63, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x20, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14,
      0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
      0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};

  // int ret = sendto(sock_fd, msg0.data(), msg0.size(), 0, (sockaddr*)&cu_up_addr, sizeof(cu_up_addr));
  int ret = sendto(sock_fd, gtpu_ping_vec, sizeof(gtpu_ping_vec), 0, (sockaddr*)&cu_up_addr, sizeof(cu_up_addr));
  ASSERT_GE(ret, 0);

  // let the Rx thread pick up the message
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  close(sock_fd);
}
