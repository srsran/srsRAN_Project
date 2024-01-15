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

#include "../../amplitude_control/amplitude_controller_test_doubles.h"
#include "downlink_processor_notifier_test_doubles.h"
#include "pdxch/pdxch_processor_notifier_test_doubles.h"
#include "pdxch/pdxch_processor_test_doubles.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader_view.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_writer_view.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_factories.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, const pdxch_processor_baseband::symbol_context& context)
{
  fmt::print(os, "{} {} {}", context.slot, context.sector, context.symbol);
  return os;
}

std::ostream& operator<<(std::ostream& os, const sampling_rate& srate)
{
  fmt::print(os, "{}", srate);
  return os;
}

std::ostream& operator<<(std::ostream& os, const subcarrier_spacing& scs)
{
  fmt::print(os, "{}", to_string(scs));
  return os;
}

std::ostream& operator<<(std::ostream& os, const cyclic_prefix& cp)
{
  fmt::print(os, "{}", cp.to_string());
  return os;
}

std::ostream& operator<<(std::ostream& os, const pdxch_processor_configuration& config)
{
  fmt::print(os,
             "CP={} SCS={} SRate={} BW={} CenterFreq={}Hz NofTxPorts={}",
             config.cp,
             to_string(config.scs),
             config.srate,
             config.bandwidth_rb,
             config.center_freq_Hz,
             config.nof_tx_ports);
  return os;
}

bool operator==(const pdxch_processor_baseband::symbol_context left,
                const pdxch_processor_baseband::symbol_context right)
{
  return (left.slot == right.slot) && (left.symbol == right.symbol) && (left.sector == right.sector);
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

bool operator==(const pdxch_processor_configuration& left, const pdxch_processor_configuration& right)
{
  return (left.cp == right.cp) && (left.scs == right.scs) && (left.srate == right.srate) &&
         (left.bandwidth_rb == right.bandwidth_rb) && (left.center_freq_Hz == right.center_freq_Hz) &&
         (left.nof_tx_ports == right.nof_tx_ports);
}

} // namespace srsran

using LowerPhyDownlinkProcessorParams =
    std::tuple<unsigned, sampling_rate, std::tuple<subcarrier_spacing, cyclic_prefix>>;

namespace {

class LowerPhyDownlinkProcessorFixture : public ::testing::TestWithParam<LowerPhyDownlinkProcessorParams>
{
protected:
  static void SetUpTestSuite()
  {
    if (dl_proc_factory == nullptr) {
      pdxch_proc_factory = std::make_shared<pdxch_processor_factory_spy>();
      ASSERT_NE(pdxch_proc_factory, nullptr);

      amplitude_control_factory = std::make_shared<amplitude_controller_factory_spy>();
      ASSERT_NE(amplitude_control_factory, nullptr);

      dl_proc_factory = create_downlink_processor_factory_sw(pdxch_proc_factory, amplitude_control_factory);
      ASSERT_NE(dl_proc_factory, nullptr);
    }
  }

  void SetUp() override
  {
    ASSERT_NE(dl_proc_factory, nullptr);

    // Select parameters.
    unsigned           nof_tx_ports = std::get<0>(GetParam());
    sampling_rate      srate        = std::get<1>(GetParam());
    subcarrier_spacing scs          = std::get<0>(std::get<2>(GetParam()));
    cyclic_prefix      cp           = std::get<1>(std::get<2>(GetParam()));

    // Prepare configurations.
    config.sector_id           = 0;
    config.scs                 = scs;
    config.cp                  = cp;
    config.rate                = srate;
    config.bandwidth_prb       = dist_bandwidth_prb(rgen);
    config.center_frequency_Hz = dist_center_freq_Hz(rgen);
    config.nof_tx_ports        = nof_tx_ports;
    config.logger              = &srslog::fetch_basic_logger("Low-PHY");

    // Create processor.
    dl_processor = dl_proc_factory->create(config);
    ASSERT_NE(dl_processor, nullptr);

    // Select PDxCH processor spy.
    pdxch_proc_spy = &pdxch_proc_factory->get_spy();

    // Select amplitude controller spy.
    amplitude_control_spy = amplitude_control_factory->get_entries().back();
  }

  static constexpr unsigned                                    nof_frames_test = 3;
  static std::mt19937                                          rgen;
  static std::uniform_int_distribution<unsigned>               dist_bandwidth_prb;
  static std::uniform_real_distribution<double>                dist_center_freq_Hz;
  static std::shared_ptr<pdxch_processor_factory_spy>          pdxch_proc_factory;
  static std::shared_ptr<amplitude_controller_factory_spy>     amplitude_control_factory;
  static std::shared_ptr<lower_phy_downlink_processor_factory> dl_proc_factory;

