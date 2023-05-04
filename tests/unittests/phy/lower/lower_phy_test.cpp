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

#include "../../gateways/baseband/baseband_gateway_test_doubles.h"
#include "../../support/task_executor_test_doubles.h"
#include "../support/prach_buffer_test_doubles.h"
#include "../support/resource_grid_test_doubles.h"
#include "lower_phy_test_doubles.h"
#include "processors/downlink/downlink_processor_test_doubles.h"
#include "processors/uplink/uplink_processor_notifier_test_doubles.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/phy/lower/lower_phy_factory.h"
#include "srsran/phy/lower/lower_phy_request_handler.h"
#include "srsran/phy/lower/lower_phy_rg_handler.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_notifier.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_notifier.h"
#include "srsran/srsvec/compare.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace srsran {

auto to_tuple(const downlink_processor_configuration& config)
{
  return std::tie(config.sector_id,
                  config.scs,
                  config.cp,
                  config.rate,
                  config.bandwidth_prb,
                  config.center_frequency_Hz,
                  config.nof_tx_ports,
                  config.initial_slot_index,
                  config.nof_slot_tti_in_advance);
}

auto to_tuple(const uplink_processor_configuration& config)
{
  return std::tie(config.sector_id,
                  config.scs,
                  config.cp,
                  config.rate,
                  config.bandwidth_prb,
                  config.center_frequency_Hz,
                  config.nof_rx_ports,
                  config.initial_slot_index);
}

auto to_tuple(const resource_grid_context& context)
{
  return std::tie(context.sector, context.slot);
}

auto to_tuple(const lower_phy_timing_context& context)
{
  return std::tie(context.slot);
}

auto to_tuple(const prach_buffer_context& context)
{
  return std::tie(context.sector,
                  context.port,
                  context.slot,
                  context.start_symbol,
                  context.format,
                  context.rb_offset,
                  context.nof_td_occasions,
                  context.nof_fd_occasions,
                  context.nof_prb_ul_grid,
                  context.pusch_scs,
                  context.root_sequence_index,
                  context.restricted_set,
                  context.zero_correlation_zone,
                  context.start_preamble_index,
                  context.nof_preamble_indices);
}

auto to_tuple(const lower_phy_rx_symbol_context& context)
{
  return std::tie(context.sector, context.slot, context.nof_symbols);
}

bool operator==(const downlink_processor_configuration& left, const downlink_processor_configuration& right)
{
  return to_tuple(left) == to_tuple(right);
}

bool operator==(const uplink_processor_configuration& left, const uplink_processor_configuration& right)
{
  return to_tuple(left) == to_tuple(right);
}

bool operator==(const resource_grid_context& left, const resource_grid_context& right)
{
  return to_tuple(left) == to_tuple(right);
}

bool operator==(const lower_phy_timing_context& left, const lower_phy_timing_context& right)
{
  return to_tuple(left) == to_tuple(right);
}

bool operator==(const prach_buffer_context& left, const prach_buffer_context& right)
{
  return to_tuple(left) == to_tuple(right);
}

bool operator==(const lower_phy_rx_symbol_context& left, const lower_phy_rx_symbol_context& right)
{
  return to_tuple(left) == to_tuple(right);
}

bool operator==(const baseband_gateway_buffer& left, const baseband_gateway_buffer& right)
{
  if (left.get_nof_channels() != right.get_nof_channels()) {
    return false;
  }

  if (left.get_nof_samples() != right.get_nof_samples()) {
    return false;
  }

  for (unsigned i_channel = 0, i_channel_end = left.get_nof_channels(); i_channel != i_channel_end; ++i_channel) {
    if (!srsvec::equal(left.get_channel_buffer(i_channel), right.get_channel_buffer(i_channel))) {
      return false;
    }
  }

  return true;
}

static std::ostream& operator<<(std::ostream& os, const subcarrier_spacing& scs)
{
  fmt::print(os, "{}", to_string(scs));
  return os;
}

static std::ostream& operator<<(std::ostream& os, const cyclic_prefix& cp)
{
  fmt::print(os, "{}", cp.to_string());
  return os;
}

static std::ostream& operator<<(std::ostream& os, const sampling_rate& srate)
{
  fmt::print(os, "{}", srate);
  return os;
}

static std::ostream& operator<<(std::ostream& os, const n_ta_offset& ta_offset)
{
  fmt::print(os, "{}Tc", static_cast<unsigned>(ta_offset));
  return os;
}

