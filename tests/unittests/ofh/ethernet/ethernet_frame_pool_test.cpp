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

static constexpr size_t TEST_NUM_SLOTS       = 50;
static constexpr float  TEST_MAX_SYMBOL_SIZE = 7648;
static unsigned         test_mtu_size        = 9000;

/// Write random data to the Ethernet frame buffers.
static void init_eth_buffers_with_rnd_data(span<frame_buffer>         frame_buffers,
                                           span<std::vector<uint8_t>> test_data,
                                           unsigned                   num_frames)
{
  for (unsigned frame = 0; frame < num_frames; ++frame) {
    unsigned pkt_size    = test_rgen::uniform_int<unsigned>(MIN_ETH_FRAME_LENGTH, test_mtu_size);
    test_data[frame]     = test_rgen::random_vector<uint8_t>(pkt_size);
    span<uint8_t> buffer = frame_buffers[frame].data().first(pkt_size);
    std::copy(test_data[frame].begin(), test_data[frame].end(), buffer.begin());
    frame_buffers[frame].set_size(pkt_size);
  }
}

namespace {

enum mtu { MTU_9000 = 9000, MTU_5000 = 5000, MTU_1500 = 1500 };

using test_params_t = std::tuple<mtu, subcarrier_spacing, cyclic_prefix, ofh::message_type>;

class EthFramePoolFixture : public ::testing::TestWithParam<test_params_t>
{
protected:
  unsigned           mtu_size    = std::get<0>(GetParam());
  subcarrier_spacing scs         = std::get<1>(GetParam());
  cyclic_prefix      cp          = std::get<2>(GetParam());
  ofh::message_type  ofh_type    = std::get<3>(GetParam());
  unsigned           nof_symbols = get_nsymb_per_slot(cp);
  unsigned           nof_frames  = std::ceil(TEST_MAX_SYMBOL_SIZE / mtu_size);
  eth_frame_pool     pool        = {units::bytes(mtu_size), nof_frames};