  downlink_processor_configuration              config;
  std::unique_ptr<lower_phy_downlink_processor> dl_processor          = nullptr;
  pdxch_processor_spy*                          pdxch_proc_spy        = nullptr;
  amplitude_controller_spy*                     amplitude_control_spy = nullptr;
};

std::mt19937                                      LowerPhyDownlinkProcessorFixture::rgen(0);
std::uniform_int_distribution<unsigned>           LowerPhyDownlinkProcessorFixture::dist_bandwidth_prb(1, MAX_RB);
std::uniform_real_distribution<double>            LowerPhyDownlinkProcessorFixture::dist_center_freq_Hz(1e8, 6e9);
std::shared_ptr<pdxch_processor_factory_spy>      LowerPhyDownlinkProcessorFixture::pdxch_proc_factory        = nullptr;
std::shared_ptr<amplitude_controller_factory_spy> LowerPhyDownlinkProcessorFixture::amplitude_control_factory = nullptr;
std::shared_ptr<lower_phy_downlink_processor_factory> LowerPhyDownlinkProcessorFixture::dl_proc_factory       = nullptr;

// Fixture class used to test the transmit buffer metadata generated by the lower phy downlink processor.
class LowerPhyDownlinkProcessorMetadataFixture : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a PDxCH processor spy that only processes symbols belonging to some slots.
    std::shared_ptr<pdxch_processor_factory_spy> pdxch_proc_factory =
        std::make_shared<pdxch_processor_factory_spy>(frame_slots_to_process);
    ASSERT_NE(pdxch_proc_factory, nullptr);

    std::shared_ptr<amplitude_controller_factory_spy> amplitude_control_factory =
        std::make_shared<amplitude_controller_factory_spy>();
    ASSERT_NE(amplitude_control_factory, nullptr);

    std::shared_ptr<lower_phy_downlink_processor_factory> dl_proc_factory =
        create_downlink_processor_factory_sw(pdxch_proc_factory, amplitude_control_factory);
    ASSERT_NE(dl_proc_factory, nullptr);

    // Set test parameters.
    scs          = subcarrier_spacing::kHz30;
    cp           = cyclic_prefix::NORMAL;
    srate        = sampling_rate::from_MHz(7.68);
    nof_tx_ports = 1;

    // Prepare configuration.
    downlink_processor_configuration config;
    config.sector_id           = 0;
    config.scs                 = scs;
    config.cp                  = cp;
    config.rate                = srate;
    config.bandwidth_prb       = MAX_RB;
    config.center_frequency_Hz = 3.5e6;
    config.nof_tx_ports        = nof_tx_ports;
    config.logger              = &srslog::fetch_basic_logger("Low-PHY");

    // Create processor.
    dl_processor = dl_proc_factory->create(config);
    ASSERT_NE(dl_processor, nullptr);
  }

  std::unique_ptr<lower_phy_downlink_processor> dl_processor = nullptr;

  static constexpr unsigned nof_frames_test = 1;

  // slots within a frame that will be processed by the lower PHY.
  std::vector<unsigned> frame_slots_to_process = {1, 9};

  // configuration parameters.
  sampling_rate      srate;
  subcarrier_spacing scs;
  cyclic_prefix      cp;
  unsigned           nof_tx_ports;

private:
};

} // namespace

TEST_P(LowerPhyDownlinkProcessorFixture, PdxchConfiguration)
{
  // Assert PDxCH factory configuration.
  pdxch_processor_configuration expected_pdxch_config;
  expected_pdxch_config.cp             = config.cp;
  expected_pdxch_config.scs            = config.scs;
  expected_pdxch_config.srate          = config.rate;
  expected_pdxch_config.bandwidth_rb   = config.bandwidth_prb;
  expected_pdxch_config.center_freq_Hz = config.center_frequency_Hz;
  expected_pdxch_config.nof_tx_ports   = config.nof_tx_ports;
  ASSERT_EQ(expected_pdxch_config, pdxch_proc_spy->get_configuration());
}

TEST_P(LowerPhyDownlinkProcessorFixture, PdxchConnection)
{
  // Create notifiers.
  downlink_processor_notifier_spy downlink_proc_notifier_spy;
  pdxch_processor_notifier_spy    pdxch_proc_notifier_spy;

  // Connect.
  dl_processor->connect(downlink_proc_notifier_spy, pdxch_proc_notifier_spy);

  // Assert no notifications.
  ASSERT_EQ(downlink_proc_notifier_spy.get_total_count(), 0);
  ASSERT_EQ(pdxch_proc_notifier_spy.get_total_count(), 0);

  // Assert notifier connection.
  ASSERT_EQ(reinterpret_cast<const void*>(&pdxch_proc_notifier_spy),
            reinterpret_cast<const void*>(pdxch_proc_spy->get_notifier()));
}