static std::ostream& operator<<(std::ostream& os, const downlink_processor_configuration& config)
{
  fmt::print(os,
             "{} {} {} {} {} {:.3f} {} {} {}",
             config.sector_id,
             to_string(config.scs),
             config.cp.to_string(),
             config.rate,
             config.bandwidth_prb,
             config.center_frequency_Hz * 1e-6,
             config.nof_tx_ports,
             config.initial_slot_index,
             config.nof_slot_tti_in_advance);
  return os;
}

static std::ostream& operator<<(std::ostream& os, const resource_grid_context& context)
{
  fmt::print(os, "{} {}", context.sector, context.slot);
  return os;
}

static std::ostream& operator<<(std::ostream& os, const lower_phy_timing_context& context)
{
  fmt::print(os, "{}", context.slot);
  return os;
}

static std::ostream& operator<<(std::ostream& os, const prach_buffer_context& context)
{
  fmt::print(os,
             "{} {} {} {} {} {} {} {} {} {} {} {} {} {} {}",
             context.sector,
             context.port,
             context.slot,
             context.start_symbol,
             context.format,
             context.rb_offset,
             context.nof_td_occasions,
             context.nof_fd_occasions,
             context.nof_prb_ul_grid,
             context.pusch_scs,
             context.root_sequence_index,
             context.restricted_set,
             context.zero_correlation_zone,
             context.start_preamble_index,
             context.nof_preamble_indices);
  return os;
}

static std::ostream& operator<<(std::ostream& os, const lower_phy_rx_symbol_context& context)
{
  fmt::print(os, "{} {} {}", context.sector, context.slot, context.nof_symbols);
  return os;
}

} // namespace srsran

namespace {

using LowerPhyParams = std::tuple<subcarrier_spacing, cyclic_prefix, sampling_rate, n_ta_offset, unsigned>;

class LowerPhyFixture : public testing::TestWithParam<LowerPhyParams>
{
public:
  LowerPhyFixture() :
    testing::TestWithParam<LowerPhyParams>(),
    tx_task_executor(1),
    rx_task_executor(1),
    dl_task_executor(1),
    ul_task_executor(1),
    prach_task_executor(1)
  {
  }

protected:
  void SetUp() override
  {
    // Select test parameters.
    const LowerPhyParams& params = GetParam();
    scs                          = std::get<0>(params);
    cp                           = std::get<1>(params);
    dft_window_offset            = dft_window_offset_dist(rgen);
    max_processing_delay_slots   = max_processing_delay_slots_dist(rgen);
    ul_to_dl_subframe_offset     = ul_to_dl_subframe_offset_dist(rgen);
    srate                        = std::get<2>(params);
    ta_offset                    = std::get<3>(params);
    time_alignment_calibration   = time_alignment_calibration_dist(rgen);
    bandwidth_rb                 = std::get<4>(params);
    dl_freq_hz                   = dl_freq_hz_dist(rgen);
    ul_freq_hz                   = ul_freq_hz_dist(rgen);
    nof_tx_ports                 = nof_tx_ports_dist(rgen);
    nof_rx_ports                 = nof_rx_ports_dist(rgen);
    tx_buffer_size               = buffer_size_dist(rgen);
    rx_buffer_size               = tx_buffer_size;

    // Select buffer sizes.
    bb_gateway_spy.set_transmitter_buffer_size(tx_buffer_size);
    bb_gateway_spy.set_receiver_buffer_size(rx_buffer_size);

    // Prepare configuration.
    lower_phy_configuration config;
    config.scs                        = scs;
    config.cp                         = cp;
    config.dft_window_offset          = dft_window_offset;
    config.max_processing_delay_slots = max_processing_delay_slots;
    config.ul_to_dl_subframe_offset   = ul_to_dl_subframe_offset;
    config.srate                      = srate;
    config.ta_offset                  = ta_offset;
    config.time_alignment_calibration = time_alignment_calibration;
    config.amplitude_config           = {};
    config.sectors.emplace_back();
    config.sectors.back().bandwidth_rb = bandwidth_rb;
    config.sectors.back().dl_freq_hz   = dl_freq_hz;
    config.sectors.back().ul_freq_hz   = ul_freq_hz;
    config.sectors.back().nof_tx_ports = nof_tx_ports;
    config.sectors.back().nof_rx_ports = nof_rx_ports;
    config.bb_gateway                  = &bb_gateway_spy;
    config.rx_symbol_notifier          = &rx_symbol_notifier_spy;
    config.timing_notifier             = &timing_notifier_spy;
    config.error_notifier              = &error_notifier_spy;
    config.tx_task_executor            = &tx_task_executor;
    config.rx_task_executor            = &rx_task_executor;
    config.dl_task_executor            = &dl_task_executor;
    config.ul_task_executor            = &ul_task_executor;
    config.prach_async_executor        = &prach_task_executor;

    // Prepare downlink processor factory.
    std::shared_ptr<lower_phy_downlink_processor_factory_spy> lphy_dl_proc_factory =
        std::make_shared<lower_phy_downlink_processor_factory_spy>();
    ASSERT_NE(lphy_dl_proc_factory, nullptr);

    // Prepare uplink processor factory.
    std::shared_ptr<lower_phy_uplink_processor_factory_spy> lphy_ul_proc_factory =
        std::make_shared<lower_phy_uplink_processor_factory_spy>();
    ASSERT_NE(lphy_ul_proc_factory, nullptr);

    // Prepare factory.
    std::shared_ptr<lower_phy_factory> lphy_factory =
        create_lower_phy_factory_sw(lphy_dl_proc_factory, lphy_ul_proc_factory);
    ASSERT_NE(lphy_factory, nullptr);

    // Create lower PHY.
    lphy = lphy_factory->create(config);
    ASSERT_NE(lphy, nullptr);

    // Select spies.
    downlink_proc_spy = lphy_dl_proc_factory->get_entries().back();
    uplink_proc_spy   = lphy_ul_proc_factory->get_entries().back();
  }

