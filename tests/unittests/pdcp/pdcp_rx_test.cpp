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

#include "pdcp_rx_test.h"
#include "lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// Test creation of PDCP RX entities
TEST_P(pdcp_rx_test, create_new_entity)
{
  init(GetParam());
  srsran::test_delimit_logger delimiter("Entity creation test. SN_SIZE={} ", sn_size);

  ASSERT_NE(pdcp_rx, nullptr);
  ASSERT_NE(test_frame, nullptr);
}

/// Test extraction of PDCP sequence numbers
TEST_P(pdcp_rx_test, sn_unpack)
{
  init(GetParam());
  auto test_hdr_reader = [this](uint32_t count) {
    srsran::test_delimit_logger delimiter("Header reader test. SN_SIZE={} COUNT={}", sn_size, count);
    // Get PDU to test
    byte_buffer test_pdu;
    get_test_pdu(count, test_pdu);
    pdcp_data_pdu_header hdr;
    ASSERT_TRUE(pdcp_rx->read_data_pdu_header(hdr, test_pdu));
    ASSERT_EQ(hdr.sn, SN(count));
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_hdr_reader(0);
    test_hdr_reader(2048);
    test_hdr_reader(4096);
    test_hdr_reader(4294967295);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_hdr_reader(0);
    test_hdr_reader(131072);
    test_hdr_reader(262144);
    test_hdr_reader(4294967295);
  } else {
    FAIL();
  }
}

/// Test in-order reception of PDCP PDUs
TEST_P(pdcp_rx_test, rx_in_order)
{
  init(GetParam());
  auto test_rx_in_order = [this](uint32_t count) {
    srsran::test_delimit_logger delimiter("RX in order test. SN_SIZE={} COUNT={}", sn_size, count);

    pdcp_rx->configure_security(sec_cfg);
    pdcp_rx->set_integrity_protection(security::integrity_enabled::on);
    pdcp_rx->set_ciphering(security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu1)});
    ASSERT_EQ(1, test_frame->sdu_queue.size());
    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu2)});
    ASSERT_EQ(2, test_frame->sdu_queue.size());
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_rx_in_order(0);
    test_rx_in_order(2047);
    test_rx_in_order(4095);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_rx_in_order(0);
    test_rx_in_order(131071);
    test_rx_in_order(262143);
  } else {
    FAIL();
  }
}

/// Test out of order reception of PDUs.
/// All PDUs are received before the t-Reordering expires.
TEST_P(pdcp_rx_test, rx_out_of_order)
{
  init(GetParam());
  auto test_rx_out_of_order = [this](uint32_t count) {
    srsran::test_delimit_logger delimiter(
        "RX out-of-order test, no t-Reordering. SN_SIZE={} COUNT=[{}, {}]", sn_size, count + 1, count);

    pdcp_rx->configure_security(sec_cfg);
    pdcp_rx->set_integrity_protection(security::integrity_enabled::on);
    pdcp_rx->set_ciphering(security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    byte_buffer test_pdu3;
    get_test_pdu(count + 2, test_pdu3);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);

    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu2)});
    ASSERT_EQ(0, test_frame->sdu_queue.size());
    // check rx_reord matches rx_next matches count + 2
    EXPECT_EQ(pdcp_rx->get_state().rx_reord, count + 2);
    EXPECT_EQ(pdcp_rx->get_state().rx_reord, pdcp_rx->get_state().rx_next);

    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu3)});
    ASSERT_EQ(0, test_frame->sdu_queue.size());
    // check rx_reord still maches count + 2, i.e did not change because t_reord is already running; rx_next moved on
    EXPECT_EQ(pdcp_rx->get_state().rx_reord, count + 2);
    EXPECT_EQ(pdcp_rx->get_state().rx_next, count + 3);

    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu1)});
    ASSERT_EQ(3, test_frame->sdu_queue.size());
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_rx_out_of_order(0);
    test_rx_out_of_order(2047);
    test_rx_out_of_order(4095);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_rx_out_of_order(0);
    test_rx_out_of_order(131071);
    test_rx_out_of_order(262143);
  } else {
    FAIL();
  }
}

/// Test out of order reception of PDUs.
/// The out-of-order PDU is received after the t-Reordering expires.
TEST_P(pdcp_rx_test, rx_reordering_timer)
{
  init(GetParam());
  auto test_rx_t_reorder = [this](uint32_t count) {
    srsran::test_delimit_logger delimiter(
        "RX out-of-order test, t-Reordering expires. SN_SIZE={} COUNT=[{}, {}]", sn_size, count + 1, count);

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
    ASSERT_EQ(1, test_frame->sdu_queue.size());
    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu1)});
    ASSERT_EQ(1, test_frame->sdu_queue.size());
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };
  if (sn_size == pdcp_sn_size::size12bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(2047);
    test_rx_t_reorder(4095);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(131071);
    test_rx_t_reorder(262143);
  } else {
    FAIL();
  }
}

