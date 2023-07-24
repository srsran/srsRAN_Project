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

#include "../../../../support/resource_grid_test_doubles.h"
#include "../../../modulation/ofdm_modulator_test_doubles.h"
#include "pdxch_processor_notifier_test_doubles.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_factories.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, subcarrier_spacing scs)
{
  fmt::print(os, "{}", to_string(scs));
  return os;
}

std::ostream& operator<<(std::ostream& os, sampling_rate srate)
{
  fmt::print(os, "{} MHz", srate.to_MHz());
  return os;
}

std::ostream& operator<<(std::ostream& os, cyclic_prefix cp)
{
  fmt::print(os, "{}", cp.to_string());
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, const pdxch_processor_baseband::symbol_context& context)
{
  fmt::print(os, "{} {} {}", context.slot, context.symbol, context.sector);
  return os;
}

std::ostream& operator<<(std::ostream& os, const ofdm_modulator_configuration& config)
{
  fmt::print(os,
             "Numerology={} BW={} DftSize={} CP={} Scale={} CenterFreq={}Hz",
             config.numerology,
             config.bw_rb,
             config.dft_size,
             config.cp.to_string(),
             config.scale,
             config.center_freq_hz);
  return os;
}

bool operator==(const pdxch_processor_baseband::symbol_context& left,
                const pdxch_processor_baseband::symbol_context& right)
{
  return (left.slot == right.slot) && (left.symbol == right.symbol) && (left.sector == right.sector);
}

bool operator==(const ofdm_modulator_configuration& left, const ofdm_modulator_configuration& right)
{
  return (left.numerology == right.numerology) && (left.bw_rb == right.bw_rb) && (left.dft_size == right.dft_size) &&
         (left.cp == right.cp) && (left.scale == right.scale) && (left.center_freq_hz == right.center_freq_hz);
}

bool operator==(span<const cf_t> left, span<const cf_t> right)
{
  return std::equal(left.begin(), left.end(), right.begin(), right.end());
}

bool operator==(const baseband_gateway_buffer_reader& left, const baseband_gateway_buffer_reader& right)
{
  if (left.get_nof_channels() != right.get_nof_channels()) {
    return false;
  }
  unsigned nof_channels = left.get_nof_channels();

  for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
    span<const cf_t> left_channel  = left.get_channel_buffer(i_channel);
    span<const cf_t> right_channel = right.get_channel_buffer(i_channel);
    if (!std::equal(left_channel.begin(), left_channel.end(), right_channel.begin(), right_channel.end())) {
      return false;
    }
  }

  return true;
}

} // namespace srsran

using LowerPhyDownlinkProcessorParams = std::tuple<unsigned, sampling_rate, subcarrier_spacing, cyclic_prefix>;

namespace {

class LowerPhyDownlinkProcessorFixture : public ::testing::TestWithParam<LowerPhyDownlinkProcessorParams>
{
protected:
  static constexpr unsigned request_queue_size = 3;

  static void SetUpTestSuite()
  {
    if (pdxch_proc_factory == nullptr) {
      ofdm_mod_factory_spy = std::make_shared<ofdm_modulator_factory_spy>();
      ASSERT_NE(ofdm_mod_factory_spy, nullptr);

      pdxch_proc_factory = create_pdxch_processor_factory_sw(request_queue_size, ofdm_mod_factory_spy);
      ASSERT_NE(pdxch_proc_factory, nullptr);
    }
  }

  void SetUp() override
  {
    ASSERT_NE(pdxch_proc_factory, nullptr);

    // Select parameters.
    unsigned           nof_tx_ports = std::get<0>(GetParam());
    sampling_rate      srate        = std::get<1>(GetParam());
    subcarrier_spacing scs          = std::get<2>(GetParam());
    cyclic_prefix      cp           = std::get<3>(GetParam());

    // Prepare configurations.
    config.cp             = cp;
    config.scs            = scs;
    config.srate          = srate;
    config.bandwidth_rb   = dist_bandwidth_prb(rgen);
    config.center_freq_Hz = dist_center_freq_Hz(rgen);
    config.nof_tx_ports   = nof_tx_ports;

    // Create processor.
    pdxch_proc = pdxch_proc_factory->create(config);
    ASSERT_NE(pdxch_proc, nullptr);

    // Select OFDM modulator processor spy.
    ofdm_mod_spy = ofdm_mod_factory_spy->get_modulators().back();
  }