  unsigned get_rx_to_tx_delay()
  {
    // Calculate time between the UL signal reception and the transmission.
    phy_time_unit ul_to_dl_delay = phy_time_unit::from_seconds(0.001 * static_cast<double>(ul_to_dl_subframe_offset));

    // UL to DL delay in number of samples.
    unsigned ul_to_dl_delay_samples = ul_to_dl_delay.to_samples(srate.to_Hz());

    // Apply time alingment calibration.
    ul_to_dl_delay_samples -= time_alignment_calibration;

    // Apply the time alignment offset.
    phy_time_unit time_alignment_offset = phy_time_unit::from_units_of_Tc(static_cast<unsigned>(ta_offset));

    ul_to_dl_delay_samples += time_alignment_offset.to_samples(srate.to_Hz());

    return ul_to_dl_delay_samples;
  }

  subcarrier_spacing scs;
  cyclic_prefix      cp;
  float              dft_window_offset;
  unsigned           max_processing_delay_slots;
  unsigned           ul_to_dl_subframe_offset;
  sampling_rate      srate;
  n_ta_offset        ta_offset;
  int                time_alignment_calibration;
  unsigned           bandwidth_rb;
  double             dl_freq_hz;
  double             ul_freq_hz;
  unsigned           nof_tx_ports;
  unsigned           nof_rx_ports;
  unsigned           tx_buffer_size;
  unsigned           rx_buffer_size;

  baseband_gateway_spy                    bb_gateway_spy;
  lower_phy_rx_symbol_notifier_spy        rx_symbol_notifier_spy;
  lower_phy_timing_notifier_spy           timing_notifier_spy;
  lower_phy_error_notifier_spy            error_notifier_spy;
  manual_task_worker_always_enqueue_tasks tx_task_executor;
  manual_task_worker_always_enqueue_tasks rx_task_executor;
  manual_task_worker_always_enqueue_tasks dl_task_executor;
  manual_task_worker_always_enqueue_tasks ul_task_executor;
  manual_task_worker_always_enqueue_tasks prach_task_executor;
  std::unique_ptr<lower_phy>              lphy;
  lower_phy_downlink_processor_spy*       downlink_proc_spy;
  lower_phy_uplink_processor_spy*         uplink_proc_spy;

