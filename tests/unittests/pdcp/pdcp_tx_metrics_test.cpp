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

#include "pdcp_tx_metrics_test.h"
#include "lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// \brief Test correct metrics counting of PDCP data SDUs/PDUs.
TEST_P(pdcp_tx_metrics_test, sdu_pdu)
{
  init(GetParam());

  auto test_pdu_gen = [this](uint32_t tx_next) {
    srsran::test_delimit_logger delimiter("TX SDU/PDU metrics test. SN_SIZE={} COUNT={}", sn_size, tx_next);
    // Set state of PDCP entiy
    pdcp_tx->reset_metrics();
    pdcp_tx_state st = {tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg);
    pdcp_tx->set_integrity_protection(security::integrity_enabled::on);
    pdcp_tx->set_ciphering(security::ciphering_enabled::on);

    // Write SDU
    byte_buffer sdu = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(sdu));
    pdcp_tx->handle_transmit_notification(pdcp_compute_sn(st.tx_next + 1, sn_size));

    uint32_t exp_sdu_size = 2;
    auto     m            = pdcp_tx->get_metrics();
    ASSERT_EQ(m.num_sdus, 1);
    ASSERT_EQ(m.num_sdu_bytes, exp_sdu_size);
    ASSERT_EQ(m.num_pdus, 1);
    ASSERT_EQ(m.num_pdu_bytes, pdu_hdr_len + exp_sdu_size + mac_hdr_len);
    ASSERT_EQ(m.num_discard_timeouts, 0);
    tick_all(10);
    m = pdcp_tx->get_metrics();
    ASSERT_EQ(m.num_discard_timeouts, 1);
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_pdu_gen(0);
    test_pdu_gen(2048);
    test_pdu_gen(4095);
    test_pdu_gen(4096);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_pdu_gen(0);
    test_pdu_gen(131072);
    test_pdu_gen(262144);
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
  fmt::format_to(buffer, "{}bit", pdcp_sn_size_to_uint(std::get<pdcp_sn_size>(info.param)));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(pdcp_tx_test_all_sn_sizes,
                         pdcp_tx_metrics_test,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1)),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
