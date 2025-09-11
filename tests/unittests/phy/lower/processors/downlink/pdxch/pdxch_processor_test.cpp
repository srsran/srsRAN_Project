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

#include "../../../../support/resource_grid_test_doubles.h"
#include "../../../amplitude_control/amplitude_controller_test_doubles.h"
#include "../../../modulation/ofdm_modulator_test_doubles.h"
#include "pdxch_processor_notifier_test_doubles.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_factories.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace srsran {

bool operator==(const lower_phy_baseband_metrics& left, const lower_phy_baseband_metrics& right)
{
  if (left.avg_power != right.avg_power) {
    return false;
  }
  if (left.peak_power != right.peak_power) {
    return false;
  }
  if (left.clipping != right.clipping) {
    return false;
  }
  return true;
}

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

std::ostream& operator<<(std::ostream& os, const pdxch_processor_baseband::slot_context& context)
{
  fmt::print(os, "{} {}", context.slot, context.sector);
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
             config.center_freq_Hz);
  return os;
}

bool operator==(const pdxch_processor_baseband::slot_context& left, const pdxch_processor_baseband::slot_context& right)
{
  return (left.slot == right.slot) && (left.sector == right.sector);
}

bool operator==(const ofdm_modulator_configuration& left, const ofdm_modulator_configuration& right)
{
  return (left.numerology == right.numerology) && (left.bw_rb == right.bw_rb) && (left.dft_size == right.dft_size) &&
         (left.cp == right.cp) && (left.scale == right.scale) && (left.center_freq_Hz == right.center_freq_Hz);
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
  static void SetUpTestSuite()
  {
    srslog::init();

    if (pdxch_proc_factory == nullptr) {
      ofdm_mod_factory_spy = std::make_shared<ofdm_modulator_factory_spy>();
      ASSERT_NE(ofdm_mod_factory_spy, nullptr);

      amplitude_control_factory = std::make_shared<amplitude_controller_factory_spy>();
      ASSERT_NE(amplitude_control_factory, nullptr);

      pdxch_proc_factory = create_pdxch_processor_factory_sw(ofdm_mod_factory_spy, amplitude_control_factory);
      ASSERT_NE(pdxch_proc_factory, nullptr);
    }
  }

  LowerPhyDownlinkProcessorFixture() :
    ::testing::TestWithParam<LowerPhyDownlinkProcessorParams>(),
    rg_spy(rg_reader_spy, rg_writer_spy),
    shared_rg_spy(rg_spy),
    modulation_executor(MAX_NSYMB_PER_SLOT * MAX_PORTS)
  {
  }

  void SetUp() override
  {
    ASSERT_NE(pdxch_proc_factory, nullptr);

    // Select parameters.
    nof_tx_ports = std::get<0>(GetParam());
    srate        = std::get<1>(GetParam());
    scs          = std::get<2>(GetParam());
    cp           = std::get<3>(GetParam());

    nof_symbols_per_slot   = get_nsymb_per_slot(cp);
    nof_slots_per_subframe = get_nof_slots_per_subframe(scs);
    nof_slots_per_frame    = nof_slots_per_subframe * NOF_SUBFRAMES_PER_FRAME;

    bandwidth_rb   = dist_bandwidth_prb(rgen);
    center_freq_Hz = dist_center_freq_Hz(rgen);

    // Prepare configurations.
    pdxch_processor_configuration config = {.cp                  = cp,
                                            .scs                 = scs,
                                            .srate               = srate,
                                            .bandwidth_rb        = bandwidth_rb,
                                            .center_freq_Hz      = center_freq_Hz,
                                            .nof_tx_ports        = nof_tx_ports,
                                            .modulation_executor = modulation_executor};

    // Create processor.
    pdxch_proc = pdxch_proc_factory->create(config);
    ASSERT_NE(pdxch_proc, nullptr);

    // Select OFDM modulator processor spy.
    ofdm_mod_spy = ofdm_mod_factory_spy->get_modulators().back();
  }

  shared_resource_grid get_shared_grid()
  {
    rg_reader_spy.reset();

    // Add a single resource grid entry per port. This makes the grid non-empty on all ports.
    for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
      resource_grid_reader_spy::expected_entry_t entry;
      entry.port       = i_port;
      entry.symbol     = 0;
      entry.subcarrier = 0;
      entry.value      = cf_t(0.0F, 0.0F);
      rg_reader_spy.write(entry);
    }

    return shared_rg_spy.get_grid();
  }

  static constexpr unsigned                                nof_frames_test    = 3;
  static constexpr unsigned                                initial_slot_index = 0;
  static std::mt19937                                      rgen;
  static std::uniform_int_distribution<unsigned>           dist_sector_id;
  static std::uniform_int_distribution<unsigned>           dist_bandwidth_prb;
  static std::uniform_real_distribution<double>            dist_center_freq_Hz;
  static std::uniform_real_distribution<float>             dist_sample;
  static std::shared_ptr<ofdm_modulator_factory_spy>       ofdm_mod_factory_spy;
  static std::shared_ptr<amplitude_controller_factory_spy> amplitude_control_factory;
  static std::shared_ptr<pdxch_processor_factory>          pdxch_proc_factory;

  resource_grid_reader_spy rg_reader_spy;
  resource_grid_writer_spy rg_writer_spy;
  resource_grid_spy        rg_spy;
  shared_resource_grid_spy shared_rg_spy;

  cyclic_prefix      cp;
  subcarrier_spacing scs;
  sampling_rate      srate;
  unsigned           bandwidth_rb;
  double             center_freq_Hz;
  unsigned           nof_tx_ports;
  unsigned           nof_symbols_per_slot;
  unsigned           nof_slots_per_subframe;
  unsigned           nof_slots_per_frame;

  std::unique_ptr<pdxch_processor> pdxch_proc   = nullptr;
  ofdm_symbol_modulator_spy*       ofdm_mod_spy = nullptr;
  manual_task_worker               modulation_executor;
};