  void SetUp() override { test_mtu_size = mtu_size; }
};

// Verify reading from empty pool returns an empty span.
TEST_P(EthFramePoolFixture, read_empty_pool_should_return_empty_span)
{
  slot_point slot(to_numerology_value(scs), 0);
  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      ofh::slot_symbol_point    symbol_point(slot, symbol, nof_symbols);
      ether::frame_pool_context ctx{{ofh_type, ofh::data_direction::downlink}, symbol_point};
      auto                      buffers = pool.read_frame_buffers(ctx);
      ASSERT_TRUE(buffers.empty()) << "Reading empty pool returned non-empty span of buffers";
      if (ofh_type == ofh::message_type::control_plane) {
        ctx.type.direction = ofh::data_direction::uplink;
        buffers            = pool.read_frame_buffers(ctx);
        ASSERT_TRUE(buffers.empty()) << "Reading empty pool returned non-empty span of buffers";
      }
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
      ofh::slot_symbol_point symbol_point(slot, symbol, nof_symbols);

      // Generate small packet filled with random data.
      std::array<uint8_t, MIN_ETH_FRAME_LENGTH> zeros{0};
      unsigned                                  pkt_size = test_rgen::uniform_int<unsigned>(1, MIN_ETH_FRAME_LENGTH);
      std::vector<uint8_t>                      pkt_data = test_rgen::random_vector<uint8_t>(pkt_size);
      std::copy(pkt_data.begin(), pkt_data.end(), zeros.begin());

      ofh::data_direction direction = ofh::data_direction::downlink;
      if (ofh_type == ofh::message_type::control_plane) {
        // For Control-Plane messages randomly choose the direction.
        direction = static_cast<ofh::data_direction>(test_rgen::uniform_int<unsigned>(
            static_cast<unsigned>(ofh::data_direction::uplink), static_cast<unsigned>(ofh::data_direction::downlink)));
      }

      // Get span of frame buffers.
      ether::frame_pool_context context{{ofh_type, direction}, symbol_point};
      span<frame_buffer>        wr_buffers = pool.get_frame_buffers(context);
      ASSERT_TRUE(!wr_buffers.empty()) << "Non-empty span of buffers expected";

      // Write random data to the first buffer.
      span<uint8_t> wr_data_buffer = wr_buffers[0].data().first(pkt_size);
      std::copy(pkt_data.begin(), pkt_data.end(), wr_data_buffer.begin());
      wr_buffers[0].set_size(pkt_size);
      pool.push_frame_buffers(context, wr_buffers);

      // Read frame buffers and verify length and content of the first one.
      auto rd_buffers = pool.read_frame_buffers(context);
      ASSERT_TRUE(!rd_buffers.empty()) << "Non-empty span of buffers expected";

      ASSERT_EQ(rd_buffers[0]->size(), MIN_ETH_FRAME_LENGTH) << "Expected size is {}" << MIN_ETH_FRAME_LENGTH;
      ASSERT_TRUE(std::equal(rd_buffers[0]->data().begin(), rd_buffers[0]->data().end(), zeros.begin()))
          << "Data mismatch";
      pool.clear_sent_frame_buffers(context);
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
      ofh::slot_symbol_point symbol_point(slot, symbol, nof_symbols);

      ofh::data_direction direction = ofh::data_direction::downlink;
      if (ofh_type == ofh::message_type::control_plane) {
        // For Control-Plane messages randomly choose the direction.
        direction = static_cast<ofh::data_direction>(test_rgen::uniform_int<unsigned>(
            static_cast<unsigned>(ofh::data_direction::uplink), static_cast<unsigned>(ofh::data_direction::downlink)));
      }

      // Get span of frame buffers.
      ether::frame_pool_context context{{ofh_type, direction}, symbol_point};
      span<frame_buffer>        frame_buffers = pool.get_frame_buffers(context);
      ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";
      std::vector<std::vector<uint8_t>> test_data(frame_buffers.size());

      // Number of Ethernet frames written in the current slot symbol.
      unsigned num_frames = test_rgen::uniform_int<unsigned>(1, frame_buffers.size());

      // Write random data to the buffers.
      init_eth_buffers_with_rnd_data(frame_buffers, test_data, num_frames);
      pool.push_frame_buffers(context, frame_buffers);

      // Read frame buffers from the pool.
      auto rd_frame_buffers = pool.read_frame_buffers(context);

      ASSERT_TRUE(rd_frame_buffers.size() == num_frames)
          << "Reading from the pool returned incorrect number of buffers";

      for (unsigned i = 0; i != rd_frame_buffers.size(); ++i) {
        // Size and data should match the randomly generated ones.
        EXPECT_EQ(test_data[i].size(), rd_frame_buffers[i]->size())
            << "Size of read packet doesn't match the size of written vector";
        if (!test_data[i].empty()) {
          ASSERT_TRUE(
              std::equal(rd_frame_buffers[i]->data().begin(), rd_frame_buffers[i]->data().end(), test_data[i].begin()))
              << "Data mismatch";
        }
      }
      pool.clear_sent_frame_buffers(context);
    }
    ++slot;
  }
}

