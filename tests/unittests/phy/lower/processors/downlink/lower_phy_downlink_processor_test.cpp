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

#include "../../amplitude_control/amplitude_controller_test_doubles.h"
#include "downlink_processor_notifier_test_doubles.h"
#include "pdxch/pdxch_processor_notifier_test_doubles.h"
#include "pdxch/pdxch_processor_test_doubles.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
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

using LowerPhyDownlinkProcessorParams = std::tuple<unsigned, sampling_rate, subcarrier_spacing, cyclic_prefix>;

namespace {

class LowerPhyDownlinkProcessorFixture : public ::testing::TestWithParam<LowerPhyDownlinkProcessorParams>
{
protected:
  static void SetUpTestSuite()
  {
    if (ul_proc_factory == nullptr) {
      pdxch_proc_factory = std::make_shared<pdxch_processor_factory_spy>();
      ASSERT_NE(pdxch_proc_factory, nullptr);

      amplitude_control_factory = std::make_shared<amplitude_controller_factory_spy>();
      ASSERT_NE(amplitude_control_factory, nullptr);

      ul_proc_factory = create_downlink_processor_factory_sw(pdxch_proc_factory, amplitude_control_factory);
      ASSERT_NE(ul_proc_factory, nullptr);
    }
  }

  void SetUp() override
  {
    ASSERT_NE(ul_proc_factory, nullptr);

    // Select parameters.
    unsigned           nof_tx_ports = std::get<0>(GetParam());
    sampling_rate      srate        = std::get<1>(GetParam());
    subcarrier_spacing scs          = std::get<2>(GetParam());
    cyclic_prefix      cp           = std::get<3>(GetParam());

    // Prepare configurations.
    config.sector_id           = 0;
    config.scs                 = scs;
    config.cp                  = cp;
    config.rate                = srate;
    config.bandwidth_prb       = dist_bandwidth_prb(rgen);
    config.center_frequency_Hz = dist_center_freq_Hz(rgen);
    config.nof_tx_ports        = nof_tx_ports;

    // Create processor.
    ul_processor = ul_proc_factory->create(config);
    ASSERT_NE(ul_processor, nullptr);

    // Select PDxCH processor spy.
    pdxch_proc_spy = &pdxch_proc_factory->get_spy();

    // Select amplitude controller spy.
    amplitude_control_spy = amplitude_control_factory->get_entries().back();
  }

  static constexpr unsigned                                    nof_frames_test = 10;
  static std::mt19937                                          rgen;
  static std::uniform_int_distribution<unsigned>               dist_bandwidth_prb;
  static std::uniform_real_distribution<double>                dist_center_freq_Hz;
  static std::shared_ptr<pdxch_processor_factory_spy>          pdxch_proc_factory;
  static std::shared_ptr<amplitude_controller_factory_spy>     amplitude_control_factory;
  static std::shared_ptr<lower_phy_downlink_processor_factory> ul_proc_factory;

  downlink_processor_configuration              config;
  std::unique_ptr<lower_phy_downlink_processor> ul_processor          = nullptr;
  pdxch_processor_spy*                          pdxch_proc_spy        = nullptr;
  amplitude_controller_spy*                     amplitude_control_spy = nullptr;
};

std::mt19937                                      LowerPhyDownlinkProcessorFixture::rgen(0);
std::uniform_int_distribution<unsigned>           LowerPhyDownlinkProcessorFixture::dist_bandwidth_prb(1, MAX_RB);
std::uniform_real_distribution<double>            LowerPhyDownlinkProcessorFixture::dist_center_freq_Hz(1e8, 6e9);
std::shared_ptr<pdxch_processor_factory_spy>      LowerPhyDownlinkProcessorFixture::pdxch_proc_factory        = nullptr;
std::shared_ptr<amplitude_controller_factory_spy> LowerPhyDownlinkProcessorFixture::amplitude_control_factory = nullptr;
std::shared_ptr<lower_phy_downlink_processor_factory> LowerPhyDownlinkProcessorFixture::ul_proc_factory       = nullptr;

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
  ul_processor->connect(downlink_proc_notifier_spy, pdxch_proc_notifier_spy);

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
  const pdxch_processor_request_handler* request_handler = &ul_processor->get_downlink_request_handler();

  // Assert request handler.
  ASSERT_EQ(reinterpret_cast<const void*>(expected_request_handler), reinterpret_cast<const void*>(request_handler));
}

TEST_P(LowerPhyDownlinkProcessorFixture, Flow)
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
  downlink_processor_notifier_spy downlink_proc_notifier_spy;
  pdxch_processor_notifier_spy    pdxch_proc_notifier_spy;
  ul_processor->connect(downlink_proc_notifier_spy, pdxch_proc_notifier_spy);

  downlink_processor_baseband& dl_proc_baseband = ul_processor->get_baseband();

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
          dl_proc_baseband.process(buffer.get_writer(), timestamp);

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

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(LowerPhyDownlinkProcessor,
                         LowerPhyDownlinkProcessorFixture,
                         ::testing::Combine(::testing::Values(1, 2),
                                            ::testing::Values(sampling_rate::from_MHz(7.68)),
                                            ::testing::Values(subcarrier_spacing::kHz15,
                                                              subcarrier_spacing::kHz30,
                                                              subcarrier_spacing::kHz60),
                                            ::testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED)));
