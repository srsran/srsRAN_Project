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

#include "lib/rlc/rlc_um_entity.h"
#include "tests/unittests/rlc/rlc_test_helpers.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <list>
#include <queue>

using namespace srsran;

/// Mocking class of the surrounding layers invoked by the RLC.
class rlc_test_frame : public rlc_rx_upper_layer_data_notifier,
                       public rlc_tx_upper_layer_data_notifier,
                       public rlc_tx_upper_layer_control_notifier,
                       public rlc_tx_lower_layer_notifier
{
public:
  std::queue<byte_buffer_chain> sdu_queue;
  uint32_t                      sdu_counter = 0;
  std::list<uint32_t>           transmitted_pdcp_sn_list;
  uint32_t                      bsr       = 0;
  uint32_t                      bsr_count = 0;

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_chain sdu) override
  {
    sdu_queue.push(std::move(sdu));
    sdu_counter++;
  }

  // rlc_tx_upper_layer_data_notifier interface
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn) override
  {
    // store in list
    transmitted_pdcp_sn_list.push_back(max_tx_pdcp_sn);
  }
  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override {}

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(unsigned bsr_) override
  {
    this->bsr = bsr_;
    this->bsr_count++;
  }
};

/// Fixture class for RLC UM tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class rlc_um_test : public ::testing::Test, public ::testing::WithParamInterface<rlc_um_sn_size>
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init RLC logger
    srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(100);

    logger.info("Creating RLC UM ({} bit)", to_number(sn_size));

    // Set Rx config
    config.rx.sn_field_length = sn_size;
    config.rx.t_reassembly    = 5;

    // Set Tx config
    config.tx.sn_field_length = sn_size;
    config.tx.queue_size      = 4096;

    // Create RLC entities
    rlc1 = std::make_unique<rlc_um_entity>(0,
                                           du_ue_index_t::MIN_DU_UE_INDEX,
                                           srb_id_t::srb0,
                                           config,
                                           std::chrono::milliseconds(1000),
                                           nullptr,
                                           tester1,
                                           tester1,
                                           tester1,
                                           tester1,
                                           timers,
                                           pcell_worker,
                                           ue_worker,
                                           pcap1);
    rlc2 = std::make_unique<rlc_um_entity>(0,
                                           du_ue_index_t::MIN_DU_UE_INDEX,
                                           srb_id_t::srb0,
                                           config,
                                           std::chrono::milliseconds(1000),
                                           nullptr,
                                           tester2,
                                           tester2,
                                           tester2,
                                           tester2,
                                           timers,
                                           pcell_worker,
                                           ue_worker,
                                           pcap2);

    // Bind interfaces
    rlc1_rx_lower = rlc1->get_rx_lower_layer_interface();
    rlc1_tx_upper = rlc1->get_tx_upper_layer_data_interface();
    rlc1_tx_lower = rlc1->get_tx_lower_layer_interface();
    rlc2_rx_lower = rlc2->get_rx_lower_layer_interface();
    rlc2_tx_upper = rlc2->get_tx_upper_layer_data_interface();
    rlc2_tx_lower = rlc2->get_tx_lower_layer_interface();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  /// \brief Constructs a SDU with deterministic content.
  ///
  /// The constructed SDU contains a contiguous sequence of @length bytes starting with the value of @first_byte.
  ///
  /// \param first_byte Value of the first byte in the sequence.
  /// \param length Total length of the sequence in bytes.
  /// \return The constructed byte_buffer object
  byte_buffer construct_sdu(uint8_t first_byte, uint32_t length)
  {
    byte_buffer sdu = {};
    for (uint32_t k = 0; k < length; ++k) {
      report_error_if_not(sdu.append(first_byte + k), "Failed to allocate byte buffer");
    }
    return sdu;
  }

  void tx_with_pdu_duplicates(uint32_t hold_back_pdu)
  {
    uint32_t buffer_state = 0;

    const uint32_t num_sdus = 1;
    const uint32_t sdu_size = 100;

    // Push SDUs into RLC1
    byte_buffer sdu_bufs[num_sdus];
    for (uint32_t i = 0; i < num_sdus; i++) {
      sdu_bufs[i] = byte_buffer();
      // Write the index into the buffer
      for (uint32_t k = 0; k < sdu_size; ++k) {
        ASSERT_TRUE(sdu_bufs[i].append(i + k));
      }

      // write SDU into upper end
      rlc_sdu sdu = {sdu_bufs[i].deep_copy(), 0}; // no std::move - keep local copy for later comparison
      rlc1_tx_upper->handle_sdu(std::move(sdu));
    }
    buffer_state = rlc1_tx_lower->get_buffer_state();
    EXPECT_EQ(num_sdus * (sdu_size + 1), buffer_state);

    // Read PDUs from RLC1 with grant of 25 Bytes each
    const uint32_t    payload_len  = 25;
    const uint32_t    max_num_pdus = 10;
    uint32_t          num_pdus     = 0;
    byte_buffer_chain pdu_bufs[max_num_pdus];

    buffer_state = rlc1_tx_lower->get_buffer_state();
    std::vector<uint8_t> tx_pdu(payload_len);
    while (buffer_state > 0 && num_pdus < max_num_pdus) {
      unsigned n         = rlc1_tx_lower->pull_pdu(tx_pdu);
      pdu_bufs[num_pdus] = byte_buffer_slice{span<uint8_t>{tx_pdu.data(), n}};

      if (pdu_bufs[num_pdus].empty()) {
        break;
      }
      // TODO: write PCAP
      num_pdus++;
      buffer_state = rlc1_tx_lower->get_buffer_state();
    }
    EXPECT_EQ(0, buffer_state);

    // Write all PDUs twice into RLC2 (except for hold_back_pdu)
    for (uint32_t k = 0; k < 2; k++) {
      for (uint32_t i = 0; i < num_pdus; i++) {
        if (i != hold_back_pdu) {
          byte_buffer pdu;
          for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
            pdu.append(slice);
          }
          rlc2_rx_lower->handle_pdu(std::move(pdu));
        }
      }
    }
    buffer_state = rlc2_tx_lower->get_buffer_state();
    EXPECT_EQ(0, buffer_state);

    // Write the skipped PDU into RLC2
    {
      byte_buffer pdu;
      for (const byte_buffer_slice& slice : pdu_bufs[hold_back_pdu].slices()) {
        pdu.append(slice);
      }
      rlc2_rx_lower->handle_pdu(std::move(pdu));
    }

    // Read SDUs from RLC2's upper layer
    EXPECT_EQ(num_sdus, tester2.sdu_counter);
    for (uint32_t i = 0; i < num_sdus; i++) {
      EXPECT_TRUE(tester2.sdu_queue.empty() == false);
      byte_buffer_chain& rx_sdu = tester2.sdu_queue.front();
      EXPECT_EQ(sdu_size, rx_sdu.length());
      EXPECT_TRUE(sdu_bufs[i] == rx_sdu);
      tester2.sdu_queue.pop();
    }
  }

  void tick(uint32_t ticks)
  {
    for (uint32_t i = 0; i < ticks; ++i) {
      tick();
    }
  }

  void tick()
  {
    timers.tick();
    ue_worker.run_pending_tasks();
  }

  srslog::basic_logger&              logger  = srslog::fetch_basic_logger("TEST", false);
  rlc_um_sn_size                     sn_size = GetParam();
  rlc_um_config                      config;
  timer_manager                      timers;
  manual_task_worker                 ue_worker{128};
  manual_task_worker                 pcell_worker{128};
  rlc_test_frame                     tester1, tester2;
  null_rlc_pcap                      pcap1, pcap2;
  std::unique_ptr<rlc_um_entity>     rlc1, rlc2;
  rlc_rx_lower_layer_interface*      rlc1_rx_lower = nullptr;
  rlc_tx_upper_layer_data_interface* rlc1_tx_upper = nullptr;
  rlc_tx_lower_layer_interface*      rlc1_tx_lower = nullptr;
  rlc_rx_lower_layer_interface*      rlc2_rx_lower = nullptr;
  rlc_tx_upper_layer_data_interface* rlc2_tx_upper = nullptr;
  rlc_tx_lower_layer_interface*      rlc2_tx_lower = nullptr;
};

