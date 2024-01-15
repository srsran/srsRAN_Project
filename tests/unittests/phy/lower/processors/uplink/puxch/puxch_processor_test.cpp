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

#include "../../../../support/resource_grid_test_doubles.h"
#include "../../../modulation/ofdm_demodulator_test_doubles.h"
#include "puxch_processor_notifier_test_doubles.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_request_handler.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_factories.h"
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

std::ostream& operator<<(std::ostream& os, const lower_phy_rx_symbol_context& context)
{
  fmt::print(os, "{} {} {}", context.slot, context.nof_symbols, context.sector);
  return os;
}

std::ostream& operator<<(std::ostream& os, const ofdm_demodulator_configuration& config)
{
  fmt::print(os,
             "Numerology={} BW={} DftSize={} CP={} WindowOffset={} Scale={} CenterFreq={}Hz",
             config.numerology,
             config.bw_rb,
             config.dft_size,
             config.cp.to_string(),
             config.nof_samples_window_offset,
             config.scale,
             config.center_freq_hz);
  return os;
}

bool operator==(const lower_phy_rx_symbol_context& left, const lower_phy_rx_symbol_context& right)
{
  return (left.slot == right.slot) && (left.nof_symbols == right.nof_symbols) && (left.sector == right.sector);
}

bool operator==(const ofdm_demodulator_configuration& left, const ofdm_demodulator_configuration& right)
{
  return (left.numerology == right.numerology) && (left.bw_rb == right.bw_rb) && (left.dft_size == right.dft_size) &&
         (left.cp == right.cp) && (left.nof_samples_window_offset == right.nof_samples_window_offset) &&
         (left.scale == right.scale) && (left.center_freq_hz == right.center_freq_hz);
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

using LowerPhyUplinkProcessorParams = std::tuple<unsigned, sampling_rate, subcarrier_spacing, cyclic_prefix>;

namespace {

class LowerPhyUplinkProcessorFixture : public ::testing::TestWithParam<LowerPhyUplinkProcessorParams>
{
protected:
  static constexpr unsigned request_queue_size = 16;

  static void SetUpTestSuite()
  {
    if (puxch_proc_factory == nullptr) {
      ofdm_demod_factory_spy = std::make_shared<ofdm_demodulator_factory_spy>();
      ASSERT_NE(ofdm_demod_factory_spy, nullptr);

      puxch_proc_factory = create_puxch_processor_factory_sw(request_queue_size, ofdm_demod_factory_spy);
      ASSERT_NE(puxch_proc_factory, nullptr);
    }
  }

  void SetUp() override
  {
    ASSERT_NE(puxch_proc_factory, nullptr);

    // Select parameters.
    unsigned           nof_rx_ports = std::get<0>(GetParam());
    sampling_rate      srate        = std::get<1>(GetParam());
    subcarrier_spacing scs          = std::get<2>(GetParam());
    cyclic_prefix      cp           = std::get<3>(GetParam());

    // Prepare configurations.
    config.cp                = cp;
    config.scs               = scs;
    config.srate             = srate;
    config.bandwidth_rb      = dist_bandwidth_prb(rgen);
    config.dft_window_offset = dist_dft_window_offset(rgen);
    config.center_freq_Hz    = dist_center_freq_Hz(rgen);
    config.nof_rx_ports      = nof_rx_ports;

    // Create processor.
    puxch_proc = puxch_proc_factory->create(config);
    ASSERT_NE(puxch_proc, nullptr);

    // Select OFDM demodulator processor spy.
    ofdm_demod_spy = ofdm_demod_factory_spy->get_demodulators().back();
  }

  static constexpr unsigned                            nof_frames_test = 3;
  static std::mt19937                                  rgen;
  static std::uniform_int_distribution<unsigned>       dist_sector_id;
  static std::uniform_int_distribution<unsigned>       dist_bandwidth_prb;
  static std::uniform_real_distribution<double>        dist_center_freq_Hz;
  static std::uniform_real_distribution<float>         dist_sample;
  static std::uniform_real_distribution<float>         dist_dft_window_offset;
  static std::shared_ptr<ofdm_demodulator_factory_spy> ofdm_demod_factory_spy;
  static std::shared_ptr<puxch_processor_factory>      puxch_proc_factory;

  puxch_processor_configuration    config;
  std::unique_ptr<puxch_processor> puxch_proc     = nullptr;
  ofdm_symbol_demodulator_spy*     ofdm_demod_spy = nullptr;
};

std::mt19937                                  LowerPhyUplinkProcessorFixture::rgen(0);
std::uniform_int_distribution<unsigned>       LowerPhyUplinkProcessorFixture::dist_sector_id(0, 16);
std::uniform_int_distribution<unsigned>       LowerPhyUplinkProcessorFixture::dist_bandwidth_prb(1, MAX_RB);
std::uniform_real_distribution<double>        LowerPhyUplinkProcessorFixture::dist_center_freq_Hz(1e8, 6e9);
std::uniform_real_distribution<float>         LowerPhyUplinkProcessorFixture::dist_sample(-1, 1);
std::uniform_real_distribution<float>         LowerPhyUplinkProcessorFixture::dist_dft_window_offset(0, 0.9);
std::shared_ptr<ofdm_demodulator_factory_spy> LowerPhyUplinkProcessorFixture::ofdm_demod_factory_spy = nullptr;
std::shared_ptr<puxch_processor_factory>      LowerPhyUplinkProcessorFixture::puxch_proc_factory     = nullptr;

} // namespace

TEST_P(LowerPhyUplinkProcessorFixture, DemodulatorConfiguration)
{
  sampling_rate      srate = std::get<1>(GetParam());
  subcarrier_spacing scs   = std::get<2>(GetParam());
  cyclic_prefix      cp    = std::get<3>(GetParam());

  puxch_processor_configuration expected_config;
  expected_config.cp                = cp;
  expected_config.scs               = scs;
  expected_config.srate             = srate;
  expected_config.bandwidth_rb      = config.bandwidth_rb;
  expected_config.dft_window_offset = config.dft_window_offset;
  expected_config.center_freq_Hz    = config.center_freq_Hz;
  expected_config.nof_rx_ports      = config.nof_rx_ports;

  ofdm_demodulator_configuration expected_demod_config;
  expected_demod_config.numerology                = to_numerology_value(scs);
  expected_demod_config.bw_rb                     = config.bandwidth_rb;
  expected_demod_config.dft_size                  = srate.get_dft_size(scs);
  expected_demod_config.cp                        = cp;
  expected_demod_config.nof_samples_window_offset = static_cast<unsigned>(
      static_cast<float>(cp.get_length(1, scs).to_samples(srate.to_Hz())) * config.dft_window_offset);
  expected_demod_config.scale          = 1.0F;
  expected_demod_config.center_freq_hz = config.center_freq_Hz;

  ASSERT_EQ(ofdm_demod_spy->get_configuration(), expected_demod_config);
}

TEST_P(LowerPhyUplinkProcessorFixture, FlowNoRequest)
{
  unsigned           nof_rx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<2>(GetParam());
  cyclic_prefix      cp           = std::get<3>(GetParam());

  unsigned base_symbol_size = srate.get_dft_size(scs);

  baseband_gateway_buffer_dynamic buffer(nof_rx_ports, 2 * base_symbol_size);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  puxch_processor_notifier_spy puxch_proc_notifier_spy;
  puxch_proc->connect(puxch_proc_notifier_spy);

  slot_point slot(to_numerology_value(scs), 0);
  for (unsigned i_frame = 0; i_frame != nof_frames_test; ++i_frame) {
    for (unsigned i_subframe = 0; i_subframe != NOF_SUBFRAMES_PER_FRAME; ++i_subframe) {
      for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot, ++slot) {
        for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
          unsigned cp_size = cp.get_length(i_symbol_subframe, scs).to_samples(srate.to_Hz());
          // Setup buffer.
          buffer.resize(cp_size + base_symbol_size);

          // Fill buffer.
          for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
            span<cf_t> port_buffer = buffer[i_port];
            std::generate(
                port_buffer.begin(), port_buffer.end(), []() { return cf_t(dist_sample(rgen), dist_sample(rgen)); });
          }

          // Clear spies.
          puxch_proc_notifier_spy.clear_notifications();
          ofdm_demod_spy->clear_demodulate_entries();

          // Prepare expected PUxCH baseband entry context.
          lower_phy_rx_symbol_context puxch_context;
          puxch_context.slot        = slot;
          puxch_context.sector      = dist_sector_id(rgen);
          puxch_context.nof_symbols = i_symbol_subframe;

          // Process baseband.
          puxch_proc->get_baseband().process_symbol(buffer.get_reader(), puxch_context);

          // Assert OFDM demodulator call.
          auto& ofdm_demod_entries = ofdm_demod_spy->get_demodulate_entries();
          ASSERT_EQ(ofdm_demod_entries.size(), 0);

          // Assert notification.
          ASSERT_EQ(puxch_proc_notifier_spy.get_nof_notifications(), 0);
        }
      }
    }
  }
}

