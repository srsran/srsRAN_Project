
/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

TEST_F(pdcp_rx_metrics_container_test, init)
{
  pdcp_rx_metrics_container m = {};

  srslog::fetch_basic_logger("TEST", false).info("Metrics: {}", m);

  // Check values
  ASSERT_EQ(m.num_pdus, 0);
  ASSERT_EQ(m.num_pdu_bytes, 0);
  ASSERT_EQ(m.num_data_pdus, 0);
  ASSERT_EQ(m.num_data_pdu_bytes, 0);
  ASSERT_EQ(m.num_dropped_pdus, 0);
  ASSERT_EQ(m.num_sdus, 0);
  ASSERT_EQ(m.num_sdu_bytes, 0);
  ASSERT_EQ(m.num_integrity_verified_pdus, 0);
  ASSERT_EQ(m.num_integrity_failed_pdus, 0);
  ASSERT_EQ(m.num_t_reordering_timeouts, 0);
  ASSERT_EQ(m.reordering_delay_us, 0);
  ASSERT_EQ(m.reordering_counter, 0);
  ASSERT_EQ(m.sum_sdu_latency_ns, 0);
  ASSERT_EQ(m.counter, 0);
  for (auto freq : m.sdu_latency_hist) {
    ASSERT_EQ(freq, 0);
  }
  ASSERT_EQ(m.min_sdu_latency_ns, std::nullopt);
  ASSERT_EQ(m.max_sdu_latency_ns, std::nullopt);

  {
    // Check fmt formatter
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "{}", m);
    std::string out_str = to_c_str(buffer);
    std::string exp_str =
        "num_sdus=0 num_sdu_bytes=0 num_dropped_pdus=0 num_pdus=0 num_pdu_bytes=0 "
        "num_integrity_verified_pdus=0 num_integrity_failed_pdus=0 num_t_reordering_timeouts=0 "
        "avg_reordering_delay=0.00ms reordering_counter=0 avg_sdu_latency=0.00us sdu_latency_hist=[0 0 0 0 0 0 "
        "0 0] min_sdu_latency={na} max_sdu_latency={na} avg_crypto_latency=0.00us";
    srslog::fetch_basic_logger("TEST", false).info("out_str={}", out_str);
    srslog::fetch_basic_logger("TEST", false).info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }

  {
    // Check custom formatter
    timer_duration dur{2}; // 2ms
    std::string    out_str = format_pdcp_rx_metrics(dur, m);
    std::string    exp_str =
        "num_sdus=0 sdu_rate= 0bps num_dropped_pdus=0 num_pdus=0 pdu_rate= 0bps "
        "num_integrity_verified_pdus=0 num_integrity_failed_pdus=0 num_t_reordering_timeouts=0 "
        "avg_reordering_delay=0.00ms reordering_counter=0 avg_sdu_latency=0.00us sdu_latency_hist=[ 0  0  0  0 "
        " 0  0  0  0] min_sdu_latency={na} max_sdu_latency={na} crypto_cpu_usage=0.00\%";
    srslog::fetch_basic_logger("TEST", false).info("out_str={}", out_str);
    srslog::fetch_basic_logger("TEST", false).info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }
}

