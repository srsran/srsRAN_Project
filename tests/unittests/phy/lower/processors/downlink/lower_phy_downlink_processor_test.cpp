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

#include "downlink_processor_notifier_test_doubles.h"
#include "pdxch/pdxch_processor_notifier_test_doubles.h"
#include "pdxch/pdxch_processor_test_doubles.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader_view.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_factories.h"
#include "srsran/support/executors/manual_task_worker.h"
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

std::ostream& operator<<(std::ostream& os, const pdxch_processor_baseband::slot_context& context)
{
  fmt::print(os, "{} {}", context.slot, context.sector);
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

std::ostream& operator<<(std::ostream& os, cyclic_prefix cp)
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

bool operator==(const pdxch_processor_baseband::slot_context left, const pdxch_processor_baseband::slot_context right)
{
  return (left.slot == right.slot) && (left.sector == right.sector);
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

  if (left.get_nof_samples() != right.get_nof_samples()) {
    return false;
  }

  unsigned nof_channels = left.get_nof_channels();

  bool same_data = true;
  for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
    span<const cf_t> left_channel  = left.get_channel_buffer(i_channel);
    span<const cf_t> right_channel = right.get_channel_buffer(i_channel);
    same_data &= (left_channel.data() == right_channel.data());
  }

  if (same_data) {
    return true;
  }

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
  LowerPhyDownlinkProcessorFixture() :
    ::testing::TestWithParam<LowerPhyDownlinkProcessorParams>(), modulation_executor(MAX_NSYMB_PER_SLOT * MAX_PORTS)
  {
  }

  static void SetUpTestSuite()
  {
    if (dl_proc_factory == nullptr) {
      pdxch_proc_factory = std::make_shared<pdxch_processor_factory_spy>();
      ASSERT_NE(pdxch_proc_factory, nullptr);

      dl_proc_factory = create_downlink_processor_factory_sw(pdxch_proc_factory);
      ASSERT_NE(dl_proc_factory, nullptr);
    }
  }

  void SetUp() override
  {
    ASSERT_NE(dl_proc_factory, nullptr);

    // Select parameters.
    nof_tx_ports = std::get<0>(GetParam());
    srate        = std::get<1>(GetParam());
    scs          = std::get<0>(std::get<2>(GetParam()));
    cp           = std::get<1>(std::get<2>(GetParam()));

    nof_symbols_per_slot   = get_nsymb_per_slot(cp);
    nof_slots_per_subframe = get_nof_slots_per_subframe(scs);
    nof_slots_per_frame    = nof_slots_per_subframe * NOF_SUBFRAMES_PER_FRAME;

    bandwidth_rb   = dist_bandwidth_prb(rgen);
    center_freq_Hz = dist_center_freq_Hz(rgen);

    // Prepare configurations.
    downlink_processor_configuration config = {.sector_id           = sector_id,
                                               .scs                 = scs,
                                               .cp                  = cp,
                                               .rate                = srate,
                                               .bandwidth_prb       = bandwidth_rb,
                                               .center_frequency_Hz = center_freq_Hz,
                                               .nof_tx_ports        = nof_tx_ports,
                                               .modulation_executor = modulation_executor};

    // Create processor.
    dl_processor = dl_proc_factory->create(config);
    ASSERT_NE(dl_processor, nullptr);

    // Select PDxCH processor spy.
    pdxch_proc_spy = &pdxch_proc_factory->get_spy();
  }

  static constexpr unsigned                                    nof_frames_test    = 3;
  static constexpr unsigned                                    initial_slot_index = 0;
  static constexpr unsigned                                    sector_id          = 0;
  static std::mt19937                                          rgen;
  static std::uniform_int_distribution<unsigned>               dist_bandwidth_prb;
  static std::uniform_real_distribution<double>                dist_center_freq_Hz;
  static std::shared_ptr<pdxch_processor_factory_spy>          pdxch_proc_factory;
  static std::shared_ptr<lower_phy_downlink_processor_factory> dl_proc_factory;

  cyclic_prefix      cp;
  subcarrier_spacing scs;
  sampling_rate      srate;
  unsigned           bandwidth_rb;
  double             center_freq_Hz;
  unsigned           nof_tx_ports;
  unsigned           nof_symbols_per_slot;
  unsigned           nof_slots_per_subframe;
  unsigned           nof_slots_per_frame;

  std::unique_ptr<lower_phy_downlink_processor> dl_processor   = nullptr;
  pdxch_processor_spy*                          pdxch_proc_spy = nullptr;
  manual_task_worker                            modulation_executor;
};

std::mt19937                                          LowerPhyDownlinkProcessorFixture::rgen(0);
std::uniform_int_distribution<unsigned>               LowerPhyDownlinkProcessorFixture::dist_bandwidth_prb(1, MAX_RB);
std::uniform_real_distribution<double>                LowerPhyDownlinkProcessorFixture::dist_center_freq_Hz(1e8, 6e9);
std::shared_ptr<pdxch_processor_factory_spy>          LowerPhyDownlinkProcessorFixture::pdxch_proc_factory = nullptr;
std::shared_ptr<lower_phy_downlink_processor_factory> LowerPhyDownlinkProcessorFixture::dl_proc_factory    = nullptr;

// Fixture class used to test the transmit buffer metadata generated by the lower phy downlink processor.
class LowerPhyDownlinkProcessorMetadataFixture : public ::testing::Test
{
protected:
  LowerPhyDownlinkProcessorMetadataFixture() : ::testing::Test(), modulation_executor(MAX_NSYMB_PER_SLOT * MAX_PORTS) {}