TEST_P(rlc_um_test, create_new_entity)
{
  EXPECT_NE(rlc1_rx_lower, nullptr);
  EXPECT_NE(rlc1_tx_upper, nullptr);
  ASSERT_NE(rlc1_tx_lower, nullptr);
  EXPECT_NE(rlc2_rx_lower, nullptr);
  EXPECT_NE(rlc2_tx_upper, nullptr);
  ASSERT_NE(rlc2_tx_lower, nullptr);

  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester1.bsr, 0);
  EXPECT_EQ(tester1.bsr_count, 0);
  EXPECT_EQ(rlc2_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester2.bsr, 0);
  EXPECT_EQ(tester2.bsr_count, 0);
}

TEST_P(rlc_um_test, rx_empty_pdu)
{
  // Create empty PDU
  byte_buffer pdu_buf = {};

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_buf)};
  rlc2_rx_lower->handle_pdu(std::move(pdu));

  // Read SDUs from RLC2's upper layer
  EXPECT_EQ(tester2.sdu_counter, 0);
}

TEST_P(rlc_um_test, rx_pdu_with_short_header)
{
  // Create a short header of a PDU segment
  byte_buffer pdu_buf = {};
  ASSERT_TRUE(pdu_buf.append(0b11000000)); // SI = 0b11

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_buf)};
  rlc2_rx_lower->handle_pdu(std::move(pdu));

  // Read SDUs from RLC2's upper layer
  EXPECT_EQ(tester2.sdu_counter, 0);
}