// Verify writing sequentially two batches of packets and then reading all of them.
TEST_P(EthFramePoolFixture, read_multiple_written_packets_per_symbol_should_return_correct_data)
{
  // Initialize slot to a random value.
  slot_point slot(to_numerology_value(scs), 0);
  unsigned   slots_per_system_frame = slot.nof_slots_per_system_frame();
  slot += test_rgen::uniform_int<unsigned>(0, slots_per_system_frame);

  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      ofh::slot_symbol_point symbol_point(slot, symbol, nof_symbols);

      // Random test data for random number of eAxC.
      unsigned                          num_antennas = test_rgen::uniform_int<unsigned>(1, ofh::MAX_NOF_SUPPORTED_EAXC);
      std::vector<std::vector<uint8_t>> test_data;
      std::vector<unsigned>             eax_num_of_packets;

      ofh::data_direction direction = ofh::data_direction::downlink;
      if (ofh_type == ofh::message_type::control_plane) {
        // For Control-Plane messages randomly choose the direction.
        direction = static_cast<ofh::data_direction>(test_rgen::uniform_int<unsigned>(
            static_cast<unsigned>(ofh::data_direction::uplink), static_cast<unsigned>(ofh::data_direction::downlink)));
      }
      ether::frame_pool_context context{{ofh_type, direction}, symbol_point};

      for (unsigned eaxc = 0; eaxc != num_antennas; ++eaxc) {
        // Get a batch of frame buffers for the current slot symbol.
        span<frame_buffer> frame_buffers = pool.get_frame_buffers(context);
        ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";

        // Number of Ethernet frames to fill with data.
        unsigned num_frames = test_rgen::uniform_int<unsigned>(1, frame_buffers.size());

        // Write random data.
        for (unsigned i = 0; i < num_frames; ++i) {
          test_data.emplace_back();
        }
        span<std::vector<uint8_t>> test_data_span = {&test_data[test_data.size() - num_frames], num_frames};
        init_eth_buffers_with_rnd_data(frame_buffers, test_data_span, num_frames);
        pool.push_frame_buffers(context, frame_buffers);
        eax_num_of_packets.push_back(num_frames);
      }

      // Read frame buffers from the pool and verify data.
      auto rd_frame_buffers = pool.read_frame_buffers(context);
      EXPECT_TRUE(!rd_frame_buffers.empty()) << "Expected non empty span of ethernet frame buffers";

      unsigned start_idx = 0;
      for (unsigned i = 0; i < num_antennas; ++i) {
        span<std::vector<uint8_t>> test_data_span     = {&test_data[start_idx], eax_num_of_packets[i]};
        span<const frame_buffer*>  eaxc_frame_buffers = {&rd_frame_buffers[start_idx], eax_num_of_packets[i]};
        start_idx += eax_num_of_packets[i];

        for (unsigned j = 0, e = test_data_span.size(); j != e; ++j) {
          // Size and data should match the randomly generated ones.
          EXPECT_EQ(test_data_span[j].size(), eaxc_frame_buffers[j]->size())
              << "Size of read packet doesn't match the size of written vector, slot=" << slot_count
              << " symbol=" << symbol;
          if (!test_data_span[j].empty()) {
            ASSERT_TRUE(std::equal(
                eaxc_frame_buffers[j]->data().begin(), eaxc_frame_buffers[j]->data().end(), test_data_span[j].begin()))
                << "Data mismatch";
          }
        }
      }
      pool.clear_sent_frame_buffers(context);
    }
    ++slot;
  }
}