TEST_P(LowerPhyDownlinkProcessorFixture, PdxchRequestHandler)
{
  // Get expected request handler pointer.
  const pdxch_processor_request_handler* expected_request_handler = &pdxch_proc_spy->get_request_handler();

  // Get actual request handler.
  const pdxch_processor_request_handler* request_handler = &dl_processor->get_downlink_request_handler();

  // Assert request handler.
  ASSERT_EQ(reinterpret_cast<const void*>(expected_request_handler), reinterpret_cast<const void*>(request_handler));
}

TEST_P(LowerPhyDownlinkProcessorFixture, Flow)
{
  unsigned           nof_tx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<0>(std::get<2>(GetParam()));
  cyclic_prefix      cp           = std::get<1>(std::get<2>(GetParam()));

  unsigned base_symbol_size = srate.get_dft_size(scs);

  baseband_gateway_buffer_dynamic buffer(nof_tx_ports, 2 * base_symbol_size);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  downlink_processor_notifier_spy downlink_proc_notifier_spy;
  pdxch_processor_notifier_spy    pdxch_proc_notifier_spy;
  dl_processor->connect(downlink_proc_notifier_spy, pdxch_proc_notifier_spy);

  downlink_processor_baseband& dl_proc_baseband = dl_processor->get_baseband();

  baseband_gateway_timestamp timestamp = 0;
  for (unsigned i_frame = 0, i_slot_frame = 0; i_frame != nof_frames_test; ++i_frame) {
    for (unsigned i_subframe = 0; i_subframe != NOF_SUBFRAMES_PER_FRAME; ++i_subframe) {
      for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot, ++i_slot_frame) {
        for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
          // Calculate cyclic prefix size in samples.
          unsigned cp_size = cp.get_length(i_symbol_subframe, scs).to_samples(srate.to_Hz());

          // Setup buffer.
          buffer.resize(cp_size + base_symbol_size);

          // Clear spies.
          downlink_proc_notifier_spy.clear_notifications();
          pdxch_proc_notifier_spy.clear_notifications();
          pdxch_proc_spy->clear();
          amplitude_control_spy->clear();

          // Process baseband.
          baseband_gateway_transmitter_metadata md = dl_proc_baseband.process(buffer.get_writer(), timestamp);

          // Assert buffer metadata.
          ASSERT_FALSE(md.is_empty);
          ASSERT_FALSE(md.tx_start.has_value());
          ASSERT_FALSE(md.tx_end.has_value());

          // Prepare expected PDxCH baseband entry context.
          pdxch_processor_baseband::symbol_context pdxch_context;
          pdxch_context.slot   = slot_point(to_numerology_value(scs), i_slot_frame);
          pdxch_context.sector = config.sector_id;
          pdxch_context.symbol = i_symbol;

          // Assert PDxCH processor call.
          auto& pdxch_proc_entries = pdxch_proc_spy->get_baseband_entries();
          ASSERT_EQ(pdxch_proc_entries.size(), 1);
          auto& pdxch_proc_entry = pdxch_proc_entries.back();
          ASSERT_EQ(pdxch_proc_entry.context, pdxch_context);

          // Assert amplitude controller.
          auto& amplitude_controller_entries = amplitude_control_spy->get_entries();
          ASSERT_EQ(amplitude_controller_entries.size(), config.nof_tx_ports);
          for (unsigned i_port = 0, i_port_end = config.nof_tx_ports; i_port != i_port_end; ++i_port) {
            // Make sure the amplitude controller input match with the PDxCH processor output.
            ASSERT_EQ(span<const cf_t>(amplitude_controller_entries[i_port].input),
                      span<const cf_t>(pdxch_proc_entry.samples[i_port]));
            // Make sure the amplitude controller output match with the resultant buffer contents.
            ASSERT_EQ(span<const cf_t>(amplitude_controller_entries[i_port].output), buffer[i_port]);
          }

          // No PDxCH notifications.
          ASSERT_EQ(pdxch_proc_notifier_spy.get_nof_notifications(), 0);

          // Assert TTI boundary notification.
          const auto& tti_boundary_entries = downlink_proc_notifier_spy.get_tti_boundaries();
          if (i_symbol == 0) {
            ASSERT_EQ(tti_boundary_entries.size(), 1);
          } else {
            ASSERT_EQ(tti_boundary_entries.size(), 0);
          }

          // Increment timestamp.
          timestamp += cp_size + base_symbol_size;
        }
      }
    }
  }
}