TEST_P(rlc_um_test, rx_pdu_without_payload)
{
  // Create a complete header of a PDU segment
  byte_buffer pdu_buf = {};
  ASSERT_TRUE(pdu_buf.append(0b11000000)); // SI = 0b11
  ASSERT_TRUE(pdu_buf.append({0x11, 0x22}));
  if (sn_size == rlc_um_sn_size::size12bits) {
    ASSERT_TRUE(pdu_buf.append(0x33));
  }

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_buf)};
  rlc2_rx_lower->handle_pdu(std::move(pdu));

  // Read SDUs from RLC2's upper layer
  EXPECT_EQ(tester2.sdu_counter, 0);
}

TEST_P(rlc_um_test, tx_without_segmentation)
{
  const uint32_t num_sdus = 5;
  const uint32_t num_pdus = 5;
  const uint32_t sdu_size = 1;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    for (uint32_t k = 0; k < sdu_size; ++k) {
      ASSERT_TRUE(sdu_bufs[i].append(i + k));
    }

    // write SDU into upper end
    rlc_sdu sdu = {sdu_bufs[i].deep_copy(), i + 13}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Read PDUs from RLC1
  byte_buffer_chain    pdu_bufs[num_pdus];
  const int            payload_len = 1 + sdu_size; // 1 bytes for header + payload
  std::vector<uint8_t> tx_pdu(payload_len);
  for (uint32_t i = 0; i < num_pdus; i++) {
    unsigned nwritten = rlc1_tx_lower->pull_pdu(tx_pdu);
    pdu_bufs[i]       = byte_buffer_slice{span<uint8_t>{tx_pdu.data(), nwritten}};
    EXPECT_EQ(payload_len, pdu_bufs[i].length());
    EXPECT_EQ(i, pdu_bufs[i][payload_len - 1]); // check if last payload item corresponds with index

    // Verify transmit notification
    EXPECT_EQ(1, tester1.transmitted_pdcp_sn_list.size());
    EXPECT_EQ(i + 13, tester1.transmitted_pdcp_sn_list.front());
    tester1.transmitted_pdcp_sn_list.pop_front();

    // TODO: write PCAP
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Write PDUs into RLC2
  for (uint32_t i = 0; i < num_pdus; i++) {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc2_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester2.bsr, 0);
  EXPECT_EQ(tester2.bsr_count, 0);

  // Read SDUs from RLC2's upper layer
  EXPECT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    EXPECT_TRUE(tester2.sdu_queue.empty() == false);
    byte_buffer_chain& rx_sdu = tester2.sdu_queue.front();
    EXPECT_EQ(sdu_size, rx_sdu.length());
    EXPECT_TRUE(sdu_bufs[i] == rx_sdu);
    tester2.sdu_queue.pop();
  }
}

TEST_P(rlc_um_test, tx_with_segmentation)
{
  const uint32_t num_sdus = 1;
  const uint32_t sdu_size = 100;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      ASSERT_TRUE(sdu_bufs[i].append(i + k));
    }

    // write SDU into upper end
    rlc_sdu sdu = {sdu_bufs[i].deep_copy(), i}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Read PDUs from RLC1 with grant of 25 Bytes each
  const uint32_t    payload_len  = 25;
  const uint32_t    max_num_pdus = 10;
  uint32_t          num_pdus     = 0;
  byte_buffer_chain pdu_bufs[max_num_pdus];

  std::vector<uint8_t> tx_pdu(payload_len);
  while (rlc1_tx_lower->get_buffer_state() > 0 && num_pdus < max_num_pdus) {
    unsigned nwritten  = rlc1_tx_lower->pull_pdu(tx_pdu);
    pdu_bufs[num_pdus] = byte_buffer_slice{span<uint8_t>(tx_pdu.data(), nwritten)};

    if (num_pdus % ((sdu_size / payload_len) + 1) == 0) {
      // Verify transmit notification
      EXPECT_EQ(1, tester1.transmitted_pdcp_sn_list.size());
      EXPECT_EQ(num_pdus, tester1.transmitted_pdcp_sn_list.front());
      tester1.transmitted_pdcp_sn_list.pop_front();
    }

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Verify there are no multiple transmit notifications
  EXPECT_EQ(0, tester1.transmitted_pdcp_sn_list.size());

  // Write PDUs into RLC2
  // receive PDUs in order
  for (uint32_t i = 0; i < num_pdus; i++) {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc2_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester2.bsr, 0);
  EXPECT_EQ(tester2.bsr_count, 0);

  // Read SDUs from RLC2's upper layer
  EXPECT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    EXPECT_TRUE(tester2.sdu_queue.empty() == false);
    byte_buffer_chain& rx_sdu = tester2.sdu_queue.front();
    EXPECT_EQ(sdu_size, rx_sdu.length());
    EXPECT_TRUE(sdu_bufs[i] == rx_sdu);
    tester2.sdu_queue.pop();
  }
}

