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
std::ostream& operator<<(std::ostream& os, cyclic_prefix cp)
{
  return os << fmt::format("cp={}", cp.to_string());
}

namespace ofh {
std::ostream& operator<<(std::ostream& os, const message_type& t)
{
  return os << fmt::format("ofh={}", t == message_type::control_plane ? "control" : "user");
}
std::ostream& operator<<(std::ostream& os, const data_direction& dir)
{
  return os << fmt::format("direction={}", dir == data_direction::downlink ? "downlink" : "uplink");
}

} // namespace ofh

} // namespace srsran

static constexpr size_t TEST_NUM_SLOTS       = 50;
static constexpr float  TEST_MAX_SYMBOL_SIZE = 7648;
static unsigned         test_mtu_size        = 9000;

/// Write random data to the Ethernet frame buffers.
static void init_eth_buffers_with_rnd_data(span<scoped_frame_buffer>  reserved_buffers,
                                           span<std::vector<uint8_t>> test_data,
                                           unsigned                   num_frames)
{
  for (unsigned frame = 0; frame < num_frames; ++frame) {
    auto pkt_size        = test_rgen::uniform_int<unsigned>(MIN_ETH_FRAME_LENGTH, test_mtu_size);
    test_data[frame]     = test_rgen::random_vector<uint8_t>(pkt_size);
    span<uint8_t> buffer = reserved_buffers[frame]->get_buffer().first(pkt_size);
    std::copy(test_data[frame].begin(), test_data[frame].end(), buffer.begin());
    reserved_buffers[frame]->set_size(pkt_size);
  }
}

namespace {

enum mtu { MTU_9000 = 9000, MTU_5000 = 5000, MTU_1500 = 1500 };

using test_params_t = std::tuple<mtu, subcarrier_spacing, cyclic_prefix, ofh::message_type, ofh::data_direction>;

class EthFramePoolFixture : public ::testing::TestWithParam<test_params_t>
{
protected:
  unsigned              mtu_size      = std::get<0>(GetParam());
  subcarrier_spacing    scs           = std::get<1>(GetParam());
  cyclic_prefix         cp            = std::get<2>(GetParam());
  ofh::message_type     ofh_type      = std::get<3>(GetParam());
  ofh::data_direction   ofh_direction = std::get<4>(GetParam());
  unsigned              nof_symbols   = get_nsymb_per_slot(cp);
  unsigned              nof_frames    = std::ceil(TEST_MAX_SYMBOL_SIZE / mtu_size);
  srslog::basic_logger& logger        = srslog::fetch_basic_logger("TEST");
  eth_frame_pool        pool          = {logger, units::bytes(mtu_size), nof_frames, ofh_type, ofh_direction};

  void SetUp() override { test_mtu_size = mtu_size; }
};

/// Verify reading from empty pool returns an empty span.
TEST_P(EthFramePoolFixture, read_empty_pool_should_return_empty_span)
{
  slot_point slot(to_numerology_value(scs), 0);
  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      static_vector<scoped_frame_buffer, MAX_TX_BURST_SIZE> frame_burst;
      ofh::slot_symbol_point                                symbol_point(slot, symbol, nof_symbols);

      pool.enqueue_pending_into_burst(symbol_point, frame_burst);

      ASSERT_TRUE(frame_burst.empty()) << "Reading empty pool returned non-empty span of buffers";
    }
    ++slot;
  }
}

/// Verify that written small packet size is rounded to 64 Bytes.
TEST_P(EthFramePoolFixture, writing_small_pkt_is_rounded_to_min_eth_size)
{
  // Initialize slot to a random value.
  slot_point slot(to_numerology_value(scs), 0);
  unsigned   slots_per_system_frame = slot.nof_slots_per_hyper_system_frame();
  slot += test_rgen::uniform_int<unsigned>(0, slots_per_system_frame);

  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      ofh::slot_symbol_point symbol_point(slot, symbol, nof_symbols);

      // Generate small packet filled with random data.
      std::array<uint8_t, MIN_ETH_FRAME_LENGTH> zeros{0};
      unsigned                                  pkt_size = test_rgen::uniform_int<unsigned>(1, MIN_ETH_FRAME_LENGTH);
      std::vector<uint8_t>                      pkt_data = test_rgen::random_vector<uint8_t>(pkt_size);
      std::copy(pkt_data.begin(), pkt_data.end(), zeros.begin());

      // Reserve a frame buffer.
      {
        auto wr_buffer = pool.reserve(symbol_point);
        ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";

        // Write random data.
        span<uint8_t> wr_data_buffer = wr_buffer->get_buffer().first(pkt_size);
        std::copy(pkt_data.begin(), pkt_data.end(), wr_data_buffer.begin());
        wr_buffer->set_size(pkt_size);
      }
      // Read pending frame buffer.
      {
        static_vector<scoped_frame_buffer, MAX_TX_BURST_SIZE> frame_burst;
        pool.enqueue_pending_into_burst(symbol_point, frame_burst);
        ASSERT_TRUE(frame_burst.size() == 1) << "Expected to read one buffer from the pool";
        ASSERT_EQ(frame_burst[0]->size(), MIN_ETH_FRAME_LENGTH) << "Expected size is {}" << MIN_ETH_FRAME_LENGTH;

        ASSERT_TRUE(std::equal(frame_burst[0]->data().begin(), frame_burst[0]->data().end(), zeros.begin()))
            << "Data mismatch";
      }
    }
    ++slot;
  }
}