  static std::mt19937                            rgen;
  static std::uniform_real_distribution<float>   dft_window_offset_dist;
  static std::uniform_int_distribution<unsigned> max_processing_delay_slots_dist;
  static std::uniform_int_distribution<unsigned> ul_to_dl_subframe_offset_dist;
  static std::uniform_int_distribution<int>      time_alignment_calibration_dist;
  static std::uniform_real_distribution<double>  dl_freq_hz_dist;
  static std::uniform_real_distribution<double>  ul_freq_hz_dist;
  static std::uniform_int_distribution<unsigned> nof_tx_ports_dist;
  static std::uniform_int_distribution<unsigned> nof_rx_ports_dist;
  static std::uniform_int_distribution<unsigned> slot_dist;
  static std::uniform_int_distribution<unsigned> sector_id_dist;
  static std::uniform_int_distribution<unsigned> buffer_size_dist;
};

std::mt19937                            LowerPhyFixture::rgen;
std::uniform_real_distribution<float>   LowerPhyFixture::dft_window_offset_dist(0.1, 0.9);
std::uniform_int_distribution<unsigned> LowerPhyFixture::max_processing_delay_slots_dist(1, 3);
std::uniform_int_distribution<unsigned> LowerPhyFixture::ul_to_dl_subframe_offset_dist(1, 3);
std::uniform_int_distribution<int>      LowerPhyFixture::time_alignment_calibration_dist(-100, 100);
std::uniform_real_distribution<double>  LowerPhyFixture::dl_freq_hz_dist(800e6, 6000e6);
std::uniform_real_distribution<double>  LowerPhyFixture::ul_freq_hz_dist(800e6, 6000e6);
std::uniform_int_distribution<unsigned> LowerPhyFixture::nof_tx_ports_dist(1, 4);
std::uniform_int_distribution<unsigned> LowerPhyFixture::nof_rx_ports_dist(1, 4);
std::uniform_int_distribution<unsigned> LowerPhyFixture::slot_dist(0, 10240 - 1);
std::uniform_int_distribution<unsigned> LowerPhyFixture::sector_id_dist(0, 1024);
std::uniform_int_distribution<unsigned> LowerPhyFixture::buffer_size_dist(64, 128);

TEST_P(LowerPhyFixture, Factory)
{
  // Check that the downlink processor configuration is correct.
  downlink_processor_configuration dl_proc_config;
  dl_proc_config.sector_id               = 0;
  dl_proc_config.scs                     = scs;
  dl_proc_config.cp                      = cp;
  dl_proc_config.rate                    = srate;
  dl_proc_config.bandwidth_prb           = bandwidth_rb;
  dl_proc_config.center_frequency_Hz     = dl_freq_hz;
  dl_proc_config.nof_tx_ports            = nof_tx_ports;
  dl_proc_config.initial_slot_index      = ul_to_dl_subframe_offset * get_nof_slots_per_subframe(scs);
  dl_proc_config.nof_slot_tti_in_advance = max_processing_delay_slots;
  ASSERT_EQ(dl_proc_config, downlink_proc_spy->get_config());

  // Check that the uplink processor configuration is correct.
  uplink_processor_configuration ul_proc_config;
  ul_proc_config.sector_id           = 0;
  ul_proc_config.scs                 = scs;
  ul_proc_config.cp                  = cp;
  ul_proc_config.rate                = srate;
  ul_proc_config.bandwidth_prb       = bandwidth_rb;
  ul_proc_config.center_frequency_Hz = ul_freq_hz;
  ul_proc_config.nof_rx_ports        = nof_rx_ports;
  ul_proc_config.initial_slot_index  = 0;
  ASSERT_EQ(ul_proc_config, uplink_proc_spy->get_config());
}

TEST_P(LowerPhyFixture, TimingNotifiers)
{
  // Get downlink processor notifier.
  downlink_processor_notifier* downlink_notifier = downlink_proc_spy->get_notifier();
  ASSERT_NE(downlink_notifier, nullptr);

  // Get uplink processor notifier.
  uplink_processor_notifier* uplink_notifier = uplink_proc_spy->get_notifier();
  ASSERT_NE(uplink_notifier, nullptr);

  // Make sure no notifications are stored.
  timing_notifier_spy.clear_all_events();
  rx_symbol_notifier_spy.clear_all_events();
  error_notifier_spy.clear_all_errors();

  // Notify TTI boundary.
  {
    lower_phy_timing_context context;
    context.slot = slot_point(to_numerology_value(scs), slot_dist(rgen));
    downlink_notifier->on_tti_boundary(context);
    auto& entries = timing_notifier_spy.get_tti_boundaries_events();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back());
  }

