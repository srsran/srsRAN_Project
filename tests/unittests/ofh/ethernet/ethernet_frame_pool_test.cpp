/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ether;

// Output stream formatting to make GTest results human-readable.
namespace srsran {

std::ostream& operator<<(std::ostream& os, const subcarrier_spacing& scs)
{
  return os << fmt::format("scs={}", to_string(scs));
}
std::ostream& operator<<(std::ostream& os, const cyclic_prefix& cp)
{
  return os << fmt::format("cp={}", cp.to_string());
}

namespace ofh {
std::ostream& operator<<(std::ostream& os, const message_type& t)
{
  return os << fmt::format("ofh={}", t == message_type::control_plane ? "control" : "user");
}
} // namespace ofh

} // namespace srsran

/// Write random data to the Ethernet frame buffers.
static void init_eth_buffers_with_rnd_data(span<frame_buffer>         frame_buffers,
                                           span<std::vector<uint8_t>> test_data,
                                           unsigned                   num_frames)
{
  for (unsigned frame = 0; frame < num_frames; ++frame) {
    unsigned pkt_size    = test_rgen::uniform_int<unsigned>(MIN_ETH_FRAME_LENGTH, MAX_ETH_FRAME_LENGTH);
    test_data[frame]     = test_rgen::random_vector<uint8_t>(pkt_size);
    span<uint8_t> buffer = frame_buffers[frame].data().first(pkt_size);
    std::copy(test_data[frame].begin(), test_data[frame].end(), buffer.begin());
    frame_buffers[frame].set_size(pkt_size);
  }
}

namespace {
constexpr size_t TEST_NUM_SLOTS      = 40;
constexpr size_t MAX_PACKETS_PER_SYM = std::max(ofh::MAX_CP_MESSAGES_PER_SYMBOL, ofh::MAX_UP_MESSAGES_PER_SYMBOL);

using test_params_t = std::tuple<subcarrier_spacing, cyclic_prefix, ofh::message_type>;

class EthFramePoolFixture : public ::testing::TestWithParam<test_params_t>
{
protected:
  subcarrier_spacing scs         = std::get<0>(GetParam());
  cyclic_prefix      cp          = std::get<1>(GetParam());
  ofh::message_type  ofh_type    = std::get<2>(GetParam());
  unsigned           nof_symbols = get_nsymb_per_slot(cp);
  eth_frame_pool     pool        = {};
};

// Verify reading from empty pool returns an empty span.
TEST_P(EthFramePoolFixture, read_empty_pool_should_return_empty_span)
{
  slot_point slot(to_numerology_value(scs), 0);
  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      span<const span<const frame_buffer>> buffers = pool.read_frame_buffers(slot, symbol, ofh_type);
      ASSERT_TRUE(buffers.empty()) << "Reading empty pool returned non-empty span of buffers";
    }
    ++slot;
  }
}

// Verify that written small packet size is rounded to 64 Bytes.
TEST_P(EthFramePoolFixture, writing_small_pkt_is_rounded_to_min_eth_size)
{
  // Initialize slot to a random value.
  slot_point slot(to_numerology_value(scs), 0);
  unsigned   slots_per_system_frame = slot.nof_slots_per_system_frame();
  slot += test_rgen::uniform_int<unsigned>(0, slots_per_system_frame);

  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      // Generate small packet filled with random data.
      std::array<uint8_t, MIN_ETH_FRAME_LENGTH> zeros{0};
      unsigned                                  pkt_size = test_rgen::uniform_int<unsigned>(1, MIN_ETH_FRAME_LENGTH);
      std::vector<uint8_t>                      pkt_data = test_rgen::random_vector<uint8_t>(pkt_size);
      std::copy(pkt_data.begin(), pkt_data.end(), zeros.begin());

      // Get span of frame buffers.
      span<frame_buffer> wr_buffers = pool.get_frame_buffers(slot, symbol, ofh_type);
      // Write random data to the first buffer.
      span<uint8_t> wr_data_buffer = wr_buffers[0].data().first(pkt_size);
      std::copy(pkt_data.begin(), pkt_data.end(), wr_data_buffer.begin());
      wr_buffers[0].set_size(pkt_size);
      pool.eth_frames_ready(slot, symbol, ofh_type, wr_buffers);

      // Read frame buffers and verify length and content of the first one.
      span<const span<const frame_buffer>> frame_buffers = pool.read_frame_buffers(slot, symbol, ofh_type);
      ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";

      span<const frame_buffer> rd_buffers = frame_buffers[0];
      ASSERT_TRUE(!rd_buffers.empty()) << "Non-empty span of buffers expected";

      ASSERT_EQ(rd_buffers[0].size(), MIN_ETH_FRAME_LENGTH) << "Expected size is {}" << MIN_ETH_FRAME_LENGTH;
      ASSERT_TRUE(std::equal(rd_buffers[0].data().begin(), rd_buffers[0].data().end(), zeros.begin()))
          << "Data mismatch";
      pool.eth_frames_sent(slot, symbol, ofh_type);
    }
    ++slot;
  }
}