// Verify reading the data right after writing it.
TEST_P(EthFramePoolFixture, read_interval_should_return_correct_data)
{
  // Initialize slot to a random value.
  slot_point slot(to_numerology_value(scs), 0);
  unsigned   slots_per_system_frame = slot.nof_slots_per_system_frame();
  slot += test_rgen::uniform_int<unsigned>(0, slots_per_system_frame);

  unsigned num_antennas = test_rgen::uniform_int<unsigned>(1, ofh::MAX_NOF_SUPPORTED_EAXC);

  /// 1. Test C-Plane (prepare a packet for each antenna in symbol 0 and read back an interval including symbol 0).
  if (ofh_type == ofh::message_type::control_plane) {
    ofh::slot_symbol_point test_symbol_point(slot, 0, nof_symbols);
    ofh::slot_symbol_point start_symbol_point = test_symbol_point - 3U;
    ofh::slot_symbol_point end_symbol_point   = test_symbol_point + 1U;

    // For C-Plane messages randomly choose the direction.
    ofh::data_direction direction = static_cast<ofh::data_direction>(test_rgen::uniform_int<unsigned>(
        static_cast<unsigned>(ofh::data_direction::uplink), static_cast<unsigned>(ofh::data_direction::downlink)));

    ether::ofh_pool_message_type      pool_msg_type{ofh_type, direction};
    ether::frame_pool_context         context{pool_msg_type, test_symbol_point};
    std::vector<std::vector<uint8_t>> test_data;

    for (unsigned eaxc = 0; eaxc != num_antennas; ++eaxc) {
      // Get span of frame buffers.
      auto frame_buffers = pool.get_frame_buffers(context);
      ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";

      // Fill 1 buffer with random data.
      test_data.emplace_back();
      init_eth_buffers_with_rnd_data(frame_buffers, {&test_data[eaxc], 1}, 1);
      pool.push_frame_buffers(context, frame_buffers);
    }

    // Create interval and read frame buffers from the pool.
    ether::frame_pool_interval interval{pool_msg_type, start_symbol_point, end_symbol_point};
    auto                       rd_frame_buffers = pool.read_frame_buffers(interval);
    // Make sure we got 1 buffer for each antenna.
    ASSERT_TRUE(rd_frame_buffers.size() == num_antennas)
        << "Reading from the pool returned incorrect number of buffers";
    // Verify data.
    for (unsigned i = 0; i != num_antennas; ++i) {
      EXPECT_EQ(test_data[i].size(), rd_frame_buffers[i]->size())
          << "Size of read packet doesn't match the size of written vector";
      ASSERT_TRUE(
          std::equal(rd_frame_buffers[i]->data().begin(), rd_frame_buffers[i]->data().end(), test_data[i].begin()))
          << "Data mismatch";
    }
    pool.clear_sent_frame_buffers(interval);

    // Try to get buffers for the same context and make sure the buffers are available.
    for (unsigned i = 0; i < 2 * ofh::MAX_NOF_SUPPORTED_EAXC; ++i) {
      auto frame_buffers = pool.get_frame_buffers(context);
      ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";
      pool.push_frame_buffers(context, frame_buffers);
    }
    // Read buffers and push it back without actually using. Verify the pool returns empty vector for the interval.
    auto frame_buffers = pool.get_frame_buffers(context);
    pool.push_frame_buffers(context, frame_buffers);
    rd_frame_buffers = pool.read_frame_buffers(interval);
    ASSERT_TRUE(rd_frame_buffers.empty()) << "Reading from the pool returned incorrect number of buffers";
    return;
  }
  /// 2. Test U-Plane.
  ofh::data_direction    direction = ofh::data_direction::downlink;
  unsigned               symbol_id = test_rgen::uniform_int<unsigned>(0, nof_symbols - 1);
  ofh::slot_symbol_point middle_symbol_point(slot, symbol_id, nof_symbols);
  ofh::slot_symbol_point start_symbol_point = middle_symbol_point - 3U;
  ofh::slot_symbol_point end_symbol_point   = middle_symbol_point + 1U;

  // Fill data for the start symbol in the interval and one symbol in the middle.
  ether::ofh_pool_message_type pool_msg_type{ofh_type, direction};
  ether::frame_pool_context    context_mid{pool_msg_type, middle_symbol_point};
  ether::frame_pool_context    context_start{pool_msg_type, start_symbol_point};

  std::vector<std::vector<uint8_t>> test_data_start_symbol;
  std::vector<std::vector<uint8_t>> test_data_mid_symbol;
  unsigned                          total_nof_prepared_buffers = 0;

  for (unsigned eaxc = 0; eaxc != num_antennas; ++eaxc) {
    // Get a batch of frame buffers for the start symbol in the interval.
    span<frame_buffer> frame_buffers = pool.get_frame_buffers(context_start);
    ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";

    // Number of Ethernet frames to fill with data.
    unsigned num_frames = test_rgen::uniform_int<unsigned>(1, frame_buffers.size());
    for (unsigned i = 0; i < num_frames; ++i) {
      test_data_start_symbol.emplace_back();
    }
    // Write random data.
    span<std::vector<uint8_t>> test_data_span = {&test_data_start_symbol[test_data_start_symbol.size() - num_frames],
                                                 num_frames};
    init_eth_buffers_with_rnd_data(frame_buffers, test_data_span, num_frames);
    pool.push_frame_buffers(context_start, frame_buffers);
    total_nof_prepared_buffers += num_frames;

    // Get a batch of frame buffers for the middle symbol in the interval.
    frame_buffers = pool.get_frame_buffers(context_mid);
    ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";

    num_frames = test_rgen::uniform_int<unsigned>(1, frame_buffers.size());
    for (unsigned i = 0; i < num_frames; ++i) {
      test_data_mid_symbol.emplace_back();
    }
    test_data_span = {&test_data_mid_symbol[test_data_mid_symbol.size() - num_frames], num_frames};
    init_eth_buffers_with_rnd_data(frame_buffers, test_data_span, num_frames);
    pool.push_frame_buffers(context_mid, frame_buffers);
    total_nof_prepared_buffers += num_frames;
  }

  // Create interval and read frame buffers from the pool.
  ether::frame_pool_interval interval{pool_msg_type, start_symbol_point, end_symbol_point};

  auto rd_frame_buffers = pool.read_frame_buffers(interval);
  // Make sure we got all buffers prepared in the interval.
  ASSERT_TRUE(rd_frame_buffers.size() == total_nof_prepared_buffers)
      << "Reading from the pool returned incorrect number of buffers";

  unsigned rd_buffer_idx = 0;
  for (auto& test_vector : test_data_start_symbol) {
    // Size and data should match the randomly generated ones.
    EXPECT_EQ(test_vector.size(), rd_frame_buffers[rd_buffer_idx]->size())
        << "Size of read packet doesn't match the size of written vector";
    ASSERT_TRUE(std::equal(rd_frame_buffers[rd_buffer_idx]->data().begin(),
                           rd_frame_buffers[rd_buffer_idx]->data().end(),
                           test_vector.begin()))
        << "Data mismatch";
    ++rd_buffer_idx;
  }
  for (auto& test_vector : test_data_mid_symbol) {
    // Size and data should match the randomly generated ones.
    EXPECT_EQ(test_vector.size(), rd_frame_buffers[rd_buffer_idx]->size())
        << "Size of read packet doesn't match the size of written vector";
    ASSERT_TRUE(std::equal(rd_frame_buffers[rd_buffer_idx]->data().begin(),
                           rd_frame_buffers[rd_buffer_idx]->data().end(),
                           test_vector.begin()))
        << "Data mismatch";
    ++rd_buffer_idx;
  }
  pool.clear_sent_frame_buffers(interval);
}

