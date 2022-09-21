/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp_rx_test.h"
#include "../../lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/support/test_utils.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsgnb;

TEST_P(pdcp_rx_test, create_new_entity)
{
  init(GetParam());

  srsgnb::test_delimit_logger delimiter("Entity creation test. SN_SIZE={} ", sn_size);
  ASSERT_NE(pdcp_rx, nullptr);
}

TEST_P(pdcp_rx_test, sn_unpack)
{
  init(GetParam());

  auto test_hdr_reader = [this](uint32_t count) {
    srsgnb::test_delimit_logger delimiter("Header reader test. SN_SIZE={} COUNT={}", sn_size, count);
    // Get PDU to test
    byte_buffer test_pdu;
    get_test_pdu(count, test_pdu);
    uint32_t sn = {};
    ASSERT_TRUE(pdcp_rx->read_data_pdu_header(test_pdu, sn));
    ASSERT_EQ(sn, SN(count));
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_hdr_reader(0);
    test_hdr_reader(2048);
    test_hdr_reader(4096);
    test_hdr_reader(4294967295);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_hdr_reader(0);
    test_hdr_reader(131072);
    test_hdr_reader(262144);
    test_hdr_reader(4294967295);
  } else {
    FAIL();
  }
}

TEST_P(pdcp_rx_test, rx_in_order)
{
  init(GetParam());

  auto test_rx_in_order = [this](uint32_t count) {
    srsgnb::test_delimit_logger delimiter("RX in order test. SN_SIZE={} COUNT={}", sn_size, count);

    pdcp_rx->set_as_security_config(sec_cfg);
    pdcp_rx->enable_or_disable_security(pdcp_integrity_enabled::enabled, pdcp_ciphering_enabled::enabled);
    // Set the direction to downlink to avoid duplicating the test vectors.
    pdcp_rx->set_direction(security_direction::downlink);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(std::move(test_pdu1));
    ASSERT_EQ(1, test_frame.sdu_queue.size());
    pdcp_rx->handle_pdu(std::move(test_pdu2));
    ASSERT_EQ(2, test_frame.sdu_queue.size());
    while (not test_frame.sdu_queue.empty()) {
      ASSERT_EQ(test_frame.sdu_queue.front(), sdu1);
      test_frame.sdu_queue.pop();
    }
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_rx_in_order(0);
    test_rx_in_order(2047);
    test_rx_in_order(4095);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_rx_in_order(0);
    test_rx_in_order(131071);
    test_rx_in_order(262143);
  } else {
    FAIL();
  }
}

TEST_P(pdcp_rx_test, rx_out_of_order)
{
  init(GetParam());

  auto test_rx_in_order = [this](uint32_t count) {
    srsgnb::test_delimit_logger delimiter("RX in order test. SN_SIZE={} COUNT={}", sn_size, count);

    pdcp_rx->set_as_security_config(sec_cfg);
    pdcp_rx->enable_or_disable_security(pdcp_integrity_enabled::enabled, pdcp_ciphering_enabled::enabled);
    // Set the direction to downlink to avoid duplicating the test vectors.
    pdcp_rx->set_direction(security_direction::downlink);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(std::move(test_pdu2));
    ASSERT_EQ(0, test_frame.sdu_queue.size());
    pdcp_rx->handle_pdu(std::move(test_pdu1));
    ASSERT_EQ(2, test_frame.sdu_queue.size());
    while (not test_frame.sdu_queue.empty()) {
      ASSERT_EQ(test_frame.sdu_queue.front(), sdu1);
      test_frame.sdu_queue.pop();
    }
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_rx_in_order(0);
    test_rx_in_order(2047);
    test_rx_in_order(4095);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_rx_in_order(0);
    test_rx_in_order(131071);
    test_rx_in_order(262143);
  } else {
    FAIL();
  }
}
///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
std::string test_param_info_to_string(const ::testing::TestParamInfo<pdcp_sn_size>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "{}bit", to_number(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(pdcp_rx_test_all_sn_sizes,
                         pdcp_rx_test,
                         ::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