TEST_F(pdcp_rx_metrics_container_test, values)
{
  pdcp_rx_metrics_container m = {.num_pdus                         = 49532,
                                 .num_pdu_bytes                    = 10000,
                                 .num_data_pdus                    = 49121,
                                 .num_data_pdu_bytes               = 9500,
                                 .num_dropped_pdus                 = 94925,
                                 .num_sdus                         = 59493,
                                 .num_sdu_bytes                    = 20000,
                                 .num_integrity_verified_pdus      = 449993,
                                 .num_integrity_failed_pdus        = 865423,
                                 .num_t_reordering_timeouts        = 4456,
                                 .reordering_delay_us              = 456773,
                                 .reordering_counter               = 23,
                                 .sum_sdu_latency_ns               = 547454,
                                 .counter                          = 833333332,
                                 .sum_crypto_processing_latency_ns = 10000000,
                                 .sdu_latency_hist   = {999, 20, 400, 8000, 160000, 3200000, 64000000, 128},
                                 .min_sdu_latency_ns = 1200,
                                 .max_sdu_latency_ns = 54322};

  srslog::fetch_basic_logger("TEST", false).info("Metrics: {}", m);

  // Check values
  ASSERT_EQ(m.num_pdus, 49532);
  ASSERT_EQ(m.num_pdu_bytes, 10000);
  ASSERT_EQ(m.num_data_pdus, 49121);
  ASSERT_EQ(m.num_data_pdu_bytes, 9500);
  ASSERT_EQ(m.num_dropped_pdus, 94925);
  ASSERT_EQ(m.num_sdus, 59493);
  ASSERT_EQ(m.num_sdu_bytes, 20000);
  ASSERT_EQ(m.num_integrity_verified_pdus, 449993);
  ASSERT_EQ(m.num_integrity_failed_pdus, 865423);
  ASSERT_EQ(m.num_t_reordering_timeouts, 4456);
  ASSERT_EQ(m.reordering_delay_us, 456773);
  ASSERT_EQ(m.reordering_counter, 23);
  ASSERT_EQ(m.sum_sdu_latency_ns, 547454);
  ASSERT_EQ(m.counter, 833333332);
  ASSERT_EQ(m.sum_crypto_processing_latency_ns, 10000000);
  std::array<uint32_t, 8> h = {999, 20, 400, 8000, 160000, 3200000, 64000000, 128};
  ASSERT_EQ(m.sdu_latency_hist, h);
  ASSERT_EQ(m.min_sdu_latency_ns, 1200);
  ASSERT_EQ(m.max_sdu_latency_ns, 54322);

  {
    // Check fmt formatter
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "{}", m);
    std::string out_str = to_c_str(buffer);
    std::string exp_str =
        "num_sdus=59493 num_sdu_bytes=20000 num_dropped_pdus=94925 num_pdus=49532 num_pdu_bytes=10000 "
        "num_integrity_verified_pdus=449993 num_integrity_failed_pdus=865423 num_t_reordering_timeouts=4456 "
        "avg_reordering_delay=19.86ms reordering_counter=23 avg_sdu_latency=0.01us sdu_latency_hist=[999 20 400 "
        "8000 "
        "160000 3200000 64000000 128] min_sdu_latency=1200ns max_sdu_latency=54322ns "
        "avg_crypto_latency=0.17us";
    srslog::fetch_basic_logger("TEST", false).info("out_str={}", out_str);
    srslog::fetch_basic_logger("TEST", false).info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }

  {
    // Check custom formatter
    timer_duration dur{2}; // 2ms
    std::string    out_str = format_pdcp_rx_metrics(dur, m);
    std::string    exp_str =
        "num_sdus=59.5k sdu_rate=80Mbps num_dropped_pdus=94.9k num_pdus=49.5k pdu_rate=40Mbps "
        "num_integrity_verified_pdus=450k num_integrity_failed_pdus=865k num_t_reordering_timeouts=4.46k "
        "avg_reordering_delay=19.86ms reordering_counter=23 avg_sdu_latency=0.01us sdu_latency_hist=[ 999  20  400 "
        "8.0k "
        "160k 3.2M 64M  128] min_sdu_latency=1.20us max_sdu_latency=54.32us crypto_cpu_usage=500.00\%";
    srslog::fetch_basic_logger("TEST", false).info("out_str={}", out_str);
    srslog::fetch_basic_logger("TEST", false).info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }
}

/// \brief Test correct metrics counting of PDCP data SDUs/PDUs
TEST_P(pdcp_rx_metrics_test, sdu_pdu_metrics)
{
  auto test_metrics = [this](uint32_t count) {
    init(GetParam());
    srsran::test_delimit_logger delimiter("RX PDU/SDU metrics tests. SN_SIZE={} COUNT={}", sn_size, count);

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    byte_buffer test_pdu;
    get_test_pdu(count, test_pdu);

    // Get expected values
    uint32_t exp_sdu_size = 2;
    uint32_t exp_pdu_size = test_pdu.length();

    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu)).value());
    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();

    tick_all(150);
    if (metrics_notif.metrics_list.size() == 0) {
      FAIL() << "No metrics notification received";
    }
    auto m = metrics_notif.metrics_list.back().rx;
    ASSERT_EQ(m.num_pdus, 1);
    ASSERT_EQ(m.num_pdu_bytes, exp_pdu_size);
    ASSERT_EQ(m.num_data_pdus, 1);
    ASSERT_EQ(m.num_data_pdu_bytes, exp_pdu_size);
    ASSERT_EQ(m.num_sdus, 1);
    ASSERT_EQ(m.num_sdu_bytes, exp_sdu_size);
    ASSERT_EQ(m.num_integrity_verified_pdus, 1);
    ASSERT_EQ(m.num_integrity_failed_pdus, 0);
    ASSERT_EQ(m.num_t_reordering_timeouts, 0);
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

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    byte_buffer test_pdu;
    get_test_pdu(count, test_pdu);
    ASSERT_TRUE(test_pdu.append(0)); // mess up the MAC

    // Get expected values
    uint32_t exp_pdu_size = test_pdu.length();

    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu)).value());
    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();

    tick_all(100);
    if (metrics_notif.metrics_list.size() == 0) {
      FAIL() << "No metrics notification received";
    }
    auto m = metrics_notif.metrics_list.back().rx;
    ASSERT_EQ(m.num_pdus, 1);
    ASSERT_EQ(m.num_pdu_bytes, exp_pdu_size);
    ASSERT_EQ(m.num_data_pdus, 1);
    ASSERT_EQ(m.num_data_pdu_bytes, exp_pdu_size);
    ASSERT_EQ(m.num_sdus, 0);
    ASSERT_EQ(m.num_sdu_bytes, 0);
    ASSERT_EQ(m.num_integrity_verified_pdus, 0);
    ASSERT_EQ(m.num_integrity_failed_pdus, 1);
    ASSERT_EQ(m.num_t_reordering_timeouts, 0);
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

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());
    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    ASSERT_EQ(0, test_frame->sdu_queue.size());
    tick_all(150);
    if (metrics_notif.metrics_list.size() == 0) {
      FAIL() << "No metrics notification received";
    }
    auto m = metrics_notif.metrics_list.back().rx;
    ASSERT_EQ(1, m.num_t_reordering_timeouts);
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
  fmt::format_to(std::back_inserter(buffer),
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