  // Notify half slot.
  {
    lower_phy_timing_context context;
    context.slot = slot_point(to_numerology_value(scs), slot_dist(rgen));
    uplink_notifier->on_half_slot(context);
    auto& entries = timing_notifier_spy.get_ul_half_slot_events();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back());
  }

  // Notify full slot.
  {
    lower_phy_timing_context context;
    context.slot = slot_point(to_numerology_value(scs), slot_dist(rgen));
    uplink_notifier->on_full_slot(context);
    auto& entries = timing_notifier_spy.get_ul_full_slot_events();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back());
  }

  // Assert only three timing events.
  ASSERT_EQ(timing_notifier_spy.get_nof_events(), 3);

  // No other events.
  ASSERT_EQ(rx_symbol_notifier_spy.get_nof_events(), 0);
  ASSERT_EQ(error_notifier_spy.get_nof_errors(), 0);
}

TEST_P(LowerPhyFixture, ErrorNotifiers)
{
  // Get PDxCH notifier.
  pdxch_processor_notifier* pdxch_notifier = downlink_proc_spy->get_pdxch_notifier();
  ASSERT_NE(pdxch_notifier, nullptr);

  // Get PRACH notifier.
  prach_processor_notifier* prach_notifier = uplink_proc_spy->get_prach_notifier();
  ASSERT_NE(prach_notifier, nullptr);

  // Get PUxCH notifier.
  puxch_processor_notifier* puxch_notifier = uplink_proc_spy->get_puxch_notifier();
  ASSERT_NE(puxch_notifier, nullptr);

  // Make sure no notifications are stored.
  timing_notifier_spy.clear_all_events();
  rx_symbol_notifier_spy.clear_all_events();
  error_notifier_spy.clear_all_errors();

  // Notify late RG event.
  {
    resource_grid_context context;
    context.sector = sector_id_dist(rgen);
    context.slot   = slot_point(to_numerology_value(scs), slot_dist(rgen));
    pdxch_notifier->on_late_resource_grid(context);
    auto& entries = error_notifier_spy.get_late_rg_errors();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back());
  }

  // Notify overflow RG event.
  {
    resource_grid_context context;
    context.sector = sector_id_dist(rgen);
    context.slot   = slot_point(to_numerology_value(scs), slot_dist(rgen));
    pdxch_notifier->on_overflow_resource_grid(context);
    auto& entries = error_notifier_spy.get_overflow_rg_errors();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back());
  }

  // Notify late PRACH request event.
  {
    prach_buffer_context context;
    context.sector                = sector_id_dist(rgen);
    context.port                  = 0;
    context.slot                  = slot_point(to_numerology_value(scs), slot_dist(rgen));
    context.start_symbol          = 0;
    context.format                = prach_format_type::zero;
    context.rb_offset             = 0;
    context.nof_td_occasions      = 0;
    context.nof_fd_occasions      = 0;
    context.nof_prb_ul_grid       = 0;
    context.pusch_scs             = scs;
    context.root_sequence_index   = 0;
    context.restricted_set        = restricted_set_config::UNRESTRICTED;
    context.zero_correlation_zone = 0;
    context.start_preamble_index  = 0;
    context.nof_preamble_indices  = 0;

    prach_notifier->on_prach_request_late(context);
    auto& entries = error_notifier_spy.get_prach_request_late_errors();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back());
  }

  // Notify overflow PRACH request event.
  {
    prach_buffer_context context;
    context.sector                = sector_id_dist(rgen);
    context.port                  = 0;
    context.slot                  = slot_point(to_numerology_value(scs), slot_dist(rgen));
    context.start_symbol          = 0;
    context.format                = prach_format_type::zero;
    context.rb_offset             = 0;
    context.nof_td_occasions      = 0;
    context.nof_fd_occasions      = 0;
    context.nof_prb_ul_grid       = 0;
    context.pusch_scs             = scs;
    context.root_sequence_index   = 0;
    context.restricted_set        = restricted_set_config::UNRESTRICTED;
    context.zero_correlation_zone = 0;
    context.start_preamble_index  = 0;
    context.nof_preamble_indices  = 0;

    prach_notifier->on_prach_request_overflow(context);
    auto& entries = error_notifier_spy.get_prach_request_overflow_errors();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back());
  }

  // Notify late PUxCH request event.
  {
    resource_grid_context context;
    context.sector = sector_id_dist(rgen);
    context.slot   = slot_point(to_numerology_value(scs), slot_dist(rgen));
    puxch_notifier->on_puxch_request_late(context);
    auto& entries = error_notifier_spy.get_puxch_request_late_errors();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back());
  }

  // Notify overflow PUxCH request event.
  {
    resource_grid_context context;
    context.sector = sector_id_dist(rgen);
    context.slot   = slot_point(to_numerology_value(scs), slot_dist(rgen));
    puxch_notifier->on_puxch_request_overflow(context);
    auto& entries = error_notifier_spy.get_puxch_request_overflow_errors();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back());
  }

  // Assert only six error events.
  ASSERT_EQ(error_notifier_spy.get_nof_errors(), 6);

  // No other events.
  ASSERT_EQ(timing_notifier_spy.get_nof_events(), 0);
  ASSERT_EQ(rx_symbol_notifier_spy.get_nof_events(), 0);
}