// Verify reading the data right after writing it.
TEST_P(EthFramePoolFixture, read_after_write_should_return_correct_data)
{
  // Initialize slot to a random value.
  slot_point slot(to_numerology_value(scs), 0);
  unsigned   slots_per_system_frame = slot.nof_slots_per_system_frame();
  slot += test_rgen::uniform_int<unsigned>(0, slots_per_system_frame);

  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      // Get span of frame buffers.
      span<frame_buffer> frame_buffers = pool.get_frame_buffers(slot, symbol, ofh_type);
      std::array<std::vector<uint8_t>, MAX_PACKETS_PER_SYM> test_data;

      // Number of Ethernet frames written in the current slot symbol.
      unsigned max_num_packets = ofh_type == ofh::message_type::control_plane ? 1 : ofh::MAX_UP_MESSAGES_PER_SYMBOL;
      unsigned num_frames      = test_rgen::uniform_int<unsigned>(1, max_num_packets);

      // Write random data to the buffers.
      init_eth_buffers_with_rnd_data(frame_buffers, test_data, num_frames);
      pool.eth_frames_ready(slot, symbol, ofh_type, frame_buffers);

      // Read frame buffers from the pool.
      span<const span<const frame_buffer>> prepared_frame_buffers = pool.read_frame_buffers(slot, symbol, ofh_type);
      ASSERT_TRUE(prepared_frame_buffers.size() == 1) << "One batch of frame buffers is expected";

      span<const frame_buffer> rd_frame_buffers = prepared_frame_buffers[0];
      EXPECT_TRUE(!rd_frame_buffers.empty()) << "Expected non empty span of ethernet frame buffers";
      ASSERT_TRUE(rd_frame_buffers.size() <= MAX_PACKETS_PER_SYM)
          << "Reading from the pool returned more buffers than expected";

      for (unsigned i = 0; i != rd_frame_buffers.size(); ++i) {
        // Size and data should match the randomly generated ones.
        EXPECT_EQ(test_data[i].size(), rd_frame_buffers[i].size())
            << "Size of read packet doesn't match the size of written vector";
        if (!test_data[i].empty()) {
          ASSERT_TRUE(
              std::equal(rd_frame_buffers[i].data().begin(), rd_frame_buffers[i].data().end(), test_data[i].begin()))
              << "Data mismatch";
        }
      }
      pool.eth_frames_sent(slot, symbol, ofh_type);
    }
    ++slot;
  }
}

// Verify writing sequentially two batches of packets and then reading all of them.
TEST_P(EthFramePoolFixture, read_two_sequentially_written_packets_should_return_correct_data)
{
  // Initialize slot to a random value.
  slot_point slot(to_numerology_value(scs), 0);
  unsigned   slots_per_system_frame = slot.nof_slots_per_system_frame();
  slot += test_rgen::uniform_int<unsigned>(0, slots_per_system_frame);

  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      // Random test data for first and second batch of frame_buffers.
      std::array<std::vector<uint8_t>, MAX_PACKETS_PER_SYM> test_data_0;
      std::array<std::vector<uint8_t>, MAX_PACKETS_PER_SYM> test_data_1;

      // Number of Ethernet frames written in the current slot symbol.
      unsigned max_num_packets = ofh_type == ofh::message_type::control_plane ? 1 : ofh::MAX_UP_MESSAGES_PER_SYMBOL;
      unsigned num_frames      = test_rgen::uniform_int<unsigned>(1, max_num_packets);

      /// Get first batch of frame buffers for the current slot symbol.
      span<frame_buffer> frame_buffers = pool.get_frame_buffers(slot, symbol, ofh_type);
      // Write random data.
      init_eth_buffers_with_rnd_data(frame_buffers, test_data_0, num_frames);
      pool.eth_frames_ready(slot, symbol, ofh_type, frame_buffers);

      /// Get second batch of frame buffers for the current slot symbol.
      frame_buffers = pool.get_frame_buffers(slot, symbol, ofh_type);
      // Write random data.
      init_eth_buffers_with_rnd_data(frame_buffers, test_data_1, num_frames);
      pool.eth_frames_ready(slot, symbol, ofh_type, frame_buffers);

      /// Read frame buffers from the pool and verify data.
      span<const span<const frame_buffer>> prepared_frame_buffers = pool.read_frame_buffers(slot, symbol, ofh_type);
      ASSERT_TRUE(prepared_frame_buffers.size() == 2) << "Two batches of packets expected";
      for (unsigned i = 0; i < 2; ++i) {
        span<const frame_buffer> rd_frame_buffers = prepared_frame_buffers[i];
        EXPECT_TRUE(!rd_frame_buffers.empty()) << "Expected non empty span of ethernet frame buffers";
        ASSERT_TRUE(rd_frame_buffers.size() == max_num_packets)
            << "Reading from the pool returned incorrect number of buffers";

        std::array<std::vector<uint8_t>, MAX_PACKETS_PER_SYM>& test_data = (i == 0) ? test_data_0 : test_data_1;
        for (unsigned j = 0; j != rd_frame_buffers.size(); ++j) {
          // Size and data should match the randomly generated ones.
          EXPECT_EQ(test_data[j].size(), rd_frame_buffers[j].size())
              << "Size of read packet doesn't match the size of written vector";
          if (!test_data[j].empty()) {
            ASSERT_TRUE(
                std::equal(rd_frame_buffers[j].data().begin(), rd_frame_buffers[j].data().end(), test_data[j].begin()))
                << "Data mismatch";
          }
        }
      }
      pool.eth_frames_sent(slot, symbol, ofh_type);
    }
    ++slot;
  }
}