TEST_P(rlc_um_test, sdu_discard)
{
  const uint32_t num_sdus = 6;
  const uint32_t sdu_size = 1;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    for (uint32_t k = 0; k < sdu_size; ++k) {
      ASSERT_TRUE(sdu_bufs[i].append(i + k));
    }

    // write SDU into upper end
    rlc_sdu sdu = {sdu_bufs[i].deep_copy(), i}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }

  tester1.bsr_count = 0; // reset

  // Discard valid SDUs
  rlc1_tx_upper->discard_sdu(0);
  rlc1_tx_upper->discard_sdu(4); // out-of-order discard
  rlc1_tx_upper->discard_sdu(3);

  // Compute expected buffer state
  uint32_t header_size         = 1;
  uint32_t data_pdu_size       = header_size + sdu_size;
  uint32_t expect_buffer_state = (num_sdus - 3) * data_pdu_size;

  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester1.bsr, expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 0);

  // Try discard of invalid SDU
  rlc1_tx_upper->discard_sdu(999);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester1.bsr, expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 1);

  // Try discard of already discarded SDU
  rlc1_tx_upper->discard_sdu(0);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester1.bsr, expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 2);

  // Transmit full PDU
  std::vector<uint8_t> tx_pdu(data_pdu_size);
  unsigned             nwritten = rlc1_tx_lower->pull_pdu(tx_pdu);
  byte_buffer_chain    pdu      = byte_buffer_slice{span<uint8_t>(tx_pdu.data(), nwritten)};
  EXPECT_FALSE(pdu.empty());
  EXPECT_TRUE(std::equal(pdu.begin() + header_size, pdu.end(), sdu_bufs[1].begin()));
  expect_buffer_state = (num_sdus - 4) * data_pdu_size;

  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 2);

  // Try discard of already transmitted SDU
  rlc1_tx_upper->discard_sdu(1);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 3);

  // Transmit full PDU
  tx_pdu.resize(data_pdu_size);
  nwritten = rlc1_tx_lower->pull_pdu(tx_pdu);
  pdu      = byte_buffer_slice{span<uint8_t>(tx_pdu.data(), nwritten)};
  EXPECT_FALSE(pdu.empty());
  EXPECT_TRUE(std::equal(pdu.begin() + header_size, pdu.end(), sdu_bufs[2].begin()));
  expect_buffer_state = (num_sdus - 5) * data_pdu_size;

  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 3);

  // Discard remaining SDU
  rlc1_tx_upper->discard_sdu(5);
  expect_buffer_state = 0;

  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester1.bsr, expect_buffer_state);
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 2);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 4);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 3);
}