  static constexpr unsigned                          nof_frames_test    = 3;
  static constexpr unsigned                          initial_slot_index = 0;
  static std::mt19937                                rgen;
  static std::uniform_int_distribution<unsigned>     dist_sector_id;
  static std::uniform_int_distribution<unsigned>     dist_bandwidth_prb;
  static std::uniform_real_distribution<double>      dist_center_freq_Hz;
  static std::uniform_real_distribution<float>       dist_sample;
  static std::shared_ptr<ofdm_modulator_factory_spy> ofdm_mod_factory_spy;
  static std::shared_ptr<pdxch_processor_factory>    pdxch_proc_factory;

  pdxch_processor_configuration    config;
  std::unique_ptr<pdxch_processor> pdxch_proc   = nullptr;
  ofdm_symbol_modulator_spy*       ofdm_mod_spy = nullptr;
};

std::mt19937                                LowerPhyDownlinkProcessorFixture::rgen(0);
std::uniform_int_distribution<unsigned>     LowerPhyDownlinkProcessorFixture::dist_sector_id(0, 16);
std::uniform_int_distribution<unsigned>     LowerPhyDownlinkProcessorFixture::dist_bandwidth_prb(1, MAX_RB);
std::uniform_real_distribution<double>      LowerPhyDownlinkProcessorFixture::dist_center_freq_Hz(1e8, 6e9);
std::uniform_real_distribution<float>       LowerPhyDownlinkProcessorFixture::dist_sample(-1, 1);
std::shared_ptr<ofdm_modulator_factory_spy> LowerPhyDownlinkProcessorFixture::ofdm_mod_factory_spy = nullptr;
std::shared_ptr<pdxch_processor_factory>    LowerPhyDownlinkProcessorFixture::pdxch_proc_factory   = nullptr;

} // namespace

TEST_P(LowerPhyDownlinkProcessorFixture, ModulatorConfiguration)
{
  sampling_rate      srate = std::get<1>(GetParam());
  subcarrier_spacing scs   = std::get<2>(GetParam());
  cyclic_prefix      cp    = std::get<3>(GetParam());

  pdxch_processor_configuration expected_config;
  expected_config.cp             = cp;
  expected_config.scs            = scs;
  expected_config.srate          = srate;
  expected_config.bandwidth_rb   = config.bandwidth_rb;
  expected_config.center_freq_Hz = config.center_freq_Hz;
  expected_config.nof_tx_ports   = config.nof_tx_ports;

  ofdm_modulator_configuration expected_mod_config;
  expected_mod_config.numerology     = to_numerology_value(scs);
  expected_mod_config.bw_rb          = config.bandwidth_rb;
  expected_mod_config.dft_size       = srate.get_dft_size(scs);
  expected_mod_config.cp             = cp;
  expected_mod_config.scale          = 1.0F;
  expected_mod_config.center_freq_hz = config.center_freq_Hz;

  ASSERT_EQ(ofdm_mod_spy->get_configuration(), expected_mod_config);
}