// Test baseband buffer sizes which are unaligned with OFDM symbol boundaries.
TEST_P(LowerPhyDownlinkProcessorFixture, FlowUnalignedBuffer)
{
  unsigned           nof_tx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<0>(std::get<2>(GetParam()));
  cyclic_prefix      cp           = std::get<1>(std::get<2>(GetParam()));

  unsigned base_symbol_size = srate.get_dft_size(scs);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  downlink_processor_notifier_spy downlink_proc_notifier_spy;
  pdxch_processor_notifier_spy    pdxch_proc_notifier_spy;
  dl_processor->connect(downlink_proc_notifier_spy, pdxch_proc_notifier_spy);

  downlink_processor_baseband& dl_proc_baseband = dl_processor->get_baseband();

  // Create basband buffer.
  baseband_gateway_buffer_dynamic buffer(nof_tx_ports, 3 * base_symbol_size);

  // Reset timestamp.
  baseband_gateway_timestamp timestamp = 0;

  // Process OFDM symbols in groups of 2 and assert.
  for (unsigned i_frame = 0, i_slot_frame = 0; i_frame != nof_frames_test; ++i_frame) {
    for (unsigned i_subframe = 0; i_subframe != NOF_SUBFRAMES_PER_FRAME; ++i_subframe) {
      for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot, ++i_slot_frame) {
        for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; i_symbol += 2, i_symbol_subframe += 2) {
          // Calculate cyclic prefix sizes in samples.
          unsigned cp_size_1 = cp.get_length(i_symbol_subframe, scs).to_samples(srate.to_Hz());
          unsigned cp_size_2 = cp.get_length(i_symbol_subframe + 1, scs).to_samples(srate.to_Hz());

          // Size of the two consecutive OFDM symbols to be processed.
          unsigned symbols_size = cp_size_1 + cp_size_2 + 2 * base_symbol_size;

          // Resize the buffer to hold two OFDM symbols.
          buffer.resize(symbols_size);

          // Clear spies.
          downlink_proc_notifier_spy.clear_notifications();
          pdxch_proc_notifier_spy.clear_notifications();
          pdxch_proc_spy->clear();
          amplitude_control_spy->clear();

          // Process the two OFDM symbols in chunks of smaller size that are not aligned with symbol boundaries.
          baseband_gateway_buffer_writer&     writer     = buffer.get_writer();
          unsigned                            chunk_size = (symbols_size / 3) - 1;
          baseband_gateway_buffer_writer_view buffer_view_1(writer, 0, chunk_size);
          baseband_gateway_buffer_writer_view buffer_view_2(writer, chunk_size, chunk_size);
          baseband_gateway_buffer_writer_view buffer_view_3(writer, 2 * chunk_size, symbols_size - 2 * chunk_size);

          baseband_gateway_transmitter_metadata md_1 = dl_proc_baseband.process(buffer_view_1, timestamp);
          baseband_gateway_transmitter_metadata md_2 = dl_proc_baseband.process(buffer_view_2, timestamp + chunk_size);
          baseband_gateway_transmitter_metadata md_3 =
              dl_proc_baseband.process(buffer_view_3, timestamp + 2 * chunk_size);

          // Assert buffer metadata.
          ASSERT_FALSE(md_1.is_empty);
          ASSERT_FALSE(md_1.tx_start.has_value());
          ASSERT_FALSE(md_1.tx_end.has_value());

          ASSERT_FALSE(md_2.is_empty);
          ASSERT_FALSE(md_2.tx_start.has_value());
          ASSERT_FALSE(md_2.tx_end.has_value());

          ASSERT_FALSE(md_3.is_empty);
          ASSERT_FALSE(md_3.tx_start.has_value());
          ASSERT_FALSE(md_3.tx_end.has_value());

          // Assert one PDxCH processor call per symbol.
          auto& pdxch_proc_entries = pdxch_proc_spy->get_baseband_entries();
          ASSERT_EQ(pdxch_proc_entries.size(), 2);

          // Prepare expected PDxCH baseband entry context for the first symbol.
          pdxch_processor_baseband::symbol_context pdxch_context_1;
          pdxch_context_1.slot   = slot_point(to_numerology_value(scs), i_slot_frame);
          pdxch_context_1.sector = config.sector_id;
          pdxch_context_1.symbol = i_symbol;

          // Prepare expected PDxCH baseband entry context for the second symbol.
          pdxch_processor_baseband::symbol_context pdxch_context_2;
          pdxch_context_2.slot   = slot_point(to_numerology_value(scs), i_slot_frame);
          pdxch_context_2.sector = config.sector_id;
          pdxch_context_2.symbol = i_symbol + 1;

          // Assert PDxCH baseband entry contexts.
          auto& pdxch_proc_entry_1 = pdxch_proc_entries.front();
          auto& pdxch_proc_entry_2 = pdxch_proc_entries.back();
          ASSERT_EQ(pdxch_proc_entry_1.context, pdxch_context_1);
          ASSERT_EQ(pdxch_proc_entry_2.context, pdxch_context_2);

          // Assert one amplitude controller call per symbol.
          auto& amplitude_controller_entries = amplitude_control_spy->get_entries();
          ASSERT_EQ(amplitude_controller_entries.size(), 2 * config.nof_tx_ports);

          // Prepare output views for both symbols.
          unsigned                            symbol_1_size = cp_size_1 + base_symbol_size;
          unsigned                            symbol_2_size = cp_size_2 + base_symbol_size;
          baseband_gateway_buffer_reader_view out_symbol_1(buffer.get_reader(), 0, symbol_1_size);
          baseband_gateway_buffer_reader_view out_symbol_2(buffer.get_reader(), symbol_1_size, symbol_2_size);

          for (unsigned i_port = 0, i_port_end = config.nof_tx_ports; i_port != i_port_end; ++i_port) {
            unsigned symbol_1_entry = i_port;
            unsigned symbol_2_entry = config.nof_tx_ports + i_port;

            // Make sure the amplitude controller input matches with the PDxCH processor output for each symbol.
            ASSERT_EQ(span<const cf_t>(amplitude_controller_entries[symbol_1_entry].input),
                      span<const cf_t>(pdxch_proc_entry_1.samples[i_port]));

            ASSERT_EQ(span<const cf_t>(amplitude_controller_entries[symbol_2_entry].input),
                      span<const cf_t>(pdxch_proc_entry_2.samples[i_port]));

            // Make sure the amplitude controller output matches with the resultant buffer contents for each symbol.
            span<const cf_t> amplitude_controller_output_1(amplitude_controller_entries[symbol_1_entry].output);
            span<const cf_t> amplitude_controller_output_2(amplitude_controller_entries[symbol_2_entry].output);

            ASSERT_EQ(amplitude_controller_output_1, out_symbol_1[i_port]);
            ASSERT_EQ(amplitude_controller_output_2, out_symbol_2[i_port]);
          }

          // No PDxCH notifications.
          ASSERT_EQ(pdxch_proc_notifier_spy.get_nof_notifications(), 0);

          // Assert TTI boundary notification.
          const auto& tti_boundary_entries = downlink_proc_notifier_spy.get_tti_boundaries();
          if (i_symbol == 0) {
            ASSERT_EQ(tti_boundary_entries.size(), 1);
          } else {
            ASSERT_EQ(tti_boundary_entries.size(), 0);
          }

          // Increment timestamp.
          timestamp += buffer.get_nof_samples();
        }
      }
    }
  }
}