TEST_P(rlc_um_test, sdu_discard_with_pdcp_sn_wraparound)
{
  const uint32_t num_sdus = 6;
  const uint32_t sdu_size = 1;

  const uint32_t pdcp_sn_start = 4092;
  const uint32_t pdcp_sn_mod   = 4096;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    for (uint32_t k = 0; k < sdu_size; ++k) {
      ASSERT_TRUE(sdu_bufs[i].append(i + k));
    }

    // write SDU into upper end
    rlc_sdu sdu = {sdu_bufs[i].deep_copy(),
                   /* pdcp_sn = */ (pdcp_sn_start + i) % pdcp_sn_mod}; // no std::move, keep copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }

  tester1.bsr_count = 0; // reset

  // Discard valid SDUs
  rlc1_tx_upper->discard_sdu((pdcp_sn_start + 0) % pdcp_sn_mod);
  rlc1_tx_upper->discard_sdu((pdcp_sn_start + 4) % pdcp_sn_mod); // post-wraparound discard
  rlc1_tx_upper->discard_sdu((pdcp_sn_start + 3) % pdcp_sn_mod); // out-of-order discard (pre-wraparound)

  // Compute expected buffer state
  uint32_t header_size         = 1;
  uint32_t data_pdu_size       = header_size + sdu_size;
  uint32_t expect_buffer_state = (num_sdus - 3) * data_pdu_size;

  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester1.bsr, expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 0);

  // Try discard of invalid SDU
  rlc1_tx_upper->discard_sdu((pdcp_sn_start + 999) % pdcp_sn_mod);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester1.bsr, expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 1);

  // Try discard of already discarded SDU
  rlc1_tx_upper->discard_sdu((pdcp_sn_start + 0) % pdcp_sn_mod);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester1.bsr, expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 2);

  // Transmit full PDU
  std::vector<uint8_t> tx_pdu(data_pdu_size);
  unsigned             nwritten = rlc1_tx_lower->pull_pdu(tx_pdu);
  byte_buffer_chain    pdu      = byte_buffer_slice{span<uint8_t>(tx_pdu.data(), nwritten)};
  EXPECT_FALSE(pdu.empty());
  EXPECT_TRUE(std::equal(pdu.begin() + header_size, pdu.end(), sdu_bufs[1].begin()));
  expect_buffer_state = (num_sdus - 4) * data_pdu_size;

  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 2);

  // Try discard of already transmitted SDU
  rlc1_tx_upper->discard_sdu((pdcp_sn_start + 1) % pdcp_sn_mod);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 3);

  // Transmit full PDU
  nwritten = rlc1_tx_lower->pull_pdu(tx_pdu);
  pdu      = byte_buffer_slice{span<uint8_t>(tx_pdu.data(), nwritten)};
  EXPECT_FALSE(pdu.empty());
  EXPECT_TRUE(std::equal(pdu.begin() + header_size, pdu.end(), sdu_bufs[2].begin()));
  expect_buffer_state = (num_sdus - 5) * data_pdu_size;

  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 1);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 3);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 3);

  // Discard remaining SDU
  rlc1_tx_upper->discard_sdu((pdcp_sn_start + 5) % pdcp_sn_mod);
  expect_buffer_state = 0;

  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester1.bsr_count, 2);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discarded_sdus, 4);
  EXPECT_EQ(rlc1->get_metrics().tx.num_discard_failures, 3);
}

TEST_P(rlc_um_test, tx_with_segmentation_reverse_rx)
{
  const uint32_t num_sdus = 1;
  const uint32_t sdu_size = 100;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      ASSERT_TRUE(sdu_bufs[i].append(i + k));
    }

    // write SDU into upper end
    rlc_sdu sdu = {sdu_bufs[i].deep_copy(), i}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Read PDUs from RLC1 with grant of 25 Bytes each
  const uint32_t    payload_len  = 25;
  const uint32_t    max_num_pdus = 10;
  uint32_t          num_pdus     = 0;
  byte_buffer_chain pdu_bufs[max_num_pdus];

  std::vector<uint8_t> tx_pdu(payload_len);
  while (rlc1_tx_lower->get_buffer_state() > 0 && num_pdus < max_num_pdus) {
    unsigned nwritten  = rlc1_tx_lower->pull_pdu(tx_pdu);
    pdu_bufs[num_pdus] = byte_buffer_slice{span<uint8_t>(tx_pdu.data(), nwritten)};

    if (num_pdus % ((sdu_size / payload_len) + 1) == 0) {
      // Verify transmit notification
      EXPECT_EQ(1, tester1.transmitted_pdcp_sn_list.size());
      EXPECT_EQ(num_pdus, tester1.transmitted_pdcp_sn_list.front());
      tester1.transmitted_pdcp_sn_list.pop_front();
    }

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Verify there are no multiple transmit notifications
  EXPECT_EQ(0, tester1.transmitted_pdcp_sn_list.size());

  // Write PDUs into RLC2
  // receive PDUs in reverse order
  for (uint32_t i = 0; i < num_pdus; i++) {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[num_pdus - i - 1].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc2_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester2.bsr, 0);
  EXPECT_EQ(tester2.bsr_count, 0);

  // Read SDUs from RLC2's upper layer
  ASSERT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    EXPECT_TRUE(tester2.sdu_queue.empty() == false);
    byte_buffer_chain& rx_sdu = tester2.sdu_queue.front();
    EXPECT_EQ(sdu_size, rx_sdu.length());
    EXPECT_TRUE(sdu_bufs[i] == rx_sdu);
    tester2.sdu_queue.pop();
  }
}