TEST_P(LowerPhyUplinkProcessorFixture, FlowFloodRequest)
{
  unsigned           nof_rx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<2>(GetParam());
  cyclic_prefix      cp           = std::get<3>(GetParam());

  unsigned base_symbol_size = srate.get_dft_size(scs);

  baseband_gateway_buffer_dynamic buffer(nof_rx_ports, 2 * base_symbol_size);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  puxch_processor_notifier_spy puxch_proc_notifier_spy;
  puxch_proc->connect(puxch_proc_notifier_spy);

  slot_point slot(to_numerology_value(scs), 0);
  for (unsigned i_frame = 0; i_frame != nof_frames_test; ++i_frame) {
    for (unsigned i_subframe = 0; i_subframe != NOF_SUBFRAMES_PER_FRAME; ++i_subframe) {
      for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot, ++slot) {
        resource_grid_context rg_context;
        rg_context.slot   = slot;
        rg_context.sector = dist_sector_id(rgen);

        // Request resource grid demodulation for the current slot.
        resource_grid_spy rg_spy;
        puxch_proc->get_request_handler().handle_request(rg_spy, rg_context);

        for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
          unsigned cp_size = cp.get_length(i_symbol_subframe, scs).to_samples(srate.to_Hz());
          // Setup buffer.
          buffer.resize(cp_size + base_symbol_size);

          // Fill buffer.
          for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
            span<cf_t> port_buffer = buffer[i_port];
            std::generate(
                port_buffer.begin(), port_buffer.end(), []() { return cf_t(dist_sample(rgen), dist_sample(rgen)); });
          }

          // Clear spies.
          puxch_proc_notifier_spy.clear_notifications();
          ofdm_demod_spy->clear_demodulate_entries();

          // Prepare expected PUxCH baseband entry context.
          lower_phy_rx_symbol_context puxch_context;
          puxch_context.slot        = rg_context.slot;
          puxch_context.sector      = rg_context.sector;
          puxch_context.nof_symbols = i_symbol;

          // Process baseband.
          puxch_proc->get_baseband().process_symbol(buffer.get_reader(), puxch_context);

          // Assert OFDM demodulator call.
          const auto& ofdm_demod_entries = ofdm_demod_spy->get_demodulate_entries();
          ASSERT_EQ(ofdm_demod_entries.size(), nof_rx_ports);
          for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
            const auto& ofdm_demod_entry = ofdm_demod_entries[i_port];
            ASSERT_EQ(span<const cf_t>(ofdm_demod_entry.input), buffer[i_port]);
            ASSERT_EQ(static_cast<const void*>(ofdm_demod_entry.grid), static_cast<const void*>(&rg_spy.get_writer()));
            ASSERT_EQ(ofdm_demod_entry.port_index, i_port);
            ASSERT_EQ(ofdm_demod_entry.symbol_index, i_symbol_subframe);
          }

          // Assert notification.
          ASSERT_EQ(puxch_proc_notifier_spy.get_request_late().size(), 0);
          ASSERT_EQ(puxch_proc_notifier_spy.get_rx_symbol().size(), 1);
        }
      }
    }
  }
}