std::mt19937                                      LowerPhyDownlinkProcessorFixture::rgen(0);
std::uniform_int_distribution<unsigned>           LowerPhyDownlinkProcessorFixture::dist_sector_id(0, 16);
std::uniform_int_distribution<unsigned>           LowerPhyDownlinkProcessorFixture::dist_bandwidth_prb(1, MAX_RB);
std::uniform_real_distribution<double>            LowerPhyDownlinkProcessorFixture::dist_center_freq_Hz(1e8, 6e9);
std::uniform_real_distribution<float>             LowerPhyDownlinkProcessorFixture::dist_sample(-1, 1);
std::shared_ptr<ofdm_modulator_factory_spy>       LowerPhyDownlinkProcessorFixture::ofdm_mod_factory_spy      = nullptr;
std::shared_ptr<amplitude_controller_factory_spy> LowerPhyDownlinkProcessorFixture::amplitude_control_factory = nullptr;
std::shared_ptr<pdxch_processor_factory>          LowerPhyDownlinkProcessorFixture::pdxch_proc_factory        = nullptr;

} // namespace

TEST_P(LowerPhyDownlinkProcessorFixture, ModulatorConfiguration)
{
  ofdm_modulator_configuration expected_mod_config = {.numerology     = to_numerology_value(scs),
                                                      .bw_rb          = bandwidth_rb,
                                                      .dft_size       = srate.get_dft_size(scs),
                                                      .cp             = cp,
                                                      .scale          = 1.0F,
                                                      .center_freq_Hz = center_freq_Hz};

  ASSERT_EQ(ofdm_mod_spy->get_configuration(), expected_mod_config);
}

TEST_P(LowerPhyDownlinkProcessorFixture, FlowNoRequest)
{
  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  for (slot_point slot_begin{to_numerology_value(scs), initial_slot_index},
       slot(slot_begin),
       slot_end = slot_begin + NOF_SUBFRAMES_PER_FRAME * nof_slots_per_subframe * nof_frames_test;
       slot != slot_end;
       ++slot) {
    // Clear spies.
    pdxch_proc_notifier_spy.clear_notifications();
    ofdm_mod_spy->clear_modulate_entries();

    // Prepare expected PDxCH baseband entry context.
    pdxch_processor_baseband::slot_context context = {.slot = slot, .sector = dist_sector_id(rgen)};

    // Process baseband.
    auto slot_result = pdxch_proc->get_baseband().process_slot(context);

    // Verify result.
    ASSERT_EQ(slot_result.metrics, lower_phy_baseband_metrics{});
    ASSERT_EQ(slot_result.buffer, nullptr);

    // Assert OFDM modulator is not called.
    ASSERT_TRUE(ofdm_mod_spy->get_modulate_entries().empty());

    // Assert notification.
    ASSERT_EQ(pdxch_proc_notifier_spy.get_nof_notifications(), 0);
  }
}