/// Verify reading the data right after writing it.
TEST_P(EthFramePoolFixture, read_after_write_should_return_correct_data)
{
  // Initialize slot to a random value.
  slot_point slot(to_numerology_value(scs), 0);
  unsigned   slots_per_system_frame = slot.nof_slots_per_hyper_system_frame();
  slot += test_rgen::uniform_int<unsigned>(0, slots_per_system_frame);

  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      ofh::slot_symbol_point symbol_point(slot, symbol, nof_symbols);

      // Number of Ethernet frames written in the current slot symbol.
      auto                              num_frames = test_rgen::uniform_int<unsigned>(1, nof_frames);
      std::vector<std::vector<uint8_t>> test_data(num_frames);

      {
        std::vector<scoped_frame_buffer> reserved_buffers;
        // Reserve buffers.
        while (reserved_buffers.size() != num_frames) {
          auto wr_buffer = pool.reserve(symbol_point);
          ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";
          reserved_buffers.emplace_back(std::move(wr_buffer));
        }

        // Write random data to the buffers and release them.
        init_eth_buffers_with_rnd_data(reserved_buffers, test_data, num_frames);
      }

      {
        // Read frame buffers from the pool.
        static_vector<scoped_frame_buffer, MAX_TX_BURST_SIZE> frame_burst;
        pool.enqueue_pending_into_burst(symbol_point, frame_burst);

        ASSERT_TRUE(frame_burst.size() == num_frames) << "Reading from the pool returned incorrect number of buffers";

        for (unsigned i = 0, e = frame_burst.size(); i != e; ++i) {
          // Size and data should match the randomly generated ones.
          EXPECT_EQ(test_data[i].size(), frame_burst[i]->size())
              << "Size of read packet doesn't match the size of written vector";
          if (!test_data[i].empty()) {
            ASSERT_TRUE(std::equal(frame_burst[i]->data().begin(), frame_burst[i]->data().end(), test_data[i].begin()))
                << "Data mismatch";
          }
        }
      }
    }
    ++slot;
  }
}

//// Verify writing sequentially two batches of packets and then reading all of them.
TEST_P(EthFramePoolFixture, read_multiple_written_packets_per_symbol_should_return_correct_data)
{
  // Initialize slot to a random value.
  slot_point slot(to_numerology_value(scs), 0);
  unsigned   slots_per_system_frame = slot.nof_slots_per_hyper_system_frame();
  slot += test_rgen::uniform_int<unsigned>(0, slots_per_system_frame);

  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      ofh::slot_symbol_point symbol_point(slot, symbol, nof_symbols);

      // Random test data for random number of eAxC.
      unsigned                          num_antennas = test_rgen::uniform_int<unsigned>(1, ofh::MAX_NOF_SUPPORTED_EAXC);
      std::vector<std::vector<uint8_t>> test_data;
      std::vector<unsigned>             eax_num_of_packets;

      for (unsigned eaxc = 0; eaxc != num_antennas; ++eaxc) {
        // Randomly choose number of Ethernet frames written in the current slot symbol for the current eAxC.
        unsigned num_frames_needed = test_rgen::uniform_int<unsigned>(1, nof_frames);

        unsigned nof_reserved = 0;
        while (nof_reserved != num_frames_needed) {
          auto wr_buffer = pool.reserve(symbol_point);
          ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";

          auto& test_data_entry = test_data.emplace_back();
          init_eth_buffers_with_rnd_data({&wr_buffer, 1}, {&test_data_entry, 1}, 1);
          ++nof_reserved;
        }
        eax_num_of_packets.push_back(num_frames_needed);
      }
      unsigned total_reserved = std::accumulate(eax_num_of_packets.begin(), eax_num_of_packets.end(), 0);

      {
        // Read frame buffers from the pool.
        static_vector<scoped_frame_buffer, MAX_TX_BURST_SIZE> frame_burst;
        pool.enqueue_pending_into_burst(symbol_point, frame_burst);

        EXPECT_TRUE(!frame_burst.empty()) << "Expected non empty span of ethernet frame buffers";
        EXPECT_TRUE(frame_burst.size() == total_reserved)
            << "Number of frame buffers read from the pool doesn't match number of reserved ones";

        for (unsigned i = 0, e = frame_burst.size(); i != e; ++i) {
          // Size and data should match the randomly generated ones.
          EXPECT_EQ(test_data[i].size(), frame_burst[i]->size())
              << "Size of read packet doesn't match the size of written vector, slot = " << slot_count
              << " symbol=" << symbol;
          if (!test_data[i].empty()) {
            ASSERT_TRUE(std::equal(frame_burst[i]->data().begin(), frame_burst[i]->data().end(), test_data[i].begin()))
                << "Data mismatch";
          }
        }
      }
    }
    ++slot;
  }
}