// Test a baseband buffer size comprising an entire slot.
TEST_P(LowerPhyDownlinkProcessorFixture, FlowLargeBuffer)
{
  unsigned           nof_tx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<0>(std::get<2>(GetParam()));
  cyclic_prefix      cp           = std::get<1>(std::get<2>(GetParam()));

  unsigned base_symbol_size = srate.get_dft_size(scs);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  downlink_processor_notifier_spy downlink_proc_notifier_spy;
  pdxch_processor_notifier_spy    pdxch_proc_notifier_spy;
  dl_processor->connect(downlink_proc_notifier_spy, pdxch_proc_notifier_spy);

  downlink_processor_baseband& dl_proc_baseband = dl_processor->get_baseband();

  baseband_gateway_buffer_dynamic buffer(nof_tx_ports, 20 * base_symbol_size);

  // Reset timestamp.
  baseband_gateway_timestamp timestamp = 0;

  for (unsigned i_frame = 0, i_slot_frame = 0; i_frame != nof_frames_test; ++i_frame) {
    for (unsigned i_subframe = 0; i_subframe != NOF_SUBFRAMES_PER_FRAME; ++i_subframe) {
      for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot, ++i_slot_frame) {
        // Resize baseband buffer to hold an entire slot.
        unsigned slot_size = 0;
        for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol) {
          unsigned cp_size = cp.get_length(i_symbol_subframe + i_symbol, scs).to_samples(srate.to_Hz());
          slot_size += cp_size + base_symbol_size;
        }
        buffer.resize(slot_size);

        // Clear spies.
        downlink_proc_notifier_spy.clear_notifications();
        pdxch_proc_notifier_spy.clear_notifications();
        pdxch_proc_spy->clear();
        amplitude_control_spy->clear();

        // Process slot in one go.
        baseband_gateway_transmitter_metadata md = dl_proc_baseband.process(buffer.get_writer(), timestamp);

        // Assert buffer metadata.
        ASSERT_FALSE(md.is_empty);
        ASSERT_FALSE(md.tx_start.has_value());
        ASSERT_FALSE(md.tx_end.has_value());

        // Assert PDxCH processor calls.
        auto& pdxch_proc_entries = pdxch_proc_spy->get_baseband_entries();
        ASSERT_EQ(pdxch_proc_entries.size(), nof_symbols_per_slot);

        // Assert amplitude controller calls.
        auto& amplitude_controller_entries = amplitude_control_spy->get_entries();
        ASSERT_EQ(amplitude_controller_entries.size(), config.nof_tx_ports * nof_symbols_per_slot);

        // No PDxCH notifications.
        ASSERT_EQ(pdxch_proc_notifier_spy.get_nof_notifications(), 0);

        // Assert TTI boundary notification.
        const auto& tti_boundary_entries = downlink_proc_notifier_spy.get_tti_boundaries();
        ASSERT_EQ(tti_boundary_entries.size(), 1);

        unsigned i_symbol_begin = 0;
        for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
          // Prepare expected PDxCH baseband entry context.
          pdxch_processor_baseband::symbol_context pdxch_context;
          pdxch_context.slot   = slot_point(to_numerology_value(scs), i_slot_frame);
          pdxch_context.sector = config.sector_id;
          pdxch_context.symbol = i_symbol;

          auto& pdxch_proc_entry = pdxch_proc_entries[i_symbol];
          ASSERT_EQ(pdxch_proc_entry.context, pdxch_context);

          unsigned cp_size       = cp.get_length(i_symbol_subframe, scs).to_samples(srate.to_Hz());
          unsigned i_symbol_size = base_symbol_size + cp_size;

          baseband_gateway_buffer_reader_view out_i_symbol(buffer.get_reader(), i_symbol_begin, i_symbol_size);

          for (unsigned i_port = 0, i_port_end = config.nof_tx_ports; i_port != i_port_end; ++i_port) {
            unsigned i_entry = i_symbol * i_port_end + i_port;
            // Make sure the amplitude controller input match with the PDxCH processor output.
            ASSERT_EQ(span<const cf_t>(amplitude_controller_entries[i_entry].input),
                      span<const cf_t>(pdxch_proc_entry.samples[i_port]));
            // Make sure the amplitude controller output matches with the resultant buffer contents.
            span<const cf_t> amplitude_controller_output(amplitude_controller_entries[i_entry].output);
            ASSERT_EQ(amplitude_controller_output, out_i_symbol[i_port]);
          }

          // Increment symbol iterator.
          i_symbol_begin += i_symbol_size;
        }
        // Increment timestamp.
        timestamp += buffer.get_nof_samples();
      }
    }
  }
}