TEST_P(LowerPhyDownlinkProcessorFixture, FlowFloodRequest)
{
  unsigned sector_id = dist_sector_id(rgen);

  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  // Gets a grid.
  shared_resource_grid shared_rg = get_shared_grid();

  // Process a few slots.
  for (slot_point slot_begin{to_numerology_value(scs), initial_slot_index},
       slot(slot_begin),
       slot_end = slot_begin + NOF_SUBFRAMES_PER_FRAME * nof_slots_per_subframe * nof_frames_test;
       slot != slot_end;
       ++slot) {
    // Clear spies.
    pdxch_proc_notifier_spy.clear_notifications();
    ofdm_mod_spy->clear_modulate_entries();

    // Prepare expected PDxCH baseband entry context.
    resource_grid_context                  req_context  = {.slot = slot, .sector = sector_id};
    pdxch_processor_baseband::slot_context proc_context = {.slot = slot, .sector = sector_id};

    // Request resource grid modulation for the current slot.
    pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), req_context);

    // Modulate.
    unsigned count = 0;
    for (; modulation_executor.try_run_next(); ++count) {
    }
    ASSERT_EQ(count, nof_symbols_per_slot * nof_tx_ports);

    // Assert OFDM modulator call.
    const auto& ofdm_mod_entries = ofdm_mod_spy->get_modulate_entries();
    ASSERT_EQ(ofdm_mod_entries.size(), nof_symbols_per_slot * nof_tx_ports);
    for (unsigned i_symbol = 0, i_symbol_subframe = nof_symbols_per_slot * slot.subframe_slot_index();
         i_symbol != nof_symbols_per_slot;
         ++i_symbol, ++i_symbol_subframe) {
      for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
        const auto& ofdm_mod_entry = ofdm_mod_entries[i_symbol * nof_tx_ports + i_port];
        ASSERT_EQ(static_cast<const void*>(ofdm_mod_entry.grid), static_cast<const void*>(&rg_reader_spy));
        ASSERT_EQ(ofdm_mod_entry.port_index, i_port);
        ASSERT_EQ(ofdm_mod_entry.symbol_index, i_symbol_subframe);
      }
    }

    // Process baseband.
    auto result = pdxch_proc->get_baseband().process_slot(proc_context);

    // Assert notification.
    ASSERT_EQ(pdxch_proc_notifier_spy.get_request_late().size(), 0);
  }
}

TEST_P(LowerPhyDownlinkProcessorFixture, LateRequest)
{
  unsigned sector_id        = dist_sector_id(rgen);
  unsigned modulation_count = 0;

  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  // Gets a grid.
  shared_resource_grid shared_rg = get_shared_grid();

  // Initial request.
  resource_grid_context initial_rg_context;
  initial_rg_context.slot   = slot_point(to_numerology_value(scs), initial_slot_index);
  initial_rg_context.sector = sector_id;
  pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), initial_rg_context);
  for (; modulation_executor.try_run_next(); ++modulation_count) {
  }

  // Late request.
  resource_grid_context late_rg_context;
  late_rg_context.slot   = initial_rg_context.slot - 1;
  late_rg_context.sector = sector_id;
  pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), late_rg_context);
  for (; modulation_executor.try_run_next(); ++modulation_count) {
  }

  // Next request.
  resource_grid_context next_rg_context;
  next_rg_context.slot   = initial_rg_context.slot + 1;
  next_rg_context.sector = sector_id;
  pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), next_rg_context);
  for (; modulation_executor.try_run_next(); ++modulation_count) {
  }

  // Verify the number of modulation tasks matches with the requests.
  ASSERT_EQ(modulation_count, 3 * nof_symbols_per_slot * nof_tx_ports);

  // Clear spies.
  pdxch_proc_notifier_spy.clear_notifications();
  ofdm_mod_spy->clear_modulate_entries();

  // Process a few slots.
  for (slot_point slot_begin{to_numerology_value(scs), initial_slot_index},
       slot(slot_begin),
       slot_end = slot_begin + NOF_SUBFRAMES_PER_FRAME * nof_slots_per_subframe * nof_frames_test;
       slot != slot_end;
       ++slot) {
    // Prepare expected PDxCH baseband entry context.
    pdxch_processor_baseband::slot_context proc_context = {.slot = slot, .sector = sector_id};

    // Process baseband.
    pdxch_processor_baseband::slot_result slot_result = pdxch_proc->get_baseband().process_slot(proc_context);

    // Assert results. Only two of the three request must have been processed.
    if ((slot == initial_rg_context.slot) || (slot == next_rg_context.slot)) {
      ASSERT_NE(slot_result.buffer, nullptr);
    } else {
      ASSERT_EQ(slot_result.buffer, nullptr);
    }
  }

  // Assert notifications. Only one late must have been detected.
  const auto& lates = pdxch_proc_notifier_spy.get_request_late();
  ASSERT_EQ(lates.size(), 1);
  ASSERT_EQ(lates.front().slot, late_rg_context.slot);
  ASSERT_EQ(lates.front().sector, late_rg_context.sector);
}