//// Verify reading an interval returns expected data.
TEST_P(EthFramePoolFixture, read_interval_should_return_correct_data)
{
  // Initialize slot to a random value.
  slot_point slot(to_numerology_value(scs), 0);
  unsigned   slots_per_system_frame = slot.nof_slots_per_hyper_system_frame();
  slot += test_rgen::uniform_int<unsigned>(0, slots_per_system_frame);

  auto num_antennas = test_rgen::uniform_int<unsigned>(1, ofh::MAX_NOF_SUPPORTED_EAXC);

  /// 1. Test C-Plane (prepare a packet for each antenna in symbol 0 and read back an interval including symbol 0).
  if (ofh_type == ofh::message_type::control_plane) {
    ofh::slot_symbol_point test_symbol_point(slot, 0, nof_symbols);
    ofh::slot_symbol_point start_symbol_point = test_symbol_point - 3U;
    ofh::slot_symbol_point end_symbol_point   = test_symbol_point + 1U;

    std::vector<std::vector<uint8_t>> test_data;

    for (unsigned eaxc = 0; eaxc != num_antennas; ++eaxc) {
      // Reserve one buffer.
      auto wr_buffer = pool.reserve(test_symbol_point);
      ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";

      auto& test_data_entry = test_data.emplace_back();
      init_eth_buffers_with_rnd_data({&wr_buffer, 1}, {&test_data_entry, 1}, 1);
    }

    // Create interval and read frame buffers from the pool.
    {
      ether::frame_pool_interval                            interval{start_symbol_point, end_symbol_point};
      static_vector<scoped_frame_buffer, MAX_TX_BURST_SIZE> frame_burst;
      pool.enqueue_pending_into_burst(interval, frame_burst);

      // Make sure we got 1 buffer for each antenna.
      ASSERT_TRUE(frame_burst.size() == num_antennas) << "Reading from the pool returned incorrect number of buffers";
      // Verify data.
      for (unsigned i = 0; i != num_antennas; ++i) {
        EXPECT_EQ(test_data[i].size(), frame_burst[i]->size())
            << "Size of read packet doesn't match the size of written vector";
        ASSERT_TRUE(std::equal(frame_burst[i]->data().begin(), frame_burst[i]->data().end(), test_data[i].begin()))
            << "Data mismatch";
      }
    }

    // Try to get maximum buffers for the same context and make sure the buffers are available.
    for (unsigned i = 0; i < 2 * ofh::MAX_NOF_SUPPORTED_EAXC; ++i) {
      auto wr_buffer = pool.reserve(test_symbol_point);
      ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";
      wr_buffer->set_size(1);
    }
    // Read them and discard.
    {
      ether::frame_pool_interval                            interval{start_symbol_point, end_symbol_point};
      static_vector<scoped_frame_buffer, MAX_TX_BURST_SIZE> frame_burst;
      pool.enqueue_pending_into_burst(interval, frame_burst);
      ASSERT_TRUE(frame_burst.size() == 2 * ofh::MAX_NOF_SUPPORTED_EAXC)
          << "Reading from the pool returned incorrect number of buffers";
    }

    // Reserve buffers and push it back without actually using. Verify the pool returns empty vector for the interval.
    {
      auto wr_buffer = pool.reserve(test_symbol_point);
      ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";
    }
    {
      ether::frame_pool_interval                            interval{start_symbol_point, end_symbol_point};
      static_vector<scoped_frame_buffer, MAX_TX_BURST_SIZE> frame_burst;
      pool.enqueue_pending_into_burst(interval, frame_burst);
      ASSERT_TRUE(frame_burst.empty()) << "No buffers were expected to be read from the pool";
    }
    return;
  }

  /// 2. Test U-Plane.
  unsigned               symbol_id = test_rgen::uniform_int<unsigned>(0, nof_symbols - 1);
  ofh::slot_symbol_point middle_symbol_point(slot, symbol_id, nof_symbols);
  ofh::slot_symbol_point start_symbol_point = middle_symbol_point - 3U;
  ofh::slot_symbol_point end_symbol_point   = middle_symbol_point + 1U;

  std::vector<std::vector<uint8_t>> test_data_start_symbol;
  std::vector<std::vector<uint8_t>> test_data_mid_symbol;
  unsigned                          total_nof_prepared_buffers = 0;

  for (unsigned eaxc = 0; eaxc != num_antennas; ++eaxc) {
    // First write buffers in the start symbol.
    auto num_frames_needed = test_rgen::uniform_int<unsigned>(1, nof_frames);
    for (unsigned i = 0; i < num_frames_needed; ++i) {
      // Reserve buffer.
      auto wr_buffer = pool.reserve(start_symbol_point);
      ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";

      // Write random data.
      auto& test_data_entry = test_data_start_symbol.emplace_back();
      init_eth_buffers_with_rnd_data({&wr_buffer, 1}, {&test_data_entry, 1}, 1);

      ++total_nof_prepared_buffers;
    }

    // Second write buffers in the middle symbol.
    num_frames_needed = test_rgen::uniform_int<unsigned>(1, nof_frames);
    for (unsigned i = 0; i < num_frames_needed; ++i) {
      // Reserve buffer.
      auto wr_buffer = pool.reserve(middle_symbol_point);
      ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";

      // Write random data.
      auto& test_data_entry = test_data_mid_symbol.emplace_back();
      init_eth_buffers_with_rnd_data({&wr_buffer, 1}, {&test_data_entry, 1}, 1);

      ++total_nof_prepared_buffers;
    }
  }

  // Create interval and read frame buffers from the pool.
  ether::frame_pool_interval interval{start_symbol_point, end_symbol_point};
  {
    // Read prepared buffers.
    static_vector<scoped_frame_buffer, MAX_TX_BURST_SIZE> frame_burst;
    pool.enqueue_pending_into_burst(interval, frame_burst);

    // Make sure we got 1 buffer for each antenna.
    ASSERT_TRUE(frame_burst.size() == total_nof_prepared_buffers)
        << "Reading from the pool returned incorrect number of buffers";
    unsigned buff_idx = 0;
    for (unsigned i = 0, e = test_data_start_symbol.size(); i != e; ++i, ++buff_idx) {
      // Verify data.
      EXPECT_EQ(test_data_start_symbol[i].size(), frame_burst[buff_idx]->size())
          << "Size of read packet doesn't match the size of written vector";
      ASSERT_TRUE(std::equal(frame_burst[buff_idx]->data().begin(),
                             frame_burst[buff_idx]->data().end(),
                             test_data_start_symbol[i].begin()))
          << "Data mismatch";
    }
    for (unsigned i = 0, e = test_data_mid_symbol.size(); i != e; ++i, ++buff_idx) {
      // Verify data.
      EXPECT_EQ(test_data_mid_symbol[i].size(), frame_burst[buff_idx]->size())
          << "Size of read packet doesn't match the size of written vector";
      ASSERT_TRUE(std::equal(
          frame_burst[buff_idx]->data().begin(), frame_burst[buff_idx]->data().end(), test_data_mid_symbol[i].begin()))
          << "Data mismatch";
    }
  }
}