TEST_P(LowerPhyUplinkProcessorFixture, LateRequest)
{
  unsigned           sector_id    = dist_sector_id(rgen);
  unsigned           nof_rx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<2>(GetParam());
  cyclic_prefix      cp           = std::get<3>(GetParam());

  unsigned base_symbol_size = srate.get_dft_size(scs);

  baseband_gateway_buffer_dynamic buffer(nof_rx_ports, 2 * base_symbol_size);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  puxch_processor_notifier_spy puxch_proc_notifier_spy;
  puxch_proc->connect(puxch_proc_notifier_spy);

  unsigned initial_slot = 3;
  unsigned late_slot    = 2;
  unsigned next_slot    = 4;

  resource_grid_spy initial_rg_spy(0, 0, 0);
  resource_grid_spy late_rg_spy(0, 0, 0);
  resource_grid_spy next_rg_spy(0, 0, 0);

  // Initial request.
  resource_grid_context initial_rg_context;
  initial_rg_context.slot   = slot_point(to_numerology_value(scs), initial_slot);
  initial_rg_context.sector = sector_id;
  puxch_proc->get_request_handler().handle_request(initial_rg_spy, initial_rg_context);

  // Late request.
  resource_grid_context late_rg_context;
  late_rg_context.slot   = slot_point(to_numerology_value(scs), late_slot);
  late_rg_context.sector = sector_id;
  puxch_proc->get_request_handler().handle_request(late_rg_spy, late_rg_context);

  // Next request.
  resource_grid_context next_rg_context;
  next_rg_context.slot   = slot_point(to_numerology_value(scs), next_slot);
  next_rg_context.sector = sector_id;
  puxch_proc->get_request_handler().handle_request(next_rg_spy, next_rg_context);

  for (unsigned i_subframe = 0; i_subframe != NOF_SUBFRAMES_PER_FRAME; ++i_subframe) {
    for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot) {
      // Process one frame.
      for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
        unsigned cp_size = cp.get_length(i_symbol_subframe, scs).to_samples(srate.to_Hz());
        // Setup buffer.
        buffer.resize(cp_size + base_symbol_size);

        // Fill buffer.
        for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
          span<cf_t> port_buffer = buffer[i_port];
          std::generate(
              port_buffer.begin(), port_buffer.end(), []() { return cf_t(dist_sample(rgen), dist_sample(rgen)); });
        }

        // Clear spies.
        puxch_proc_notifier_spy.clear_notifications();
        ofdm_demod_spy->clear_demodulate_entries();

        // Prepare expected PUxCH baseband entry context.
        lower_phy_rx_symbol_context puxch_context;
        puxch_context.slot        = slot_point(to_numerology_value(scs), i_slot);
        puxch_context.sector      = sector_id;
        puxch_context.nof_symbols = i_symbol;

        // Process baseband.
        puxch_proc->get_baseband().process_symbol(buffer.get_reader(), puxch_context);

        // Assert OFDM demodulator call only for initial and next slot.
        const auto& ofdm_demod_entries = ofdm_demod_spy->get_demodulate_entries();
        if ((i_slot == initial_slot) || (i_slot == next_slot)) {
          resource_grid_spy* rg_spy = (i_slot == initial_slot) ? &initial_rg_spy : &next_rg_spy;
          ASSERT_EQ(ofdm_demod_entries.size(), nof_rx_ports);
          for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
            const auto& ofdm_demod_entry = ofdm_demod_entries[i_port];
            ASSERT_EQ(span<const cf_t>(ofdm_demod_entry.input), buffer[i_port]);
            ASSERT_EQ(static_cast<const void*>(ofdm_demod_entry.grid), static_cast<const void*>(rg_spy));
            ASSERT_EQ(ofdm_demod_entry.port_index, i_port);
            ASSERT_EQ(ofdm_demod_entry.symbol_index, i_symbol_subframe);
          }
        } else {
          ASSERT_EQ(ofdm_demod_entries.size(), 0);
        }

        // Assert notifications.
        if (i_slot == next_slot) {
          const auto& lates = puxch_proc_notifier_spy.get_request_late();
          ASSERT_EQ(lates.size(), 1);
          ASSERT_EQ(lates.front().slot, late_rg_context.slot);
          ASSERT_EQ(lates.front().sector, late_rg_context.sector);
        } else {
          ASSERT_EQ(puxch_proc_notifier_spy.get_request_late().size(), 0);
        }
        if ((i_slot == initial_slot) || (i_slot == next_slot)) {
          ASSERT_EQ(puxch_proc_notifier_spy.get_rx_symbol().size(), 1);
        }
      }
    }
  }
}