TEST_P(rlc_um_test, tx_multiple_SDUs_with_segmentation)
{
  const uint32_t num_sdus = 2;
  const uint32_t sdu_size = 100;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      ASSERT_TRUE(sdu_bufs[i].append(i + k));
    }

    // write SDU into upper end
    rlc_sdu sdu = {sdu_bufs[i].deep_copy(), 0}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Read PDUs from RLC1 with grant of 25 Bytes each
  const uint32_t    payload_len  = 25;
  const uint32_t    max_num_pdus = 20;
  uint32_t          num_pdus     = 0;
  byte_buffer_chain pdu_bufs[max_num_pdus];

  std::vector<uint8_t> tx_pdu(payload_len);
  while (rlc1_tx_lower->get_buffer_state() > 0 && num_pdus < max_num_pdus) {
    unsigned nwritten  = rlc1_tx_lower->pull_pdu(tx_pdu);
    pdu_bufs[num_pdus] = byte_buffer_slice{span<uint8_t>(tx_pdu.data(), nwritten)};

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Write PDUs into RLC2 (except 1 and 6)
  for (uint32_t i = 0; i < num_pdus; i++) {
    if (i != 1 && i != 6) {
      byte_buffer pdu;
      for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
        pdu.append(slice);
      }
      rlc2_rx_lower->handle_pdu(std::move(pdu));
    }
  }

  // write remaining two PDUs in reverse-order (so SN=1 is received first)
  {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[6].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }
  {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[1].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }
  pcell_worker.run_pending_tasks();

  EXPECT_EQ(rlc2_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester2.bsr, 0);
  EXPECT_EQ(tester2.bsr_count, 0);

  // Read SDUs from RLC2's upper layer
  // Upper layer receives SDUs in order of completion (i.e. here in reverse order)
  // Therefore compare with initial testvectors in reverse order
  ASSERT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    EXPECT_TRUE(tester2.sdu_queue.empty() == false);
    byte_buffer_chain& rx_sdu = tester2.sdu_queue.front();
    EXPECT_EQ(sdu_size, rx_sdu.length());
    EXPECT_TRUE(sdu_bufs[num_sdus - 1 - i] == rx_sdu);
    tester2.sdu_queue.pop();
  }
}

TEST_P(rlc_um_test, tx_with_PDU_duplicates_hold_0)
{
  tx_with_pdu_duplicates(0);
}

TEST_P(rlc_um_test, tx_with_PDU_duplicates_hold_1)
{
  tx_with_pdu_duplicates(1);
}

TEST_P(rlc_um_test, tx_with_PDU_duplicates_hold_2)
{
  tx_with_pdu_duplicates(2);
}

TEST_P(rlc_um_test, tx_with_PDU_duplicates_hold_3)
{
  tx_with_pdu_duplicates(3);
}

TEST_P(rlc_um_test, tx_with_PDU_duplicates_hold_4)
{
  tx_with_pdu_duplicates(4);
}

TEST_P(rlc_um_test, reassembly_window_wrap_around)
{
  const uint32_t num_sdus = cardinality(to_number(sn_size)); // 2 * UM_Window_Size
  const uint32_t sdu_size = 10;

  const uint32_t payload_len  = 8;
  const uint32_t max_num_pdus = num_sdus * 2; // we need 2 PDUs for each SDU
  uint32_t       num_pdus     = 0;

  // Push SDUs into RLC1 and read PDUs from RLC1 with grant of 8 Bytes each
  uint32_t rx_sdu_idx = 0;
  for (uint32_t i = 0; i < num_sdus; i++) {
    // create and write SDU into upper end
    rlc_sdu sdu = {construct_sdu(i, sdu_size), 0};
    rlc1_tx_upper->handle_sdu(std::move(sdu));
    pcell_worker.run_pending_tasks();

    // check buffer state
    EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), sdu_size + 1);
    EXPECT_EQ(tester1.bsr, sdu_size + 1);
    EXPECT_EQ(tester1.bsr_count, i + 1);

    // read PDUs from lower end of RLC1 and write into lower end of RLC2
    std::vector<uint8_t> pdu_tmp(payload_len);
    while (rlc1_tx_lower->get_buffer_state() > 0 && num_pdus < max_num_pdus) {
      unsigned          nwritten = rlc1_tx_lower->pull_pdu(pdu_tmp);
      byte_buffer_chain tx_pdu   = byte_buffer_slice{span<uint8_t>(pdu_tmp.data(), nwritten)};
      if (tx_pdu.empty()) {
        break;
      }

      byte_buffer rx_pdu;
      for (const byte_buffer_slice& slice : tx_pdu.slices()) {
        rx_pdu.append(slice);
      }
      rlc2_rx_lower->handle_pdu(std::move(rx_pdu));

      // TODO: write PCAP
      num_pdus++;

      // Check upper layer of RLC2 for new SDUs and validate the content
      while (!tester2.sdu_queue.empty() && rx_sdu_idx < num_sdus) {
        byte_buffer_chain& rx_sdu = tester2.sdu_queue.front();
        EXPECT_EQ(sdu_size, rx_sdu.length());
        EXPECT_TRUE(rx_sdu == construct_sdu(rx_sdu_idx, sdu_size));
        tester2.sdu_queue.pop();
        rx_sdu_idx++;
      }
    }
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester1.bsr, sdu_size + 1);
  EXPECT_EQ(tester1.bsr_count, num_sdus);

  // Check number of received SDUs
  EXPECT_EQ(num_sdus, tester2.sdu_counter);

  EXPECT_EQ(rlc2_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester2.bsr, 0);
  EXPECT_EQ(tester2.bsr_count, 0);
}

