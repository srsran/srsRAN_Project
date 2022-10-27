/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp_tx_status_report_test.h"
#include "../../lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/support/test_utils.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <list>
#include <queue>

using namespace srsgnb;

/// \brief Test basic data recovery functionality
TEST_P(pdcp_tx_status_report_test, data_recovery)
{
  auto test_hdr_writer = [this](uint32_t tx_next) {
    init(sn_size);
    unsigned                n_sdus = 5;
    std::queue<byte_buffer> exp_pdu_list;
    pdcp_tx_state           st = {tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->set_as_security_config(sec_cfg);
    pdcp_tx->enable_or_disable_security(pdcp_integrity_enabled::enabled, pdcp_ciphering_enabled::enabled);
    srsgnb::test_delimit_logger delimiter("Testing data recovery. SN_SIZE={} COUNT={}", sn_size, tx_next);
    for (uint32_t count = tx_next; count < tx_next + n_sdus; ++count) {
      // Write SDU
      byte_buffer sdu = {sdu1};
      pdcp_tx->handle_sdu(std::move(sdu));

      // Get generated PDU
      ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      exp_pdu_list.push(std::move(test_frame.pdu_queue.front()));
      test_frame.pdu_queue.pop();
      ASSERT_EQ(test_frame.pdu_queue.size(), 0);
    }
    pdcp_tx->data_recovery();
    for (uint32_t count = tx_next; count < tx_next + n_sdus; ++count) {
      byte_buffer pdu = std::move(test_frame.pdu_queue.front());
      test_frame.pdu_queue.pop();
      byte_buffer exp_pdu = std::move(exp_pdu_list.front());
      exp_pdu_list.pop();
      ASSERT_EQ(pdu.length(), exp_pdu.length());
      ASSERT_EQ(pdu, exp_pdu);
    }

    while (not test_frame.pdu_queue.empty()) {
      test_frame.pdu_queue.pop();
    }
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_hdr_writer(0);
    test_hdr_writer(2048);
    test_hdr_writer(4096);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_hdr_writer(0);
    test_hdr_writer(131072);
    test_hdr_writer(262144);
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

INSTANTIATE_TEST_SUITE_P(pdcp_tx_test_all_sn_sizes,
                         pdcp_tx_status_report_test,
                         ::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