TEST_P(EthFramePoolFixture, clearing_full_pool_should_allow_adding_more_data)
{
  logger.set_level(srslog::basic_levels::debug);

  size_t pool_size_slots = pool.pool_size_in_slots();

  slot_point slot(to_numerology_value(scs), 0);
  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      ofh::slot_symbol_point symbol_point(slot, symbol, nof_symbols);

      // Fot DL C-Plane we only write and clear buffers in slot 0.
      if (ofh_type == ofh::message_type::control_plane && ofh_direction == ofh::data_direction::downlink &&
          symbol != 0) {
        continue;
      }

      unsigned expected_reserved    = ofh::MAX_NOF_SUPPORTED_EAXC * 2 * nof_frames;
      unsigned nof_reserved_buffers = 0;
      // Reserve buffers.
      while (true) {
        auto wr_buffer = pool.reserve(symbol_point);
        if (!wr_buffer) {
          break;
        }
        wr_buffer->set_size(64);

        ++nof_reserved_buffers;
      }
      ASSERT_TRUE(nof_reserved_buffers >= expected_reserved)
          << "Number of reserved buffers " << nof_reserved_buffers << "is less then expected " << expected_reserved;

      // Increase slot by pool size and clear stale buffers in the pool.
      auto wrapped_slot = slot + pool_size_slots;
      pool.clear_slot(wrapped_slot, 0);

      // Verify the pool is empty in the given slot.
      {
        ofh::slot_symbol_point                                wrapped_symbol_point(wrapped_slot, symbol, nof_symbols);
        static_vector<scoped_frame_buffer, MAX_TX_BURST_SIZE> frame_burst;

        pool.enqueue_pending_into_burst(wrapped_symbol_point, frame_burst);
        ASSERT_TRUE(frame_burst.empty()) << "No buffers are expected to be read from the pool after clearing it";
      }

      // Try to reserve buffers again and make sure they are available.
      std::vector<scoped_frame_buffer> reserved_buffers;
      for (unsigned i = 0; i != expected_reserved; ++i) {
        // Reserve buffer.
        auto wr_buffer = pool.reserve(symbol_point);
        ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";
        reserved_buffers.push_back(std::move(wr_buffer));
      }
      ASSERT_TRUE(reserved_buffers.size() == expected_reserved) << "Couldn't reserve requested amount of buffers";
    }
    ++slot;
  }
}

