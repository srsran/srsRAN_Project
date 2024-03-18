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

#include "pdcp_tx_test.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

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
    // Prepare header
    pdcp_data_pdu_header hdr = {sn};

    // Pack header
    byte_buffer buf = {};
    ASSERT_TRUE(pdcp_tx->write_data_pdu_header(buf, hdr));
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
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_hdr_writer(0);
    test_hdr_writer(131072);
    test_hdr_writer(262144);
  } else {
    FAIL();
  }
}

/// \brief Test correct generation of PDCP PDUs
TEST_P(pdcp_tx_test, pdu_gen)
{
  init(GetParam());

  auto test_pdu_gen = [this](uint32_t tx_next) {
    srsran::test_delimit_logger delimiter("TX PDU generation. SN_SIZE={} COUNT={}", sn_size, tx_next);
    // Set state of PDCP entiy
    pdcp_tx_state st = {tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg);
    pdcp_tx->set_integrity_protection(security::integrity_enabled::on);
    pdcp_tx->set_ciphering(security::ciphering_enabled::on);

    // Write SDU
    byte_buffer sdu = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(sdu));

    // Get generated PDU
    ASSERT_EQ(test_frame.pdu_queue.size(), 1);
    pdcp_tx_pdu pdu = std::move(test_frame.pdu_queue.front());
    test_frame.pdu_queue.pop();

    // Get expected PDU
    byte_buffer exp_pdu;
    get_expected_pdu(tx_next, exp_pdu);

    ASSERT_EQ(pdu.buf.length(), exp_pdu.length());
    ASSERT_EQ(pdu.buf, exp_pdu);
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

/// \brief Test correct stalling of PDCP if RLC SDU queue is full
TEST_P(pdcp_tx_test, pdu_stall)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::am, pdcp_discard_timer::infinity);

  auto test_pdu_gen = [this](uint32_t tx_next) {
    srsran::test_delimit_logger delimiter("TX PDU stall. SN_SIZE={} COUNT={}", sn_size, tx_next);
    // Set state of PDCP entiy
    pdcp_tx_state st = {tx_next, tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg);
    pdcp_tx->set_integrity_protection(security::integrity_enabled::on);
    pdcp_tx->set_ciphering(security::ciphering_enabled::on);

    uint32_t sdu_queue_size = 4096;
    uint32_t window_size    = pdcp_window_size(sn_size);
    uint32_t stall          = std::min(sdu_queue_size, window_size - 1); // nof SDUs before stalling

    // Write SDU
    for (uint32_t count = tx_next; count < tx_next + stall; ++count) {
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Check there is a new PDU
      ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      pdcp_tx_pdu pdu = std::move(test_frame.pdu_queue.front());
      test_frame.pdu_queue.pop();
    }
    {
      // Write an SDU that should be dropped
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Check there is no new PDU
      ASSERT_EQ(test_frame.pdu_queue.size(), 0);
    }
    {
      // Notify transmission of all PDUs
      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(tx_next + stall - 1, sn_size));

      // Write an SDU that should be dropped
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Check there is no new PDU
      ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      test_frame.pdu_queue.pop();
    }
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

/// Test correct start of PDCP discard timers
/// and normal expiry of them
TEST_P(pdcp_tx_test, discard_timer_and_expiry)
{
  init(GetParam());

  auto test_discard_timer_expiry = [this](uint32_t tx_next) {
    // Set state of PDCP entiy
    pdcp_tx_state st = {tx_next, tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg);

    // Write first SDU
    {
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));
      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(tx_next, sn_size));
      ASSERT_EQ(1, pdcp_tx->nof_discard_timers());
    }
    // Write second SDU
    {
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));
      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(tx_next + 1, sn_size));
      ASSERT_EQ(2, pdcp_tx->nof_discard_timers());
    }
    // Let timers expire
    for (int i = 0; i < 10; i++) {
      timers.tick();
      worker.run_pending_tasks();
    }

    // Timers should have expired now.
    ASSERT_EQ(0, pdcp_tx->nof_discard_timers());
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_discard_timer_expiry(0);
    test_discard_timer_expiry(2047);
    test_discard_timer_expiry(4095);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_discard_timer_expiry(0);
    test_discard_timer_expiry(131071);
    test_discard_timer_expiry(262143);
  } else {
    FAIL();
  }
}