TEST_P(LowerPhyDownlinkProcessorFixture, FlowNoRequest)
{
  unsigned           nof_tx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<2>(GetParam());
  cyclic_prefix      cp           = std::get<3>(GetParam());

  unsigned base_symbol_size = srate.get_dft_size(scs);

  baseband_gateway_buffer_dynamic buffer(nof_tx_ports, 2 * base_symbol_size);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  for (unsigned i_frame = 0, i_slot_frame = initial_slot_index; i_frame != nof_frames_test; ++i_frame) {
    for (unsigned i_subframe = 0; i_subframe != NOF_SUBFRAMES_PER_FRAME; ++i_subframe) {
      for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot, ++i_slot_frame) {
        for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
          unsigned cp_size = cp.get_length(i_symbol_subframe, scs).to_samples(srate.to_Hz());
          // Setup buffer.
          buffer.resize(cp_size + base_symbol_size);

          // Fill buffer.
          for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
            span<cf_t> port_buffer = buffer[i_port];
            std::generate(
                port_buffer.begin(), port_buffer.end(), []() { return cf_t(dist_sample(rgen), dist_sample(rgen)); });
          }

          // Clear spies.
          pdxch_proc_notifier_spy.clear_notifications();
          ofdm_mod_spy->clear_modulate_entries();

          // Prepare expected PDxCH baseband entry context.
          pdxch_processor_baseband::symbol_context pdxch_context;
          pdxch_context.slot   = slot_point(to_numerology_value(scs), i_slot_frame);
          pdxch_context.sector = dist_sector_id(rgen);
          pdxch_context.symbol = i_symbol;

          // Process baseband.
          pdxch_proc->get_baseband().process_symbol(buffer.get_writer(), pdxch_context);

          // Assert OFDM modulator call.
          auto& ofdm_mod_entries = ofdm_mod_spy->get_modulate_entries();
          ASSERT_EQ(ofdm_mod_entries.size(), nof_tx_ports);
          for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
            auto& entry = ofdm_mod_entries[i_port];
            ASSERT_EQ(span<const cf_t>(entry.output), buffer[i_port]);
            ASSERT_TRUE(entry.grid->is_empty(i_port));
            ASSERT_EQ(entry.port_index, i_port);
            ASSERT_EQ(entry.symbol_index, i_symbol_subframe);
          }

          // Assert notification.
          ASSERT_EQ(pdxch_proc_notifier_spy.get_nof_notifications(), 0);
        }
      }
    }
  }
}

TEST_P(LowerPhyDownlinkProcessorFixture, FlowFloodRequest)
{
  unsigned           nof_tx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<2>(GetParam());
  cyclic_prefix      cp           = std::get<3>(GetParam());

  unsigned base_symbol_size = srate.get_dft_size(scs);

  baseband_gateway_buffer_dynamic buffer(nof_tx_ports, 2 * base_symbol_size);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  for (unsigned i_frame = 0, i_slot_frame = initial_slot_index; i_frame != nof_frames_test; ++i_frame) {
    for (unsigned i_subframe = 0; i_subframe != NOF_SUBFRAMES_PER_FRAME; ++i_subframe) {
      for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot, ++i_slot_frame) {
        resource_grid_context rg_context;
        rg_context.slot   = slot_point(to_numerology_value(scs), i_slot_frame);
        rg_context.sector = dist_sector_id(rgen);

        // Request resource grid modulation for the current slot.
        resource_grid_reader_spy rg_spy(0, 0, 0);
        pdxch_proc->get_request_handler().handle_request(rg_spy, rg_context);

        for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
          unsigned cp_size = cp.get_length(i_symbol_subframe, scs).to_samples(srate.to_Hz());
          // Setup buffer.
          buffer.resize(cp_size + base_symbol_size);

          // Clear spies.
          pdxch_proc_notifier_spy.clear_notifications();
          ofdm_mod_spy->clear_modulate_entries();

          // Prepare expected PDxCH baseband entry context.
          pdxch_processor_baseband::symbol_context pdxch_context;
          pdxch_context.slot   = slot_point(to_numerology_value(scs), i_slot_frame);
          pdxch_context.sector = dist_sector_id(rgen);
          pdxch_context.symbol = i_symbol;

          // Process baseband.
          pdxch_proc->get_baseband().process_symbol(buffer.get_writer(), pdxch_context);

          // Assert OFDM modulator call.
          const auto& ofdm_mod_entries = ofdm_mod_spy->get_modulate_entries();
          ASSERT_EQ(ofdm_mod_entries.size(), nof_tx_ports);
          for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
            const auto& ofdm_mod_entry = ofdm_mod_entries[i_port];
            ASSERT_EQ(span<const cf_t>(ofdm_mod_entry.output), buffer[i_port]);
            ASSERT_EQ(static_cast<const void*>(ofdm_mod_entry.grid), static_cast<const void*>(&rg_spy));
            ASSERT_EQ(ofdm_mod_entry.port_index, i_port);
            ASSERT_EQ(ofdm_mod_entry.symbol_index, i_symbol_subframe);
          }

          // Assert notification.
          ASSERT_EQ(pdxch_proc_notifier_spy.get_request_late().size(), 0);
          ASSERT_EQ(pdxch_proc_notifier_spy.get_request_overflow().size(), 0);
        }
      }
    }
  }
}