TEST_P(LowerPhyUplinkProcessorFixture, OverflowRequest)
{
  static constexpr unsigned nof_overflow_entries = 3;
  unsigned                  sector_id            = dist_sector_id(rgen);
  unsigned                  nof_rx_ports         = std::get<0>(GetParam());
  sampling_rate             srate                = std::get<1>(GetParam());
  subcarrier_spacing        scs                  = std::get<2>(GetParam());

  unsigned base_symbol_size = srate.get_dft_size(scs);

  baseband_gateway_buffer_dynamic buffer(nof_rx_ports, 2 * base_symbol_size);

  // Create notifiers and connect.
  puxch_processor_notifier_spy puxch_proc_notifier_spy;
  puxch_proc->connect(puxch_proc_notifier_spy);

  resource_grid_spy rg_spy;

  // Generate requests.
  slot_point slot(to_numerology_value(scs), 0);
  for (unsigned i_request = 0; i_request != request_queue_size + nof_overflow_entries; ++i_request) {
    resource_grid_context rg_context;
    rg_context.slot   = slot + i_request;
    rg_context.sector = sector_id;
    puxch_proc->get_request_handler().handle_request(rg_spy, rg_context);

    unsigned nof_expected_late = (i_request >= request_queue_size) ? (i_request - request_queue_size + 1) : 0;
    ASSERT_EQ(puxch_proc_notifier_spy.get_rx_symbol().size(), 0);
    ASSERT_EQ(puxch_proc_notifier_spy.get_request_late().size(), nof_expected_late);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(LowerPhyUplinkProcessor,
                         LowerPhyUplinkProcessorFixture,
                         ::testing::Combine(::testing::Values(1, 2, 4),
                                            ::testing::Values(sampling_rate::from_MHz(3.84),
                                                              sampling_rate::from_MHz(7.68)),
                                            ::testing::Values(subcarrier_spacing::kHz15, subcarrier_spacing::kHz30),
                                            ::testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED)));