// Pool should have enough space to store C-Plane and U-Plane packets for all antennas.
TEST_P(EthFramePoolFixture, pool_should_have_enough_space)
{
  slot_point slot(to_numerology_value(scs), 0);
  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      ofh::slot_symbol_point    symbol_point(slot, symbol, nof_symbols);
      ether::frame_pool_context ctx{{ofh_type, ofh::data_direction::downlink}, symbol_point};
      unsigned                  nof_requested_buffers = ofh::MAX_NOF_SUPPORTED_EAXC;
      // DL C-Plane and U-Plane
      for (unsigned i = 0; i != nof_requested_buffers; ++i) {
        span<frame_buffer> frame_buffers = pool.get_frame_buffers(ctx);
        ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";
        for (auto& buffer : frame_buffers) {
          buffer.set_size(64);
        }
        pool.push_frame_buffers(ctx, frame_buffers);
      }
      pool.read_frame_buffers(ctx);
      pool.clear_sent_frame_buffers(ctx);

      if (ofh_type == ofh::message_type::user_plane) {
        continue;
      }
      // UL C-Plane
      // We may need to write Type 1 and Type 3 C-Plane messages in the same slot and symbol.
      nof_requested_buffers *= 2;
      ctx = {{ofh_type, ofh::data_direction::uplink}, symbol_point};
      for (unsigned i = 0; i != nof_requested_buffers; ++i) {
        span<frame_buffer> frame_buffers = pool.get_frame_buffers(ctx);
        ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";
        for (auto& buffer : frame_buffers) {
          buffer.set_size(64);
        }
        pool.push_frame_buffers(ctx, frame_buffers);
      }
      pool.read_frame_buffers(ctx);
      pool.clear_sent_frame_buffers(ctx);
    }
    ++slot;
  }
}

