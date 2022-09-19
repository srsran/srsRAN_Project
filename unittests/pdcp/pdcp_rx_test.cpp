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
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsgnb;

TEST_P(pdcp_rx_test, create_new_entity)
{
  init(GetParam());

  ASSERT_NE(pdcp_rx, nullptr);
}

TEST_P(pdcp_rx_test, sn_unpack)
{
  init(GetParam());

  auto test_hdr_reader = [this](uint32_t count) {
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
    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    // pdcp_rx_state test1_init_state = {.tx_next = count, .rx_next = count, .rx_deliv = count, .rx_reord = 0};
    // TESTASSERT(rx_helper.test_rx(std::move(test1_pdus), test1_init_state, 2, tst_sdu1) == 0);
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_rx_in_order(0);
    test_rx_in_order(2048);
    test_rx_in_order(4096);
    test_rx_in_order(4294967295);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    // test_hdr_reader(0);
    // test_hdr_reader(131072);
    // test_hdr_reader(262144);
    // test_hdr_reader(4294967295);
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