TEST_P(rlc_um_test, lost_PDU_outside_reassembly_window)
{
  const uint32_t num_sdus = cardinality(to_number(sn_size)); // 2 * UM_Window_Size
  const uint32_t sdu_size = 10;

  const uint32_t payload_len  = 8;
  const uint32_t max_num_pdus = num_sdus * 2; // we need 2 PDUs for each SDU
  uint32_t       num_pdus     = 0;

  // Push SDUs into RLC1 and read PDUs from RLC1 with grant of 8 Bytes each
  uint32_t rx_sdu_idx = 0;
  for (uint32_t i = 0; i < num_sdus; i++) {
    // create and write SDU into upper end
    rlc_sdu sdu = {construct_sdu(i, sdu_size), 0};
    rlc1_tx_upper->handle_sdu(std::move(sdu));
    pcell_worker.run_pending_tasks();

    // check buffer state
    EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), sdu_size + 1);
    EXPECT_EQ(tester1.bsr, sdu_size + 1);
    EXPECT_EQ(tester1.bsr_count, i + 1);

    // read PDUs from lower end of RLC1 and write into lower end of RLC2 (except 11th and 22th)
    std::vector<uint8_t> pdu_tmp(payload_len);
    while (rlc1_tx_lower->get_buffer_state() > 0 && num_pdus < max_num_pdus) {
      unsigned          nwritten = rlc1_tx_lower->pull_pdu(pdu_tmp);
      byte_buffer_chain tx_pdu   = byte_buffer_slice{span<uint8_t>(pdu_tmp.data(), nwritten)};
      if (tx_pdu.empty()) {
        break;
      }

      if (num_pdus != 10 && num_pdus != 21) {
        byte_buffer rx_pdu;
        for (const byte_buffer_slice& slice : tx_pdu.slices()) {
          rx_pdu.append(slice);
        }
        rlc2_rx_lower->handle_pdu(std::move(rx_pdu));
      } else {
        // increment rx_sdu_idx to skip content check of the SDU that will be lost
        rx_sdu_idx++;
      }

      // TODO: write PCAP
      num_pdus++;

      // Check upper layer of RLC2 for new SDUs and validate the content
      while (!tester2.sdu_queue.empty() && rx_sdu_idx < num_sdus) {
        byte_buffer_chain& rx_sdu = tester2.sdu_queue.front();
        EXPECT_EQ(sdu_size, rx_sdu.length());
        EXPECT_TRUE(rx_sdu == construct_sdu(rx_sdu_idx, sdu_size));
        tester2.sdu_queue.pop();
        rx_sdu_idx++;
      }
    }
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester1.bsr, sdu_size + 1);
  EXPECT_EQ(tester1.bsr_count, num_sdus);

  // Check number of received SDUs
  EXPECT_EQ(num_sdus - 2, tester2.sdu_counter);

  EXPECT_EQ(rlc2_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester2.bsr, 0);
  EXPECT_EQ(tester2.bsr_count, 0);

  // let t-reassembly expire
  tick(config.rx.t_reassembly);

  rlc_metrics rlc2_metrics = rlc2->get_metrics();
  EXPECT_TRUE(rlc2_metrics.rx.num_lost_pdus == 2);
}

TEST_P(rlc_um_test, lost_segment_outside_reassembly_window)
{
  const uint32_t num_sdus = 10;
  const uint32_t sdu_size = 10;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      ASSERT_TRUE(sdu_bufs[i].append(i + k));
    }

    // write SDU into upper end
    rlc_sdu sdu = {sdu_bufs[i].deep_copy(), 0}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Read PDUs from RLC1 with grant of 8 Bytes each
  const uint32_t    payload_len  = 8;
  const uint32_t    max_num_pdus = num_sdus * 2; // we need 2 PDUs for each SDU
  uint32_t          num_pdus     = 0;
  byte_buffer_chain pdu_bufs[max_num_pdus];

  std::vector<uint8_t> pdu_tmp(payload_len);
  while (rlc1_tx_lower->get_buffer_state() > 0 && num_pdus < max_num_pdus) {
    unsigned nwritten  = rlc1_tx_lower->pull_pdu(pdu_tmp);
    pdu_bufs[num_pdus] = byte_buffer_slice{span<uint8_t>(pdu_tmp.data(), nwritten)};

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Write PDUs into RLC2 (except 2nd)
  for (uint32_t i = 0; i < num_pdus; i++) {
    if (i != 2) {
      byte_buffer pdu;
      for (const byte_buffer_slice& slice : pdu_bufs[i].slices()) {
        pdu.append(slice);
      }
      rlc2_rx_lower->handle_pdu(std::move(pdu));
    }
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc2_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester2.bsr, 0);
  EXPECT_EQ(tester2.bsr_count, 0);

  // let t-reassembly expire
  tick(config.rx.t_reassembly);

  // Read SDUs from RLC2's upper layer
  EXPECT_EQ(num_sdus - 1, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    if (i != 1) {
      EXPECT_TRUE(tester2.sdu_queue.empty() == false);
      byte_buffer_chain& rx_sdu = tester2.sdu_queue.front();
      EXPECT_EQ(sdu_size, rx_sdu.length());
      EXPECT_TRUE(sdu_bufs[i] == rx_sdu);
      tester2.sdu_queue.pop();
    }
  }

  rlc_metrics rlc2_metrics = rlc2->get_metrics();
  EXPECT_TRUE(rlc2_metrics.rx.num_lost_pdus == 1);
}

