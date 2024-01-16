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

#include "pdcp_rx_metrics_test.h"
#include "lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// \brief Test correct metrics counting of PDCP data SDUs/PDUs
TEST_P(pdcp_rx_metrics_test, sdu_pdu_metrics)
{
  auto test_metrics = [this](uint32_t count) {
    init(GetParam());
    srsran::test_delimit_logger delimiter("RX PDU/SDU metrics tests. SN_SIZE={} COUNT={}", sn_size, count);

    pdcp_rx->configure_security(sec_cfg);
    pdcp_rx->set_integrity_protection(security::integrity_enabled::on);
    pdcp_rx->set_ciphering(security::ciphering_enabled::on);

    byte_buffer test_pdu;
    get_test_pdu(count, test_pdu);

    // Get expected values
    uint32_t exp_sdu_size = 2;
    uint32_t exp_pdu_size = test_pdu.length();

    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu)});

    auto m = pdcp_rx->get_metrics();
    ASSERT_EQ(m.num_pdus, 1);
    ASSERT_EQ(m.num_pdu_bytes, exp_pdu_size);
    ASSERT_EQ(m.num_sdus, 1);
    ASSERT_EQ(m.num_sdu_bytes, exp_sdu_size);
    ASSERT_EQ(m.num_integrity_verified_pdus, 1);
    ASSERT_EQ(m.num_integrity_failed_pdus, 0);
    ASSERT_EQ(m.num_t_reordering_timeouts, 0);
    pdcp_rx->reset_metrics();
  };

  if (std::get<pdcp_sn_size>(GetParam()) == pdcp_sn_size::size12bits) {
    test_metrics(0);
    test_metrics(2048);
    test_metrics(4095);
    test_metrics(4096);
  } else if (std::get<pdcp_sn_size>(GetParam()) == pdcp_sn_size::size18bits) {
    test_metrics(0);
    test_metrics(131072);
    test_metrics(262144);
  } else {
    FAIL();
  }
}

/// \brief Test correct metrics counting of PDCP data with
/// integrity failures.
TEST_P(pdcp_rx_metrics_test, integrity_metrics)
{
  init(GetParam());

  auto test_metrics = [this](uint32_t count) {
    srsran::test_delimit_logger delimiter(
        "RX PDU with bad integrity metrics test. SN_SIZE={} COUNT={}", sn_size, count);

    pdcp_rx->configure_security(sec_cfg);
    pdcp_rx->set_integrity_protection(security::integrity_enabled::on);
    pdcp_rx->set_ciphering(security::ciphering_enabled::on);

    byte_buffer test_pdu;
    get_test_pdu(count, test_pdu);
    ASSERT_TRUE(test_pdu.append(0)); // mess up the MAC

    // Get expected values
    uint32_t exp_pdu_size = test_pdu.length();

    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu)});

    auto m = pdcp_rx->get_metrics();
    ASSERT_EQ(m.num_pdus, 1);
    ASSERT_EQ(m.num_pdu_bytes, exp_pdu_size);
    ASSERT_EQ(m.num_sdus, 0);
    ASSERT_EQ(m.num_sdu_bytes, 0);
    ASSERT_EQ(m.num_integrity_verified_pdus, 0);
    ASSERT_EQ(m.num_integrity_failed_pdus, 1);
    ASSERT_EQ(m.num_t_reordering_timeouts, 0);
    pdcp_rx->reset_metrics();
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_metrics(0);
    test_metrics(2048);
    test_metrics(4095);
    test_metrics(4096);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_metrics(0);
    test_metrics(131072);
    test_metrics(262144);
  } else {
    FAIL();
  }
}

/// \brief Test correct metrics counting of t-Reordering
/// timeouts
TEST_P(pdcp_rx_metrics_test, rx_reordering_timer)
{
  auto test_rx_t_reorder = [this](uint32_t count) {
    init(GetParam());
    srsran::test_delimit_logger delimiter(
        "t-Reordering expiration metrics test. SN_SIZE={} COUNT=[{}, {}]", sn_size, count + 1, count);

    pdcp_rx->configure_security(sec_cfg);
    pdcp_rx->set_integrity_protection(security::integrity_enabled::on);
    pdcp_rx->set_ciphering(security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu2)});
    ASSERT_EQ(0, test_frame->sdu_queue.size());
    tick_all(10);
    auto m = pdcp_rx->get_metrics();
    ASSERT_EQ(1, m.num_t_reordering_timeouts);
    pdcp_rx->reset_metrics();
  };
  if (std::get<pdcp_sn_size>(GetParam()) == pdcp_sn_size::size12bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(2047);
    test_rx_t_reorder(4095);
  } else if (std::get<pdcp_sn_size>(GetParam()) == pdcp_sn_size::size18bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(131071);
    test_rx_t_reorder(262143);
  } else {
    FAIL();
  }
}

///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
std::string test_param_info_to_string(const ::testing::TestParamInfo<std::tuple<pdcp_sn_size, unsigned>>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer,
                 "{}bit_nia{}_nea{}",
                 pdcp_sn_size_to_uint(std::get<pdcp_sn_size>(info.param)),
                 std::get<unsigned>(info.param),
                 std::get<unsigned>(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(pdcp_rx_test_all_sn_sizes,
                         pdcp_rx_metrics_test,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1)),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