// Test generation of baseband samples for discontinous timestamps.
TEST_P(LowerPhyDownlinkProcessorFixture, DiscontinuousProcessing)
{
  unsigned           nof_tx_ports = std::get<0>(GetParam());
  sampling_rate      srate        = std::get<1>(GetParam());
  subcarrier_spacing scs          = std::get<0>(std::get<2>(GetParam()));
  cyclic_prefix      cp           = std::get<1>(std::get<2>(GetParam()));

  unsigned base_symbol_size = srate.get_dft_size(scs);
  unsigned cp_symbol_0_size = cp.get_length(0, scs).to_samples(srate.to_Hz());
  unsigned cp_symbol_1_size = cp.get_length(1, scs).to_samples(srate.to_Hz());

  // Create notifiers and connect.
  downlink_processor_notifier_spy downlink_proc_notifier_spy;
  pdxch_processor_notifier_spy    pdxch_proc_notifier_spy;
  dl_processor->connect(downlink_proc_notifier_spy, pdxch_proc_notifier_spy);

  downlink_processor_baseband& dl_proc_baseband = dl_processor->get_baseband();

  // Create baseband buffers.
  baseband_gateway_buffer_dynamic buffer_symbol_0(nof_tx_ports, base_symbol_size);
  baseband_gateway_buffer_dynamic buffer_symbol_2(nof_tx_ports, base_symbol_size);

  // Set an initial timestamp.
  baseband_gateway_timestamp timestamp_symbol_0 = 7;
  // Set a timestamp belonging to the third OFDM symbol in the slot.
  baseband_gateway_timestamp timestamp_symbol_2 = 2 * base_symbol_size + cp_symbol_0_size + cp_symbol_1_size;

  // Clear spies.
  downlink_proc_notifier_spy.clear_notifications();
  pdxch_proc_notifier_spy.clear_notifications();
  pdxch_proc_spy->clear();
  amplitude_control_spy->clear();

  // Process only a portion of symbol 0, forcing utilization of the temporary buffer.
  baseband_gateway_transmitter_metadata md_1 =
      dl_proc_baseband.process(buffer_symbol_0.get_writer(), timestamp_symbol_0);

  // Process a portion of symbol 2, causing a timestamp jump.
  baseband_gateway_transmitter_metadata md_2 =
      dl_proc_baseband.process(buffer_symbol_2.get_writer(), timestamp_symbol_2);

  // Assert buffer metadata.
  ASSERT_FALSE(md_1.is_empty);
  ASSERT_FALSE(md_1.tx_start.has_value());
  ASSERT_FALSE(md_1.tx_end.has_value());

  ASSERT_FALSE(md_2.is_empty);
  ASSERT_FALSE(md_2.tx_start.has_value());
  ASSERT_FALSE(md_2.tx_end.has_value());

  // Assert PDxCH processor calls.
  auto& pdxch_proc_entries = pdxch_proc_spy->get_baseband_entries();
  ASSERT_EQ(pdxch_proc_entries.size(), 2);

  // Prepare expected PDxCH baseband entry context for the first symbol.
  pdxch_processor_baseband::symbol_context pdxch_context_1;
  pdxch_context_1.slot   = slot_point(to_numerology_value(scs), 0);
  pdxch_context_1.sector = config.sector_id;
  pdxch_context_1.symbol = 0;

  // Prepare expected PDxCH baseband entry context for the second symbol.
  pdxch_processor_baseband::symbol_context pdxch_context_2;
  pdxch_context_2.slot   = slot_point(to_numerology_value(scs), 0);
  pdxch_context_2.sector = config.sector_id;
  pdxch_context_2.symbol = 2;

  // Assert PDxCH baseband entry contexts.
  auto& pdxch_proc_entry_1 = pdxch_proc_entries.front();
  auto& pdxch_proc_entry_2 = pdxch_proc_entries.back();
  ASSERT_EQ(pdxch_proc_entry_1.context, pdxch_context_1);
  ASSERT_EQ(pdxch_proc_entry_2.context, pdxch_context_2);

  // Assert one amplitude controller call per symbol.
  auto& amplitude_controller_entries = amplitude_control_spy->get_entries();
  ASSERT_EQ(amplitude_controller_entries.size(), 2 * config.nof_tx_ports);

  for (unsigned i_port = 0, i_port_end = config.nof_tx_ports; i_port != i_port_end; ++i_port) {
    unsigned symbol_0_entry = i_port;
    unsigned symbol_2_entry = config.nof_tx_ports + i_port;

    // Make sure the amplitude controller input matches with the PDxCH processor output for each symbol.
    ASSERT_EQ(span<const cf_t>(amplitude_controller_entries[symbol_0_entry].input),
              span<const cf_t>(pdxch_proc_entry_1.samples[i_port]));

    ASSERT_EQ(span<const cf_t>(amplitude_controller_entries[symbol_2_entry].input),
              span<const cf_t>(pdxch_proc_entry_2.samples[i_port]));

    span<const cf_t> output_0(amplitude_controller_entries[symbol_0_entry].output);
    span<const cf_t> output_2(amplitude_controller_entries[symbol_2_entry].output);

    // Align the amplitude controller outputs, containing the entire symbol, to the baseband buffer contents.
    output_0 = output_0.subspan(timestamp_symbol_0, buffer_symbol_0.get_nof_samples());
    output_2 = output_2.first(buffer_symbol_2.get_nof_samples());

    // Make sure the amplitude controller output matches with the resultant buffer contents for each symbol.
    ASSERT_EQ(output_0, buffer_symbol_0[i_port]);
    ASSERT_EQ(output_2, buffer_symbol_2[i_port]);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(
    LowerPhyDownlinkProcessor,
    LowerPhyDownlinkProcessorFixture,
    ::testing::Combine(::testing::Values(1, 4),
                       ::testing::Values(sampling_rate::from_MHz(7.68)),
                       ::testing::Values(std::make_tuple(subcarrier_spacing::kHz15, cyclic_prefix::NORMAL),
                                         std::make_tuple(subcarrier_spacing::kHz30, cyclic_prefix::NORMAL),
                                         std::make_tuple(subcarrier_spacing::kHz60, cyclic_prefix::EXTENDED))));

// Test buffer transmit metadata for different transmit buffer alignments.
TEST_F(LowerPhyDownlinkProcessorMetadataFixture, DiscontinuousTransmitMetadata)
{
  unsigned base_symbol_size = srate.get_dft_size(scs);

  baseband_gateway_buffer_dynamic buffer(nof_tx_ports, 20 * base_symbol_size);

  unsigned nof_symbols_per_slot   = get_nsymb_per_slot(cp);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

  // Create notifiers and connect.
  downlink_processor_notifier_spy downlink_proc_notifier_spy;
  pdxch_processor_notifier_spy    pdxch_proc_notifier_spy;
  dl_processor->connect(downlink_proc_notifier_spy, pdxch_proc_notifier_spy);

  downlink_processor_baseband& dl_proc_baseband = dl_processor->get_baseband();

  // Timestamp of each slot.
  baseband_gateway_timestamp slot_timestamp = 0;

  // Test generation of metadata for different buffer timestamp offsets, with respect to the slot timestamps.
  std::vector<int> timestamp_offsets = {0, 13, -7};
  for (auto t_offset : timestamp_offsets) {
    for (unsigned i_frame = 0, i_slot_frame = 0; i_frame != nof_frames_test; ++i_frame) {
      for (unsigned i_subframe = 0; i_subframe != NOF_SUBFRAMES_PER_FRAME; ++i_subframe) {
        for (unsigned i_slot = 0, i_symbol_subframe = 0; i_slot != nof_slots_per_subframe; ++i_slot, ++i_slot_frame) {
          // Resize baseband buffer to hold an entire slot.
          unsigned slot_size = 0;
          for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol) {
            unsigned cp_size = cp.get_length(i_symbol_subframe + i_symbol, scs).to_samples(srate.to_Hz());
            slot_size += cp_size + base_symbol_size;
          }
          buffer.resize(slot_size);

          // Clear spies.
          downlink_proc_notifier_spy.clear_notifications();
          pdxch_proc_notifier_spy.clear_notifications();

          // If the symbol belongs to one of the slots to process, it is expected that the DL processor returns a
          // non-empty buffer.
          bool processed_expected =
              std::find(frame_slots_to_process.begin(),
                        frame_slots_to_process.end(),
                        i_slot_frame % (10 * get_nof_slots_per_subframe(scs))) != frame_slots_to_process.end();

          // Apply buffer timestamp alignment offset.
          baseband_gateway_timestamp buffer_timestamp =
              (static_cast<int64_t>(slot_timestamp) + t_offset >= 0) ? slot_timestamp + t_offset : slot_timestamp;

          // Process baseband.
          baseband_gateway_transmitter_metadata md = dl_proc_baseband.process(buffer.get_writer(), buffer_timestamp);

          if (processed_expected) {
            // The buffer should not be empty if the current slot is expected to be processed.
            ASSERT_FALSE(md.is_empty);

            if (buffer_timestamp < slot_timestamp) {
              // If the buffer timestamp is smaller than the slot timestamp, the Tx start metadata field should be set
              // to the buffer sample index where the processed slot starts.
              ASSERT_TRUE(md.tx_start.has_value() && (md.tx_start.value() == std::abs(t_offset)));
              ASSERT_FALSE(md.tx_end.has_value());
            } else if (buffer_timestamp > slot_timestamp) {
              // If the buffer timestamp is larger than the slot timestamp, the Tx end metadata field should be set to
              // the buffer sample index where the processed slot ends.
              ASSERT_FALSE(md.tx_start.has_value());
              ASSERT_TRUE(md.tx_end.has_value() &&
                          (md.tx_end.value() == buffer.get_nof_samples() - std::abs(t_offset)));
            } else {
              // If the buffer is aligned with the current slot, the Tx start and Tx end metadata fields should not be
              // set.
              ASSERT_FALSE(md.tx_start.has_value());
              ASSERT_FALSE(md.tx_end.has_value());
            }

          } else {
            if (buffer_timestamp == slot_timestamp) {
              // If the slot is not expected to be processed, the buffer should be marked as empty.
              ASSERT_TRUE(md.is_empty);
              ASSERT_FALSE(md.tx_start.has_value());
              ASSERT_FALSE(md.tx_end.has_value());
            }
          }

          // Increment timestamp.
          slot_timestamp += buffer.get_nof_samples();
        }
      }
    }
  }
}