TEST_P(LowerPhyDownlinkProcessorFixture, LateRequest)
{
  unsigned           sector_id    = dist_sector_id(rgen);
  unsigned           nof_tx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<2>(GetParam());
  cyclic_prefix      cp           = std::get<3>(GetParam());

  unsigned base_symbol_size = srate.get_dft_size(scs);

  baseband_gateway_buffer_dynamic buffer(nof_tx_ports, 2 * base_symbol_size);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  unsigned initial_slot = 3;
  unsigned late_slot    = 2;
  unsigned next_slot    = 4;

  resource_grid_reader_spy initial_rg_spy(0, 0, 0);
  resource_grid_reader_spy late_rg_spy(0, 0, 0);
  resource_grid_reader_spy next_rg_spy(0, 0, 0);

  // Initial request.
  resource_grid_context initial_rg_context;
  initial_rg_context.slot   = slot_point(to_numerology_value(scs), initial_slot);
  initial_rg_context.sector = sector_id;
  pdxch_proc->get_request_handler().handle_request(initial_rg_spy, initial_rg_context);

  // Late request.
  resource_grid_context late_rg_context;
  late_rg_context.slot   = slot_point(to_numerology_value(scs), late_slot);
  late_rg_context.sector = sector_id;
  pdxch_proc->get_request_handler().handle_request(late_rg_spy, late_rg_context);

  // Next request.
  resource_grid_context next_rg_context;
  next_rg_context.slot   = slot_point(to_numerology_value(scs), next_slot);
  next_rg_context.sector = sector_id;
  pdxch_proc->get_request_handler().handle_request(next_rg_spy, next_rg_context);

  for (unsigned i_subframe = 0; i_subframe != NOF_SUBFRAMES_PER_FRAME; ++i_subframe) {
    for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot) {
      // Process one frame.
      for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
        unsigned cp_size = cp.get_length(i_symbol_subframe, scs).to_samples(srate.to_Hz());
        // Setup buffer.
        buffer.resize(cp_size + base_symbol_size);

        // Fill buffer.
        for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
          span<cf_t> port_buffer = buffer[i_port];
          std::generate(
              port_buffer.begin(), port_buffer.end(), []() { return cf_t(dist_sample(rgen), dist_sample(rgen)); });
        }

        // Clear spies.
        pdxch_proc_notifier_spy.clear_notifications();
        ofdm_mod_spy->clear_modulate_entries();

        // Prepare expected PDxCH baseband entry context.
        pdxch_processor_baseband::symbol_context pdxch_context;
        pdxch_context.slot   = slot_point(to_numerology_value(scs), i_slot);
        pdxch_context.sector = dist_sector_id(rgen);
        pdxch_context.symbol = i_symbol;

        // Process baseband.
        pdxch_proc->get_baseband().process_symbol(buffer.get_writer(), pdxch_context);

        // Assert OFDM modulator call only for initial and next slot.
        const auto&               ofdm_mod_entries = ofdm_mod_spy->get_modulate_entries();
        resource_grid_reader_spy* rg_spy           = (i_slot == initial_slot) ? &initial_rg_spy : &next_rg_spy;
        ASSERT_EQ(ofdm_mod_entries.size(), nof_tx_ports);
        for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
          const auto& ofdm_mod_entry = ofdm_mod_entries[i_port];
          ASSERT_EQ(span<const cf_t>(ofdm_mod_entry.output), buffer[i_port]);
          if ((i_slot == initial_slot) || (i_slot == next_slot)) {
            ASSERT_EQ(static_cast<const void*>(ofdm_mod_entry.grid), static_cast<const void*>(rg_spy));
          } else {
            ASSERT_TRUE(ofdm_mod_entry.grid->is_empty(i_port));
          }
          ASSERT_EQ(ofdm_mod_entry.port_index, i_port);
          ASSERT_EQ(ofdm_mod_entry.symbol_index, i_symbol_subframe);
        }

        // Assert notifications.
        ASSERT_EQ(pdxch_proc_notifier_spy.get_request_overflow().size(), 0);
        if (i_slot == next_slot) {
          const auto& lates = pdxch_proc_notifier_spy.get_request_late();
          ASSERT_EQ(lates.size(), 1);
          ASSERT_EQ(lates.front().slot, late_rg_context.slot);
          ASSERT_EQ(lates.front().sector, late_rg_context.sector);
        } else {
          ASSERT_EQ(pdxch_proc_notifier_spy.get_request_late().size(), 0);
        }
      }
    }
  }
}