TEST_P(LowerPhyDownlinkProcessorFixture, OverflowWithRequest)
{
  // Maximum number of requests queued in the processor.
  static constexpr unsigned max_nof_concurrent_requests = 16;

  unsigned sector_id = dist_sector_id(rgen);

  // Create notifiers and connect.
  pdxch_processor_notifier_spy pdxch_proc_notifier_spy;
  pdxch_proc->connect(pdxch_proc_notifier_spy);

  shared_resource_grid shared_rg = get_shared_grid();
  ofdm_mod_spy->clear_modulate_entries();

  // Generate requests.
  for (unsigned i_request = 0; i_request != max_nof_concurrent_requests + 1; ++i_request) {
    // Request grid.
    resource_grid_context rg_context = {.slot   = {to_numerology_value(scs), initial_slot_index + i_request},
                                        .sector = sector_id};
    if (i_request < max_nof_concurrent_requests) {
      pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), rg_context);
    } else {
      pdxch_proc->get_request_handler().handle_request(shared_rg.copy(), rg_context);
    }

    // Verify the number of modulation tasks matches the number of symbols in the slot.
    unsigned count = 0;
    for (; modulation_executor.try_run_next(); ++count) {
    }
    ASSERT_EQ(count, nof_symbols_per_slot * nof_tx_ports) << "i_request=" << i_request;

    // Verify the current number of lates.
    unsigned nof_expected_lates =
        (i_request >= max_nof_concurrent_requests) ? (i_request + 1 - max_nof_concurrent_requests) : 0;
    ASSERT_EQ(pdxch_proc_notifier_spy.get_request_late().size(), nof_expected_lates);
  }

  // Assert OFDM modulator call only for the request enqueued.
  const auto& ofdm_mod_entries = ofdm_mod_spy->get_modulate_entries();
  ASSERT_EQ(ofdm_mod_entries.size(), (max_nof_concurrent_requests + 1) * nof_symbols_per_slot * nof_tx_ports);
  for (unsigned i_slot = 0, i_symbol_subframe = 0, entry_index = 0; i_slot != nof_slots_per_subframe; ++i_slot) {
    for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol, ++i_symbol_subframe) {
      for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port, ++entry_index) {
        const auto& ofdm_mod_entry = ofdm_mod_entries[entry_index];
        ASSERT_EQ(static_cast<const void*>(ofdm_mod_entry.grid), static_cast<const void*>(&rg_reader_spy));
        ASSERT_EQ(ofdm_mod_entry.port_index, i_port);
        ASSERT_EQ(ofdm_mod_entry.symbol_index, i_symbol_subframe);
      }
    }
  }

  // Process a few slots.
  for (slot_point slot_begin{to_numerology_value(scs), initial_slot_index + 1},
       slot(slot_begin),
       slot_end = slot_begin + NOF_SUBFRAMES_PER_FRAME * nof_slots_per_subframe * nof_frames_test;
       slot != slot_end;
       ++slot) {
    // Clear spies.
    pdxch_proc_notifier_spy.clear_notifications();
    ofdm_mod_spy->clear_modulate_entries();

    // Prepare expected PDxCH baseband entry context.
    pdxch_processor_baseband::slot_context proc_context = {.slot = slot, .sector = sector_id};

    // Process baseband.
    pdxch_processor_baseband::slot_result slot_result = pdxch_proc->get_baseband().process_slot(proc_context);

    // Process baseband.
    if (static_cast<unsigned>(slot - slot_begin) < max_nof_concurrent_requests) {
      ASSERT_NE(slot_result.buffer, nullptr);
    } else {
      ASSERT_EQ(slot_result.buffer, nullptr);
    }

    // Assert notifications.
    ASSERT_EQ(pdxch_proc_notifier_spy.get_request_late().size(), 0);
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