/// Test correct start of PDCP discard timers and stop from lower layers
TEST_P(pdcp_tx_test, discard_timer_and_stop)
{
  init(GetParam());

  auto test_discard_timer_stop = [this](pdcp_tx_state st) {
    // Set state of PDCP entiy
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg);

    constexpr uint32_t nof_sdus = 5;

    // Write SDUs
    for (uint32_t i = 0; i < nof_sdus; i++) {
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));
      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(st.tx_next, sn_size));
      ASSERT_EQ(i + 1, pdcp_tx->nof_discard_timers());
    }

    // Notify delivery of first SDU
    pdcp_tx->handle_delivery_notification(pdcp_compute_sn(st.tx_next, sn_size));
    ASSERT_EQ(nof_sdus - 1, pdcp_tx->nof_discard_timers());

    // Notify delivery up to third SDU
    pdcp_tx->handle_delivery_notification(pdcp_compute_sn(st.tx_next + 2, sn_size));
    ASSERT_EQ(nof_sdus - 3, pdcp_tx->nof_discard_timers());

    // Notify delivery of second SDU again
    // e.g. in case the UDP-based F1-U interface swaps the the order of transmit/delivery notifications
    pdcp_tx->handle_delivery_notification(pdcp_compute_sn(st.tx_next + 1, sn_size));
    ASSERT_EQ(nof_sdus - 3, pdcp_tx->nof_discard_timers());

    // Notify delivery of remaining SDUs
    pdcp_tx->handle_delivery_notification(pdcp_compute_sn(st.tx_next + nof_sdus - 1, sn_size));
    ASSERT_EQ(0, pdcp_tx->nof_discard_timers());
  };

  pdcp_tx_state st = {};
  if (config.sn_size == pdcp_sn_size::size12bits) {
    // test the beginning
    st = {0, 0, 0};
    test_discard_timer_stop(st);

    // test the center of SN range
    st = {2046, 2046, 2046};
    test_discard_timer_stop(st);

    // test the first wrap around
    st = {4094, 4094, 4094};
    test_discard_timer_stop(st);

    // test the second wrap around
    st = {8190, 8190, 8190};
    test_discard_timer_stop(st);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    // test the beginning
    st = {0, 0, 0};
    test_discard_timer_stop(st);

    // test the center of SN range
    st = {131070, 131070, 131070};
    test_discard_timer_stop(st);

    // test the first wrap around
    st = {262142, 262142, 262142};
    test_discard_timer_stop(st);

    // test the second wrap around
    st = {524286, 524286, 524286};
    test_discard_timer_stop(st);
  } else {
    FAIL();
  }
}

