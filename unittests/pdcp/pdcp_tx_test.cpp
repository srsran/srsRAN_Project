/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp_tx_test.h"
#include "../../lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsgnb;

/// \brief Test correct creation of PDCP TX  entity
TEST_P(pdcp_tx_test, create_new_entity)
{
  init(GetParam());

  ASSERT_NE(pdcp_tx, nullptr);
}

/// \brief Test correct packing of PDCP data PDU headers
TEST_P(pdcp_tx_test, sn_pack)
{
  init(GetParam());

  auto test_hdr_writer = [this](uint32_t sn) {
    // Generate PDU header
    byte_buffer buf = {};
    pdcp_tx->write_data_pdu_header(buf, sn);
    // Get expected PDU header
    byte_buffer exp_pdu;
    get_expected_pdu(sn, exp_pdu);
    byte_buffer_view expected_hdr = {exp_pdu, 0, pdu_hdr_len};
    ASSERT_EQ(buf.length(), expected_hdr.length());
    ASSERT_EQ(buf, expected_hdr);
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_hdr_writer(0);
    test_hdr_writer(2048);
    test_hdr_writer(4096);
    test_hdr_writer(4294967295);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_hdr_writer(0);
    test_hdr_writer(131072);
    test_hdr_writer(262144);
    test_hdr_writer(4294967295);
  } else {
    FAIL();
  }
}

/// \brief Test correct generation of PDCP PDUs
TEST_P(pdcp_tx_test, pdu_gen)
{
  init(GetParam());

  auto test_pdu_gen = [this](uint32_t tx_next) {
    // Set state of PDCP entiy
    pdcp_tx_state st = {tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->set_as_security_config(sec_cfg);
    pdcp_tx->enable_or_disable_security(pdcp_integrity_enabled::enabled, pdcp_ciphering_enabled::enabled);

    // Write SDU
    byte_buffer sdu = {sdu1};
    pdcp_tx->handle_sdu(std::move(sdu));

    // Get generated PDU
    ASSERT_EQ(test_frame.pdu_queue.size(), 1);
    byte_buffer pdu = std::move(test_frame.pdu_queue.front());
    test_frame.pdu_queue.pop();

    // Get expected PDU
    byte_buffer exp_pdu;
    get_expected_pdu(tx_next, exp_pdu);

    ASSERT_EQ(pdu.length(), exp_pdu.length());
    ASSERT_EQ(pdu, exp_pdu);
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_pdu_gen(0);
    test_pdu_gen(2048);
    test_pdu_gen(4096);
    test_pdu_gen(4294967295);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_pdu_gen(0);
    test_pdu_gen(131072);
    test_pdu_gen(262144);
    test_pdu_gen(4294967295);
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
                         pdcp_tx_test,
                         ::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