TEST_P(EthFramePoolFixture, clearing_full_pool_should_allow_adding_more_data)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("OFH_TEST", false);
  logger.set_level(srslog::basic_levels::debug);

  size_t pool_size_slots = pool.pool_size_in_slots();

  slot_point slot(to_numerology_value(scs), 0);
  for (unsigned slot_count = 0; slot_count < TEST_NUM_SLOTS; ++slot_count) {
    for (unsigned symbol = 0; symbol < nof_symbols; ++symbol) {
      ofh::slot_symbol_point symbol_point(slot, symbol, nof_symbols);

      if (ofh_type == ofh::message_type::control_plane && symbol != 0) {
        continue;
      }

      // DL C-Plane and U-Plane
      bool                      pool_has_space = true;
      ether::frame_pool_context ctx{{ofh_type, ofh::data_direction::downlink}, symbol_point};
      unsigned                  nof_requested_buffers = ofh::MAX_NOF_SUPPORTED_EAXC;
      while (pool_has_space) {
        span<frame_buffer> frame_buffers = pool.get_frame_buffers(ctx);
        pool_has_space                   = !frame_buffers.empty();
        for (auto& buffer : frame_buffers) {
          buffer.set_size(64);
        }
        pool.push_frame_buffers(ctx, frame_buffers);
      }
      // Increase slot by pool size, clear stale buffers in the pool.
      auto wrapped_slot = slot + pool_size_slots;
      pool.clear_downlink_slot(wrapped_slot, logger);

      // Verify the pool is empty in the given slot.
      auto rd_buffers = pool.read_frame_buffers(ctx);
      ASSERT_TRUE(rd_buffers.empty()) << "No buffers are expected to be read from the pool after clearing it";

      // Try to get buffers again and make sure they are available.
      for (unsigned i = 0; i != nof_requested_buffers; ++i) {
        span<frame_buffer> frame_buffers = pool.get_frame_buffers(ctx);
        ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";
        pool.push_frame_buffers(ctx, frame_buffers);
      }
      pool.read_frame_buffers(ctx);
      pool.clear_sent_frame_buffers(ctx);

      if (ofh_type == ofh::message_type::user_plane) {
        continue;
      }

      // UL C-Plane
      // We may need to write Type 1 and Type 3 C-Plane messages in the same slot and symbol.
      nof_requested_buffers *= 2;
      ctx            = {{ofh_type, ofh::data_direction::uplink}, symbol_point};
      pool_has_space = true;
      while (pool_has_space) {
        span<frame_buffer> frame_buffers = pool.get_frame_buffers(ctx);
        pool_has_space                   = !frame_buffers.empty();
        for (auto& buffer : frame_buffers) {
          buffer.set_size(64);
        }
        pool.push_frame_buffers(ctx, frame_buffers);
      }
      // Clear full slot in the pool.
      pool.clear_uplink_slot(wrapped_slot, logger);

      // Verify the pool is empty in the given slot.
      rd_buffers = pool.read_frame_buffers(ctx);
      ASSERT_TRUE(rd_buffers.empty()) << "No buffers are expected to be read from the pool after clearing it";

      // Try to get buffers again and make sure they are available.
      for (unsigned i = 0; i != nof_requested_buffers; ++i) {
        span<frame_buffer> frame_buffers = pool.get_frame_buffers(ctx);
        ASSERT_TRUE(!frame_buffers.empty()) << "Non-empty span of buffers expected";
        pool.push_frame_buffers(ctx, frame_buffers);
      }
      pool.read_frame_buffers(ctx);
      pool.clear_sent_frame_buffers(ctx);
    }
    ++slot;
  }
}

INSTANTIATE_TEST_SUITE_P(EthFramePoolTestSuite,
                         EthFramePoolFixture,
                         ::testing::Combine(::testing::Values(mtu::MTU_9000, mtu::MTU_5000, mtu::MTU_1500),
                                            ::testing::Values(subcarrier_spacing::kHz15, subcarrier_spacing::kHz30),
                                            ::testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED),
                                            ::testing::Values(ofh::message_type::control_plane,
                                                              ofh::message_type::user_plane)));

} // namespace