TEST_P(EthFramePoolFixture, clearing_pool_should_clear_only_late_buffers)
{
  logger.set_level(srslog::basic_levels::debug);

  size_t pool_size_slots = pool.pool_size_in_slots();

  slot_point slot(to_numerology_value(scs), 0);
  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      ofh::slot_symbol_point symbol_point(slot, symbol, nof_symbols);

      // Fot DL C-Plane we only write and clear buffers in slot 0.
      if (ofh_type == ofh::message_type::control_plane && ofh_direction == ofh::data_direction::downlink &&
          symbol != 0) {
        continue;
      }

      // Add 'late' packets.
      unsigned num_to_reserve       = ofh::MAX_NOF_SUPPORTED_EAXC * nof_frames;
      unsigned nof_reserved_buffers = 0;
      // Reserve buffers.
      while (nof_reserved_buffers < num_to_reserve) {
        auto wr_buffer = pool.reserve(symbol_point);
        ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";

        wr_buffer->set_size(64);
        ++nof_reserved_buffers;
      }

      // Increase slot by pool size.
      auto                   wrapped_slot = slot + pool_size_slots;
      ofh::slot_symbol_point wrapped_symbol_point(wrapped_slot, symbol, nof_symbols);

      // Add on-time packets.
      nof_reserved_buffers = 0;
      while (nof_reserved_buffers < num_to_reserve) {
        auto wr_buffer = pool.reserve(wrapped_symbol_point);
        ASSERT_TRUE(wr_buffer) << "Non-empty buffer is expected";

        wr_buffer->set_size(64);

        ++nof_reserved_buffers;
      }
      // This should clear 'late' packets.
      pool.clear_slot(wrapped_slot, 0);

      // Verify the pool contains num_to_reserve packets afer clearing.
      {
        static_vector<scoped_frame_buffer, MAX_TX_BURST_SIZE> frame_burst;

        pool.enqueue_pending_into_burst(wrapped_symbol_point, frame_burst);
        ASSERT_TRUE(frame_burst.size() == num_to_reserve)
            << "Incorrect number of buffers retrieved from the pool after clearing late packets";
        for (unsigned i = 0; i != num_to_reserve; ++i) {
          ASSERT_TRUE(frame_burst[i]->get_slot_symbol() == wrapped_symbol_point)
              << "Retrieved frame buffer has incorrect timestamp";
        }
      }
    }
    ++slot;
  }
}

INSTANTIATE_TEST_SUITE_P(
    EthFramePoolTestSuite,
    EthFramePoolFixture,
    ::testing::Combine(::testing::Values(mtu::MTU_9000, mtu::MTU_5000, mtu::MTU_1500),
                       ::testing::Values(subcarrier_spacing::kHz15, subcarrier_spacing::kHz30),
                       ::testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED),
                       ::testing::Values(ofh::message_type::control_plane, ofh::message_type::user_plane),
                       ::testing::Values(ofh::data_direction::downlink, ofh::data_direction::uplink)));

} // namespace