/// \brief Test correct generation of PDCP PDUs
TEST_P(pdcp_tx_test, pdu_stall_with_discard)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::am);

  auto test_pdu_gen = [this](uint32_t tx_next) {
    srsran::test_delimit_logger delimiter("TX PDU stall. SN_SIZE={} COUNT={}", sn_size, tx_next);
    // Set state of PDCP entiy
    pdcp_tx_state st = {tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg);
    pdcp_tx->set_integrity_protection(security::integrity_enabled::on);
    pdcp_tx->set_ciphering(security::ciphering_enabled::on);

    uint32_t sdu_queue_size = 4096;
    uint32_t window_size    = pdcp_window_size(sn_size);
    uint32_t stall          = std::min(sdu_queue_size, window_size - 1); // nof SDUs before stalling

    // Write SDU
    for (uint32_t count = tx_next; count < tx_next + stall; ++count) {
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Check there is a new PDU
      ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      pdcp_tx_pdu pdu = std::move(test_frame.pdu_queue.front());
      test_frame.pdu_queue.pop();
    }
    {
      // Write an SDU that should be dropped
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Check there is no new PDU
      ASSERT_EQ(test_frame.pdu_queue.size(), 0);
    }

    // Let timers expire
    for (int i = 0; i < 10; i++) {
      timers.tick();
      worker.run_pending_tasks();
    }

    // Check that we can write PDUs again
    for (uint32_t count = tx_next + stall; count < tx_next + 2 * stall; ++count) {
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Check there is a new PDU
      ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      pdcp_tx_pdu pdu = std::move(test_frame.pdu_queue.front());
      test_frame.pdu_queue.pop();
    }
    {
      // Write an SDU that should be dropped
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Check there is no new PDU
      ASSERT_EQ(test_frame.pdu_queue.size(), 0);
    }
    {
      // Notify transmission of all PDUs
      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(tx_next + 2 * stall - 1, sn_size));

      // Write an SDU that should not be dropped
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Check there is a new PDU
      ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      test_frame.pdu_queue.pop();
    }
    // Let timers expire
    for (int i = 0; i < 10; i++) {
      timers.tick();
      worker.run_pending_tasks();
    }
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
/// Test COUNT wrap-around protection systems
TEST_P(pdcp_tx_test, count_wraparound)
{
  uint32_t       tx_next_notify = 262144;
  uint32_t       tx_next_max    = 262154;
  uint32_t       tx_next_start  = 262143;
  uint32_t       n_sdus         = 20;
  pdcp_max_count max_count{tx_next_notify, tx_next_max};
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::am, pdcp_discard_timer::ms10, max_count);

  auto test_max_count = [this, n_sdus](uint32_t tx_next) {
    // Set state of PDCP entiy
    pdcp_tx_state st = {tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg);

    // Write first SDU
    for (uint32_t i = 0; i < n_sdus; i++) {
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));
      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(st.tx_next + i, sn_size));
    }
    // check nof max_count reached and max protocol failures.
    ASSERT_EQ(11, test_frame.pdu_queue.size());
    ASSERT_EQ(1, test_frame.nof_max_count_reached);
    ASSERT_EQ(1, test_frame.nof_protocol_failure);
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_max_count(tx_next_start);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_max_count(tx_next_start);
  } else {
    FAIL();
  }
}

/// \brief Test correct discard when RLC SDU queue is full
TEST_P(pdcp_tx_test_short_rlc_queue, discard_on_full_rlc_sdu_queue)
{
  init(GetParam());
  byte_buffer sdu = byte_buffer::create(sdu1).value();

  // Fill the RLC SDU queue
  for (uint32_t i = 0; i < config.custom.rlc_sdu_queue; i++) {
    pdcp_tx->handle_sdu(sdu.deep_copy().value());
    ASSERT_EQ(test_frame.pdu_queue.size(), 1);
    test_frame.pdu_queue.pop();
  }

  // Any further SDUs should be discarded and not forwarded to lower layers
  pdcp_tx->handle_sdu(sdu.deep_copy().value());
  ASSERT_TRUE(test_frame.pdu_queue.empty());

  // Make room for one more SDU and check if one PDU will be passed to lower layers
  pdcp_tx->handle_transmit_notification(0);
  pdcp_tx->handle_sdu(sdu.deep_copy().value());
  ASSERT_EQ(test_frame.pdu_queue.size(), 1);
  test_frame.pdu_queue.pop();

  // Any further SDUs should be discarded and not forwarded to lower layers
  pdcp_tx->handle_sdu(sdu.deep_copy().value());
  ASSERT_TRUE(test_frame.pdu_queue.empty());
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

INSTANTIATE_TEST_SUITE_P(pdcp_tx_test_all_sn_sizes,
                         pdcp_tx_test,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1, 2, 3)),
                         test_param_info_to_string);

INSTANTIATE_TEST_SUITE_P(pdcp_tx_test_all_sn_sizes,
                         pdcp_tx_test_short_rlc_queue,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1, 2, 3)),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