TEST_P(LowerPhyFixture, RxSymbolNotifiers)
{
  // Get PRACH notifier.
  prach_processor_notifier* prach_notifier = uplink_proc_spy->get_prach_notifier();
  ASSERT_NE(prach_notifier, nullptr);

  // Get PUxCH notifier.
  puxch_processor_notifier* puxch_notifier = uplink_proc_spy->get_puxch_notifier();
  ASSERT_NE(puxch_notifier, nullptr);

  // Make sure no notifications are stored.
  timing_notifier_spy.clear_all_events();
  rx_symbol_notifier_spy.clear_all_events();
  error_notifier_spy.clear_all_errors();

  // Notify PRACH buffer complete reception event.
  {
    prach_buffer_context context;
    context.sector                = sector_id_dist(rgen);
    context.port                  = 0;
    context.slot                  = slot_point(to_numerology_value(scs), slot_dist(rgen));
    context.start_symbol          = 0;
    context.format                = prach_format_type::zero;
    context.rb_offset             = 0;
    context.nof_td_occasions      = 0;
    context.nof_fd_occasions      = 0;
    context.nof_prb_ul_grid       = 0;
    context.pusch_scs             = scs;
    context.root_sequence_index   = 0;
    context.restricted_set        = restricted_set_config::UNRESTRICTED;
    context.zero_correlation_zone = 0;
    context.start_preamble_index  = 0;
    context.nof_preamble_indices  = 0;

    prach_buffer_spy buffer;

    prach_notifier->on_rx_prach_window(buffer, context);
    auto& entries = rx_symbol_notifier_spy.get_rx_prach_events();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back().context);
    ASSERT_EQ(&buffer, entries.back().buffer);
    ASSERT_EQ(buffer.get_total_count(), 0);
  }

  // Notify PUxCH grid complete reception event.
  {
    lower_phy_rx_symbol_context context;
    context.sector      = sector_id_dist(rgen);
    context.slot        = slot_point(to_numerology_value(scs), slot_dist(rgen));
    context.nof_symbols = 123;
    resource_grid_spy rg_spy;
    puxch_notifier->on_rx_symbol(rg_spy, context);
    auto& entries = rx_symbol_notifier_spy.get_rx_symbol_events();
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(context, entries.back().context);
    ASSERT_EQ(&rg_spy, entries.back().grid);
    ASSERT_EQ(rg_spy.get_total_count(), 0);
  }

  // Assert only six error events.
  ASSERT_EQ(rx_symbol_notifier_spy.get_nof_events(), 2);

  // No other events.
  ASSERT_EQ(timing_notifier_spy.get_nof_events(), 0);
  ASSERT_EQ(error_notifier_spy.get_nof_errors(), 0);
}

TEST_P(LowerPhyFixture, RgHandler)
{
  lower_phy_rg_handler&                      rg_handler     = lphy->get_rg_handler();
  const pdxch_processor_request_handler_spy& pdxch_proc_spy = downlink_proc_spy->get_pdxch_proc_request_handler_spy();

  // Prepare context.
  resource_grid_context context;
  context.sector = sector_id_dist(rgen);
  context.slot   = slot_point(to_numerology_value(scs), slot_dist(rgen));

  // Prepare RG spy.
  resource_grid_spy rg_spy;

  // Handle RG.
  rg_handler.handle_resource_grid(context, rg_spy);

  // Assert RG.
  auto& pdxch_entries = pdxch_proc_spy.get_entries();
  ASSERT_EQ(pdxch_entries.size(), 1);
  auto& pdxch_entry = pdxch_entries.back();
  ASSERT_EQ(pdxch_entry.context, context);
  ASSERT_EQ(pdxch_entry.grid, &rg_spy);
}