TEST_P(LowerPhyDownlinkProcessorFixture, OverflowRequest)
{
  unsigned           nof_tx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<2>(GetParam());
  cyclic_prefix      cp           = std::get<3>(GetParam());

  unsigned base_symbol_size = srate.get_dft_size(scs);

  baseband_gateway_buffer_dynamic buffer(nof_tx_ports, 2 * base_symbol_size);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  resource_grid_reader_spy rg_spy(0, 0, 0);

  // Generate requests.
  for (unsigned i_request = 0; i_request != request_queue_size + 1; ++i_request) {
    resource_grid_context rg_context;
    rg_context.slot   = slot_point(to_numerology_value(scs), initial_slot_index + i_request);
    rg_context.sector = dist_sector_id(rgen);
    pdxch_proc->get_request_handler().handle_request(rg_spy, rg_context);
    ASSERT_EQ(pdxch_proc_notifier_spy.get_request_late().size(), 0);

    unsigned nof_expected_overflows = (i_request > request_queue_size) ? (i_request - request_queue_size) : 0;
    ASSERT_EQ(pdxch_proc_notifier_spy.get_request_late().size(), 0);
    ASSERT_EQ(pdxch_proc_notifier_spy.get_request_overflow().size(), nof_expected_overflows);
  }

  // Process requests for one frame.
  for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot) {
    // Process each symbol in one frame.
    for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
      unsigned cp_size = cp.get_length(i_symbol_subframe, scs).to_samples(srate.to_Hz());
      // Setup buffer.
      buffer.resize(cp_size + base_symbol_size);

      // Fill buffer.
      for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
        span<cf_t> port_buffer = buffer[i_port];
        std::generate(
            port_buffer.begin(), port_buffer.end(), []() { return cf_t(dist_sample(rgen), dist_sample(rgen)); });
      }

      // Clear spies.
      pdxch_proc_notifier_spy.clear_notifications();
      ofdm_mod_spy->clear_modulate_entries();

      // Prepare expected PDxCH baseband entry context.
      pdxch_processor_baseband::symbol_context pdxch_context;
      pdxch_context.slot   = slot_point(to_numerology_value(scs), i_slot);
      pdxch_context.sector = dist_sector_id(rgen);
      pdxch_context.symbol = i_symbol;

      // Process baseband.
      pdxch_proc->get_baseband().process_symbol(buffer.get_writer(), pdxch_context);

      // Assert OFDM modulator call only for the request enqueued.
      const auto& ofdm_mod_entries = ofdm_mod_spy->get_modulate_entries();
      if ((i_slot >= initial_slot_index) && (i_slot < initial_slot_index + request_queue_size)) {
        ASSERT_EQ(ofdm_mod_entries.size(), nof_tx_ports);
        for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
          const auto& ofdm_mod_entry = ofdm_mod_entries[i_port];
          ASSERT_EQ(span<const cf_t>(ofdm_mod_entry.output), buffer[i_port]);
          ASSERT_EQ(static_cast<const void*>(ofdm_mod_entry.grid), static_cast<const void*>(&rg_spy));
          ASSERT_EQ(ofdm_mod_entry.port_index, i_port);
          ASSERT_EQ(ofdm_mod_entry.symbol_index, i_symbol_subframe);
        }
      } else {
        ASSERT_EQ(ofdm_mod_entries.size(), 0);
      }

      // Assert notifications.
      ASSERT_EQ(pdxch_proc_notifier_spy.get_request_overflow().size(), 0);
    }
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(LowerPhyDownlinkProcessor,
                         LowerPhyDownlinkProcessorFixture,
                         ::testing::Combine(::testing::Values(1, 2, 4),
                                            ::testing::Values(sampling_rate::from_MHz(3.84),
                                                              sampling_rate::from_MHz(7.68)),
                                            ::testing::Values(subcarrier_spacing::kHz15, subcarrier_spacing::kHz30),
                                            ::testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED)));