TEST_P(rlc_um_test, out_of_order_segments_across_SDUs)
{
  const uint32_t num_sdus = 2;
  const uint32_t sdu_size = 20;

  // Push SDUs into RLC1
  byte_buffer sdu_bufs[num_sdus];
  for (uint32_t i = 0; i < num_sdus; i++) {
    sdu_bufs[i] = byte_buffer();
    // Write the index into the buffer
    for (uint32_t k = 0; k < sdu_size; ++k) {
      ASSERT_TRUE(sdu_bufs[i].append(i + k));
    }

    // write SDU into upper end
    rlc_sdu sdu = {sdu_bufs[i].deep_copy(), 0}; // no std::move - keep local copy for later comparison
    rlc1_tx_upper->handle_sdu(std::move(sdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Read PDUs from RLC1 with grant smaller than SDU size
  const uint32_t    payload_len  = 10;
  const uint32_t    max_num_pdus = 10;
  uint32_t          num_pdus     = 0;
  byte_buffer_chain pdu_bufs[max_num_pdus];

  std::vector<uint8_t> pdu_tmp(payload_len);
  while (rlc1_tx_lower->get_buffer_state() > 0 && num_pdus < max_num_pdus) {
    unsigned nwritten  = rlc1_tx_lower->pull_pdu(pdu_tmp);
    pdu_bufs[num_pdus] = byte_buffer_slice{span<uint8_t>(pdu_tmp.data(), nwritten)};

    if (pdu_bufs[num_pdus].empty()) {
      break;
    }
    // TODO: write PCAP
    num_pdus++;
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(6, num_pdus);
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester1.bsr, num_sdus * (sdu_size + 1));
  EXPECT_EQ(tester1.bsr_count, 1);

  // Write all PDUs such that the middle section of SN=0 is received after the start section of SN=1
  //                    +------------------- skip 2nd PDU which is the middle section of SN=0
  //                    |      +------------ now feed the middle part of SN=0
  //                    |      |  +--------- and the rest of SN=1
  //                    V      V  V
  const uint32_t order[] = {0, 2, 3, 1, 4, 5};

  // Write PDUs into RLC2 (except 2nd)
  for (uint32_t i = 0; i < num_pdus; i++) {
    byte_buffer pdu;
    for (const byte_buffer_slice& slice : pdu_bufs[order[i]].slices()) {
      pdu.append(slice);
    }
    rlc2_rx_lower->handle_pdu(std::move(pdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc2_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(tester2.bsr, 0);
  EXPECT_EQ(tester2.bsr_count, 0);

  // Read SDUs from RLC2's upper layer
  EXPECT_EQ(num_sdus, tester2.sdu_counter);
  for (uint32_t i = 0; i < num_sdus; i++) {
    EXPECT_TRUE(tester2.sdu_queue.empty() == false);
    byte_buffer_chain& rx_sdu = tester2.sdu_queue.front();
    EXPECT_EQ(sdu_size, rx_sdu.length());
    EXPECT_TRUE(sdu_bufs[i] == rx_sdu);
    tester2.sdu_queue.pop();
  }

  rlc_metrics rlc2_metrics = rlc2->get_metrics();
  EXPECT_TRUE(rlc2_metrics.rx.num_lost_pdus == 0);

  EXPECT_EQ(0, timers.nof_running_timers());
}

///////////////////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size
///////////////////////////////////////////////////////////////////////////////

std::string test_param_info_to_string(const ::testing::TestParamInfo<rlc_um_sn_size>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "{}bit", to_number(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(rlc_um_test_each_sn_size,
                         rlc_um_test,
                         ::testing::Values(rlc_um_sn_size::size6bits, rlc_um_sn_size::size12bits),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
