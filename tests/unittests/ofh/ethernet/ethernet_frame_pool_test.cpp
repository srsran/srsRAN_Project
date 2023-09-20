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
constexpr size_t TEST_NUM_SLOTS      = 50;
constexpr size_t MAX_PACKETS_PER_SYM = 2;

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
      ether::frame_pool_context ctx(slot, symbol, ofh_type, ofh::data_direction::downlink);
      auto                      buffers = pool.read_frame_buffers(ctx);
      ASSERT_TRUE(buffers.empty()) << "Reading empty pool returned non-empty span of buffers";
      if (ofh_type == ofh::message_type::control_plane) {
        ctx.direction = ofh::data_direction::uplink;
        buffers       = pool.read_frame_buffers(ctx);
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
      ether::frame_pool_context context(slot, symbol, ofh_type, direction);
      span<frame_buffer>        wr_buffers = pool.get_frame_buffers(context);
      // Write random data to the first buffer.
      span<uint8_t> wr_data_buffer = wr_buffers[0].data().first(pkt_size);
      std::copy(pkt_data.begin(), pkt_data.end(), wr_data_buffer.begin());
      wr_buffers[0].set_size(pkt_size);
      pool.eth_frames_ready(context, wr_buffers);

      // Read frame buffers and verify length and content of the first one.
      auto rd_buffers = pool.read_frame_buffers(context);
      ASSERT_TRUE(!rd_buffers.empty()) << "Non-empty span of buffers expected";

      ASSERT_EQ(rd_buffers[0]->size(), MIN_ETH_FRAME_LENGTH) << "Expected size is {}" << MIN_ETH_FRAME_LENGTH;
      ASSERT_TRUE(std::equal(rd_buffers[0]->data().begin(), rd_buffers[0]->data().end(), zeros.begin()))
          << "Data mismatch";
      pool.eth_frames_sent(context);
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
      ofh::data_direction direction = ofh::data_direction::downlink;
      if (ofh_type == ofh::message_type::control_plane) {
        // For Control-Plane messages randomly choose the direction.
        direction = static_cast<ofh::data_direction>(test_rgen::uniform_int<unsigned>(
            static_cast<unsigned>(ofh::data_direction::uplink), static_cast<unsigned>(ofh::data_direction::downlink)));
      }

      // Get span of frame buffers.
      ether::frame_pool_context                             context(slot, symbol, ofh_type, direction);
      span<frame_buffer>                                    frame_buffers = pool.get_frame_buffers(context);
      std::array<std::vector<uint8_t>, MAX_PACKETS_PER_SYM> test_data;

      // Number of Ethernet frames written in the current slot symbol.
      unsigned num_frames = test_rgen::uniform_int<unsigned>(1, frame_buffers.size());

      // Write random data to the buffers.
      init_eth_buffers_with_rnd_data(frame_buffers, test_data, num_frames);
      pool.eth_frames_ready(context, frame_buffers);

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
      pool.eth_frames_sent(context);
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
      ether::frame_pool_context context(slot, symbol, ofh_type, direction);

      for (unsigned eaxc = 0; eaxc != num_antennas; ++eaxc) {
        // Get a batch of frame buffers for the current slot symbol.
        span<frame_buffer> frame_buffers = pool.get_frame_buffers(context);

        // Number of Ethernet frames to fill with data.
        unsigned num_frames = test_rgen::uniform_int<unsigned>(1, frame_buffers.size());

        // Write random data.
        for (unsigned i = 0; i < num_frames; ++i) {
          test_data.emplace_back();
        }
        span<std::vector<uint8_t>> test_data_span = {&test_data[test_data.size() - num_frames], num_frames};
        init_eth_buffers_with_rnd_data(frame_buffers, test_data_span, num_frames);
        pool.eth_frames_ready(context, frame_buffers);
        eax_num_of_packets.push_back(num_frames);
      }

      // Read frame buffers from the pool and verify data.
      auto rd_frame_buffers = pool.read_frame_buffers(context);
      EXPECT_TRUE(!rd_frame_buffers.empty()) << "Expected non empty span of ethernet frame buffers";

      unsigned start_idx = 0;
      for (unsigned i = 0; i < num_antennas; ++i) {
        span<std::vector<uint8_t>> test_data_span     = {&test_data[start_idx], eax_num_of_packets[i]};
        auto                       eaxc_frame_buffers = rd_frame_buffers.subspan(start_idx, eax_num_of_packets[i]);
        start_idx += eax_num_of_packets[i];

        for (unsigned j = 0, e = test_data_span.size(); j != e; ++j) {
          // Size and data should match the randomly generated ones.
          EXPECT_EQ(test_data_span[j].size(), eaxc_frame_buffers[j]->size())
              << "Size of read packet doesn't match the size of written vector";
          if (!test_data_span[j].empty()) {
            ASSERT_TRUE(std::equal(
                eaxc_frame_buffers[j]->data().begin(), eaxc_frame_buffers[j]->data().end(), test_data_span[j].begin()))
                << "Data mismatch";
          }
        }
      }
      pool.eth_frames_sent(context);
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
