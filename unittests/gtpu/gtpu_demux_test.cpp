/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gtpu_test.h"
#include "srsgnb/gtpu/gtpu_demux.h"
#include "srsgnb/gtpu/gtpu_demux_factory.h"
#include <gtest/gtest.h>

using namespace srsgnb;

/// Fixture class for GTPU demux tests
class gtpu_demux_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    gtpu_tunnel = std::make_unique<gtpu_test_rx_upper>();

    // create DUT object
    dut = create_gtpu_demux();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<gtpu_test_rx_upper> gtpu_tunnel;

  std::unique_ptr<gtpu_demux> dut;
  srslog::basic_logger&       test_logger = srslog::fetch_basic_logger("TEST", false);
};

//// GTPU demux tesst
TEST_F(gtpu_demux_test, when_tunnel_not_registered_pdu_is_dropped)
{
  byte_buffer pdu{gtpu_ping_vec};
  dut->handle_pdu(std::move(pdu));
  ASSERT_EQ(gtpu_tunnel->last_rx.length(), 0);
}

TEST_F(gtpu_demux_test, when_tunnel_registered_pdu_is_forwarded)
{
  byte_buffer pdu{gtpu_ping_vec};
  dut->add_tunnel(0x1, gtpu_tunnel.get());
  dut->handle_pdu(std::move(pdu));
  ASSERT_EQ(gtpu_tunnel->last_rx.length(), sizeof(gtpu_ping_vec));
}

TEST_F(gtpu_demux_test, when_tunnel_is_removed_pdu_is_dropped)
{
  byte_buffer pdu{gtpu_ping_vec};
  dut->add_tunnel(0x1, gtpu_tunnel.get());
  dut->remove_tunnel(0x1);
  dut->handle_pdu(std::move(pdu));
  ASSERT_EQ(gtpu_tunnel->last_rx.length(), 0);
}