TEST_P(LowerPhyFixture, PrachRequestHandler)
{
  lower_phy_request_handler&                 request_handler = lphy->get_request_handler();
  const prach_processor_request_handler_spy& prach_proc_spy  = uplink_proc_spy->get_prach_req_handler_spy();

  // Prepare context.
  prach_buffer_context context;
  context.sector                = sector_id_dist(rgen);
  context.port                  = 0;
  context.slot                  = slot_point(to_numerology_value(scs), slot_dist(rgen));
  context.start_symbol          = 0;
  context.format                = prach_format_type::zero;
  context.rb_offset             = 0;
  context.nof_td_occasions      = 0;
  context.nof_fd_occasions      = 0;
  context.nof_prb_ul_grid       = 0;
  context.pusch_scs             = scs;
  context.root_sequence_index   = 0;
  context.restricted_set        = restricted_set_config::UNRESTRICTED;
  context.zero_correlation_zone = 0;
  context.start_preamble_index  = 0;
  context.nof_preamble_indices  = 0;

  // Prepare RG spy.
  prach_buffer_spy buffer_spy;

  // Request RG.
  request_handler.request_prach_window(context, buffer_spy);

  // Assert context and RG.
  auto& entries = prach_proc_spy.get_entries();
  ASSERT_EQ(entries.size(), 1);
  auto& puxch_entry = entries.back();
  ASSERT_EQ(puxch_entry.context, context);
  ASSERT_EQ(puxch_entry.buffer, &buffer_spy);

  // No method of the buffer should have been called.
  ASSERT_EQ(buffer_spy.get_total_count(), 0);
}

TEST_P(LowerPhyFixture, PuxchRequestHandler)
{
  lower_phy_request_handler&                 request_handler = lphy->get_request_handler();
  const puxch_processor_request_handler_spy& puxch_proc_spy  = uplink_proc_spy->get_puxch_req_handler_spy();

  // Prepare context.
  resource_grid_context context;
  context.sector = sector_id_dist(rgen);
  context.slot   = slot_point(to_numerology_value(scs), slot_dist(rgen));

  // Prepare RG spy.
  resource_grid_spy rg_spy;

  // Request RG.
  request_handler.request_uplink_slot(context, rg_spy);

  // Assert context and RG.
  auto& puxch_entries = puxch_proc_spy.get_entries();
  ASSERT_EQ(puxch_entries.size(), 1);
  auto& puxch_entry = puxch_entries.back();
  ASSERT_EQ(puxch_entry.context, context);
  ASSERT_EQ(puxch_entry.grid, &rg_spy);

  // No method of the grid should have been called.
  ASSERT_EQ(rg_spy.get_total_count(), 0);
}