/// Test out of order reception of PDUs.
/// t-Reordering is set to 0, so PDUs are immediately delivered.
TEST_P(pdcp_rx_test, rx_reordering_timer_0ms)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::am, pdcp_t_reordering::ms0);
  auto test_rx_t_reorder = [this](uint32_t count) {
    srsran::test_delimit_logger delimiter(
        "RX out-of-order test, t-Reordering is set to 0. SN_SIZE={} COUNT=[{}, {}]", sn_size, count + 1, count);

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
    ASSERT_EQ(1, test_frame->sdu_queue.size());
    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu1)});
    ASSERT_EQ(1, test_frame->sdu_queue.size());
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };
  if (sn_size == pdcp_sn_size::size12bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(2047);
    test_rx_t_reorder(4095);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(131071);
    test_rx_t_reorder(262143);
  } else {
    FAIL();
  }
}

/// Test out of order reception of PDUs.
/// t-Reordering is set to infinite, so  no PDUs are delivered
/// until they are received in order.
TEST_P(pdcp_rx_test, rx_reordering_timer_infinite)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::am, pdcp_t_reordering::infinity);
  auto test_rx_t_reorder = [this](uint32_t count) {
    srsran::test_delimit_logger delimiter(
        "RX out-of-order test, t-Reordering is set to infinity. SN_SIZE={} COUNT=[{}, {}]", sn_size, count + 1, count);

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
    tick_all(6000); // max t-Reordering is 3000ms
    ASSERT_EQ(0, test_frame->sdu_queue.size());
    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu1)});
    ASSERT_EQ(2, test_frame->sdu_queue.size());
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };
  if (sn_size == pdcp_sn_size::size12bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(2047);
    test_rx_t_reorder(4095);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(131071);
    test_rx_t_reorder(262143);
  } else {
    FAIL();
  }
}

/// Test reception of PDUs with bad integrity checks.
/// The PDCP should notify the RRC of the integrity error.
TEST_P(pdcp_rx_test, rx_integrity_fail)
{
  init(GetParam());
  auto test_rx_integrity_fail = [this](uint32_t count) {
    srsran::test_delimit_logger delimiter("RX PDU with bad integrity. SN_SIZE={} COUNT={}", sn_size, count);

    pdcp_rx->configure_security(sec_cfg);
    pdcp_rx->set_integrity_protection(security::integrity_enabled::on);
    pdcp_rx->set_ciphering(security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    ASSERT_TRUE(test_pdu1.append(0)); // mess up MAC-I
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu1)});
    ASSERT_EQ(0, test_frame->sdu_queue.size());
    // TODO: Re-enable once the RRC supports notifications from the PDCP
    // ASSERT_EQ(1, test_frame->integrity_fail_counter);
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_rx_integrity_fail(0);
    test_rx_integrity_fail(2047);
    test_rx_integrity_fail(4095);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_rx_integrity_fail(0);
    test_rx_integrity_fail(131071);
    test_rx_integrity_fail(262143);
  } else {
    FAIL();
  }
}

/// Test count wrap-around protection for PDCP RX
/// Receive one PDU before notify limit, four after notify
/// limit and one after the hard limit.
TEST_P(pdcp_rx_test, count_wraparound)
{
  uint32_t       rx_next_notify = 262144;
  uint32_t       rx_next_max    = 262148;
  uint32_t       rx_next_start  = 262143;
  uint32_t       n_sdus         = 6;
  pdcp_max_count max_count{rx_next_notify, rx_next_max};
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::am, pdcp_t_reordering::ms10, max_count);

  auto test_max_count = [this, n_sdus](uint32_t count) {
    // Set state of PDCP entiy
    // Do not enable integrity or ciphering, to make it easier to generate test vectors.
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);

    // Write SDUs
    for (uint32_t i = 0; i < n_sdus; i++) {
      byte_buffer pdu;
      get_test_pdu(count + i, pdu);
      pdcp_rx->handle_pdu(byte_buffer_chain{std::move(pdu)});
    }

    // check nof max_count reached and max protocol failures.
    ASSERT_EQ(5, test_frame->sdu_queue.size());
    ASSERT_EQ(1, test_frame->nof_max_count_reached);
    ASSERT_EQ(1, test_frame->nof_protocol_failure);
  };

  test_max_count(rx_next_start);
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
                         pdcp_rx_test,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1, 2, 3)),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