TEST_P(EthFramePoolFixture, read_two_packets_written_in_parallel_should_return_correct_data)
{
  // Initialize slot to a random value.
  slot_point slot(to_numerology_value(scs), 0);
  unsigned   slots_per_system_frame = slot.nof_slots_per_system_frame();
  slot += test_rgen::uniform_int<unsigned>(0, slots_per_system_frame);

  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      // Random test data for first and second batch of frame_buffers.
      std::array<std::vector<uint8_t>, MAX_PACKETS_PER_SYM> test_data_0;
      std::array<std::vector<uint8_t>, MAX_PACKETS_PER_SYM> test_data_1;

      // Number of Ethernet frames written in the current slot symbol.
      unsigned max_num_packets = ofh_type == ofh::message_type::control_plane ? 1 : ofh::MAX_UP_MESSAGES_PER_SYMBOL;
      unsigned num_frames      = test_rgen::uniform_int<unsigned>(1, max_num_packets);

      /// Write two batches of frame buffers for the current slot symbol.
      span<frame_buffer> frame_buffers_0 = pool.get_frame_buffers(slot, symbol, ofh_type);
      span<frame_buffer> frame_buffers_1 = pool.get_frame_buffers(slot, symbol, ofh_type);
      // Initialize with random data.
      init_eth_buffers_with_rnd_data(frame_buffers_0, test_data_0, num_frames);
      init_eth_buffers_with_rnd_data(frame_buffers_1, test_data_1, num_frames);

      // Push prepared data in a random manner.
      unsigned push_first = test_rgen::uniform_int<unsigned>(0, 1);
      if (push_first == 1) {
        pool.eth_frames_ready(slot, symbol, ofh_type, frame_buffers_1);
        pool.eth_frames_ready(slot, symbol, ofh_type, frame_buffers_0);
      } else {
        pool.eth_frames_ready(slot, symbol, ofh_type, frame_buffers_0);
        pool.eth_frames_ready(slot, symbol, ofh_type, frame_buffers_1);
      }

      /// Read frame buffers from the pool and verify data.
      span<const span<const frame_buffer>> prepared_frame_buffers = pool.read_frame_buffers(slot, symbol, ofh_type);
      ASSERT_TRUE(prepared_frame_buffers.size() == 2) << "Two batches of packets expected";

      for (unsigned i = 0; i < 2; ++i) {
        span<const frame_buffer> rd_frame_buffers = prepared_frame_buffers[i];
        EXPECT_TRUE(!rd_frame_buffers.empty()) << "Expected non empty span of ethernet frame buffers";
        ASSERT_TRUE(rd_frame_buffers.size() == max_num_packets)
            << "Reading from the pool returned incorrect number of buffers";

        std::array<std::vector<uint8_t>, MAX_PACKETS_PER_SYM>& test_data =
            (i == 0) ? ((push_first == 1) ? test_data_1 : test_data_0)
                     : ((push_first == 1) ? test_data_0 : test_data_1);

        for (unsigned j = 0; j != rd_frame_buffers.size(); ++j) {
          // Size and data should match the randomly generated ones.
          EXPECT_EQ(test_data[j].size(), rd_frame_buffers[j].size())
              << "Size of read packet doesn't match the size of written vector";
          if (!test_data[j].empty()) {
            ASSERT_TRUE(
                std::equal(rd_frame_buffers[j].data().begin(), rd_frame_buffers[j].data().end(), test_data[j].begin()))
                << "Data mismatch";
          }
        }
      }
      pool.eth_frames_sent(slot, symbol, ofh_type);
    }
    ++slot;
  }
}

INSTANTIATE_TEST_SUITE_P(EthFramePoolTestSuite,
                         EthFramePoolFixture,
                         ::testing::Combine(::testing::Values(subcarrier_spacing::kHz15, subcarrier_spacing::kHz30),
                                            ::testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED),
                                            ::testing::Values(ofh::message_type::control_plane,
                                                              ofh::message_type::user_plane)));

} // namespace