TEST_P(LowerPhyFixture, BasebandNormalFlow)
{
  static constexpr unsigned nof_repetitions = 5;

  unsigned rx_to_tx_delay = get_rx_to_tx_delay();

  // Get lower PHY controller.
  lower_phy_controller& lphy_controller = lphy->get_controller();

  ASSERT_FALSE(rx_task_executor.has_pending_tasks());
  ASSERT_FALSE(tx_task_executor.has_pending_tasks());
  ASSERT_FALSE(ul_task_executor.has_pending_tasks());
  ASSERT_FALSE(dl_task_executor.has_pending_tasks());

  // Start streaming.
  lphy_controller.start();

  // Repeat for a number of baseband blocks.
  for (unsigned i_repetition = 0; i_repetition != nof_repetitions; ++i_repetition) {
    // Clear spies.
    bb_gateway_spy.clear_all_entries();
    downlink_proc_spy->clear();
    uplink_proc_spy->clear();

    // A task should be pending in Rx task and no baseband transaction should have occcurred.
    ASSERT_FALSE(tx_task_executor.has_pending_tasks());
    ASSERT_FALSE(ul_task_executor.has_pending_tasks());
    ASSERT_FALSE(dl_task_executor.has_pending_tasks());
    ASSERT_TRUE(bb_gateway_spy.get_transmit_entries().empty());
    ASSERT_TRUE(bb_gateway_spy.get_receive_entries().empty());

    // Execute Rx Task.
    ASSERT_TRUE(rx_task_executor.try_run_next());

    // Baseband receiver should have been called.
    auto& receive_entries = bb_gateway_spy.get_receive_entries();
    ASSERT_EQ(receive_entries.size(), 1);
    auto& receive_entry = receive_entries.back();
    ASSERT_EQ(receive_entry.stream_id, 0);
    ASSERT_EQ(receive_entry.data.get_nof_channels(), nof_rx_ports);
    ASSERT_EQ(receive_entry.data.get_nof_samples(), rx_buffer_size);

    // Run and assert downlink block.
    {
      // The DL task shall be pending.
      ASSERT_TRUE(dl_task_executor.try_run_next());

      // Extract and assert DL processor output.
      const downlink_processor_baseband_spy& baseband_spy  = downlink_proc_spy->get_downlink_proc_baseband_spy();
      auto&                                  dl_bb_entries = baseband_spy.get_entries();
      ASSERT_EQ(dl_bb_entries.size(), 1);
      auto& dl_bb_entry = dl_bb_entries.back();

      // The transmitter executor shall have the transmit task.
      ASSERT_TRUE(bb_gateway_spy.get_transmit_entries().empty());
      ASSERT_TRUE(tx_task_executor.try_run_next());

      // Extract and assert baseband transmission.
      auto& transmit_entries = bb_gateway_spy.get_transmit_entries();
      ASSERT_EQ(transmit_entries.size(), 1);
      auto& transmit_entry = transmit_entries.back();
      ASSERT_EQ(transmit_entry.stream_id, 0);
      ASSERT_EQ(transmit_entry.metadata.ts, receive_entry.metadata.ts + rx_to_tx_delay);
      ASSERT_EQ(transmit_entry.data.get_nof_channels(), nof_tx_ports);
      ASSERT_EQ(transmit_entry.data.get_nof_samples(), tx_buffer_size);

      // Assert transmitted signal is the same as generated in DL.
      ASSERT_EQ(dl_bb_entry, transmit_entry.data);
    }

    // Run and assert uplink block.
    {
      // The UL task shall be pending.
      ASSERT_TRUE(ul_task_executor.try_run_next());

      // Extract and assert DL processor output.
      const uplink_processor_baseband_spy& baseband_spy  = uplink_proc_spy->get_uplink_proc_baseband_spy();
      auto&                                ul_bb_entries = baseband_spy.get_entries();
      ASSERT_EQ(ul_bb_entries.size(), 1);
      auto& ul_bb_entry = ul_bb_entries.back();

      // Assert received signal is the same as processed in UL.
      ASSERT_EQ(ul_bb_entry, receive_entry.data);
    }

    // No task should be pending in DL, UL nor Tx.
    ASSERT_FALSE(dl_task_executor.has_pending_tasks());
    ASSERT_FALSE(ul_task_executor.has_pending_tasks());
    ASSERT_FALSE(tx_task_executor.has_pending_tasks());
    ASSERT_TRUE(rx_task_executor.has_pending_tasks());
  }

  // Request stop streaming asynchronously. As executors run in the main thread, it avoids deadlock.
  std::thread stop_thread([&lphy_controller]() { lphy_controller.stop(); });

  // Flush pending tasks until no task is left.
  while (rx_task_executor.try_run_next()) {
    dl_task_executor.run_pending_tasks();
    tx_task_executor.run_pending_tasks();
    ul_task_executor.run_pending_tasks();
  }

  // Join asynchronous thread.
  stop_thread.join();

  // No task should be pending.
  ASSERT_FALSE(tx_task_executor.has_pending_tasks());
  ASSERT_FALSE(rx_task_executor.has_pending_tasks());
  ASSERT_FALSE(ul_task_executor.has_pending_tasks());
  ASSERT_FALSE(dl_task_executor.has_pending_tasks());
}

INSTANTIATE_TEST_SUITE_P(
    LowerPhy,
    LowerPhyFixture,
    testing::Combine(testing::Values(subcarrier_spacing::kHz15, subcarrier_spacing::kHz30, subcarrier_spacing::kHz60),
                     testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED),
                     testing::Values(sampling_rate::from_MHz(23.04), sampling_rate::from_MHz(30.72)),
                     testing::Values(n_ta_offset::n25600, n_ta_offset::n39936),
                     testing::Values(25, 52, 106)));

} // namespace