  void SetUp() override
  {
    // Create a PDxCH processor spy that only processes symbols belonging to some slots.
    std::shared_ptr<pdxch_processor_factory_spy> pdxch_proc_factory = std::make_shared<pdxch_processor_factory_spy>();
    ASSERT_NE(pdxch_proc_factory, nullptr);

    std::shared_ptr<lower_phy_downlink_processor_factory> dl_proc_factory =
        create_downlink_processor_factory_sw(pdxch_proc_factory);
    ASSERT_NE(dl_proc_factory, nullptr);

    // Set test parameters.
    scs          = subcarrier_spacing::kHz30;
    cp           = cyclic_prefix::NORMAL;
    srate        = sampling_rate::from_MHz(7.68);
    nof_tx_ports = 1;

    // Prepare configuration.
    downlink_processor_configuration config = {.sector_id           = 0,
                                               .scs                 = scs,
                                               .cp                  = cp,
                                               .rate                = srate,
                                               .bandwidth_prb       = MAX_RB,
                                               .center_frequency_Hz = 3.5e6,
                                               .nof_tx_ports        = nof_tx_ports,
                                               .modulation_executor = modulation_executor};

    // Create processor.
    dl_processor = dl_proc_factory->create(config);
    ASSERT_NE(dl_processor, nullptr);
  }

  std::unique_ptr<lower_phy_downlink_processor> dl_processor = nullptr;
  manual_task_worker                            modulation_executor;

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
  pdxch_processor_configuration expected_pdxch_config = {.cp                  = cp,
                                                         .scs                 = scs,
                                                         .srate               = srate,
                                                         .bandwidth_rb        = bandwidth_rb,
                                                         .center_freq_Hz      = center_freq_Hz,
                                                         .nof_tx_ports        = nof_tx_ports,
                                                         .modulation_executor = modulation_executor};
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

TEST_P(LowerPhyDownlinkProcessorFixture, UnalignedFlow)
{
  // Create notifiers and connect.
  downlink_processor_notifier_spy downlink_proc_notifier_spy;
  pdxch_processor_notifier_spy    pdxch_proc_notifier_spy;
  dl_processor->connect(downlink_proc_notifier_spy, pdxch_proc_notifier_spy);

  downlink_processor_baseband& dl_proc_baseband = dl_processor->get_baseband();

  // Initial timestamp, aligned to the slot 0.
  baseband_gateway_timestamp timestamp = 1;

  // Process a few slots.
  for (slot_point slot_begin{to_numerology_value(scs), initial_slot_index},
       slot(slot_begin),
       slot_end = slot_begin + NOF_SUBFRAMES_PER_FRAME * nof_slots_per_subframe * nof_frames_test;
       slot != slot_end;
       ++slot) {
    // Clear spies.
    downlink_proc_notifier_spy.clear_notifications();
    pdxch_proc_notifier_spy.clear_notifications();
    pdxch_proc_spy->clear();

    // Process baseband.
    downlink_processor_baseband::processing_result result = dl_proc_baseband.process(timestamp);

    // Buffer must be valid and the timestamp must match.
    ASSERT_TRUE(result.buffer);
    ASSERT_EQ(result.metadata.ts, timestamp);

    // The first slot must be for alignment.
    if (slot == slot_begin) {
      // Assert processing results. The buffer must be valid and the metadata as it contains a full slot.
      ASSERT_TRUE(result.metadata.is_empty);
      ASSERT_FALSE(result.metadata.tx_start.has_value());
      ASSERT_FALSE(result.metadata.tx_end.has_value());

      // Assert no PDxCH processor call.
      auto& pdxch_proc_entries = pdxch_proc_spy->get_baseband_entries();
      ASSERT_EQ(pdxch_proc_entries.size(), 0);

      // Assert no TTI boundary notification.
      const auto& tti_boundary_entries = downlink_proc_notifier_spy.get_tti_boundaries();
      ASSERT_EQ(tti_boundary_entries.size(), 0);

      // Increment slot to the next subframe.
      slot += nof_slots_per_subframe - 1;
    } else {
      // Assert processing results. The buffer must be valid and the metadata as it contains a full slot.
      ASSERT_FALSE(result.metadata.is_empty);
      ASSERT_FALSE(result.metadata.tx_start.has_value());
      ASSERT_FALSE(result.metadata.tx_end.has_value());

      // Prepare expected PDxCH baseband entry context.
      pdxch_processor_baseband::slot_context slot_context = {.slot = slot, .sector = sector_id};

      // Assert PDxCH processor call.
      auto& pdxch_proc_entries = pdxch_proc_spy->get_baseband_entries();
      ASSERT_EQ(pdxch_proc_entries.size(), 1);
      auto& pdxch_proc_entry = pdxch_proc_entries.back();
      ASSERT_EQ(pdxch_proc_entry.context, slot_context);
      ASSERT_EQ(pdxch_proc_entry.samples, result.buffer->get_reader());

      // Assert TTI boundary notification.
      const auto& tti_boundary_entries = downlink_proc_notifier_spy.get_tti_boundaries();
      ASSERT_EQ(tti_boundary_entries.size(), 1);
      ASSERT_EQ(tti_boundary_entries.front().slot, slot);
    }

    // No PDxCH notifications.
    ASSERT_EQ(pdxch_proc_notifier_spy.get_nof_notifications(), 0);

    // Increment timestamp.
    timestamp += result.buffer->get_nof_samples();

    // Return buffer to the pool.
    result.buffer = nullptr;
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
