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

#pragma once

#include "../../../../gateways/baseband/baseband_gateway_buffer_test_doubles.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_factories.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_notifier.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "srsran/phy/lower/processors/lower_phy_center_freq_controller.h"
#include "srsran/phy/lower/processors/lower_phy_cfo_controller.h"
#include "srsran/phy/lower/processors/lower_phy_tx_time_offset_controller.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/srsvec/copy.h"
#include <random>
#include <vector>

namespace srsran {

class pdxch_processor_request_handler_spy : public pdxch_processor_request_handler
{
public:
  struct entry_t {
    const resource_grid_reader* grid;
    resource_grid_context       context;
  };

  void handle_request(const shared_resource_grid& grid, const resource_grid_context& context) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.context  = context;
    entry.grid     = &grid.get_reader();
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<entry_t> entries;
};

class downlink_processor_baseband_spy : public downlink_processor_baseband
{
public:
  struct entry_t {
    baseband_gateway_transmitter_metadata metadata;
    baseband_gateway_buffer_read_only     samples;
  };

  downlink_processor_baseband_spy(subcarrier_spacing scs_, cyclic_prefix cp, sampling_rate srate, unsigned nof_ports_) :
    avg_power_dist(0.01, 1.0),
    peak_power_dist(1.0, 10.0),
    scs(scs_),
    nof_ports(nof_ports_),
    nof_samples_per_subframe(srate.to_kHz()),
    nof_slots_per_subframe(get_nof_slots_per_subframe(scs)),
    nof_symbols_per_slot(get_nsymb_per_slot(cp)),
    buffer_pool(1, nof_ports, srate.to_kHz()),
    random_data_index(0),
    random_data(1009)
  {
    // Generate random data for populating the buffers.
    std::uniform_real_distribution<float> sample_dist(-1, 1);
    std::generate(random_data.begin(), random_data.end(), [this, &sample_dist]() {
      return to_ci16(cf_t(sample_dist(rgen) * INT16_MAX, sample_dist(rgen) * INT16_MAX));
    });

    // Calculate symbol sizes in a subframe.
    for (unsigned i_symbol_sf = 0; i_symbol_sf != nof_symbols_per_slot * nof_slots_per_subframe; ++i_symbol_sf) {
      symbol_sizes_sf[i_symbol_sf] =
          srate.get_dft_size(scs) + cp.get_length(i_symbol_sf, scs).to_samples(srate.to_Hz());
    }
  }

  processing_result process(baseband_gateway_timestamp timestamp) override
  {
    // Calculate the subframe index.
    auto i_sf = static_cast<unsigned>((timestamp / nof_samples_per_subframe) % (NOF_SFNS * NOF_SUBFRAMES_PER_FRAME));
    // Calculate the sample index within the subframe.
    unsigned i_sample_sf = timestamp % nof_samples_per_subframe;

    // Calculate symbol index within the subframe and the sample index within the OFDM symbol.
    unsigned i_sample_symbol = i_sample_sf;
    unsigned i_symbol_sf     = 0;
    while (i_sample_symbol >= symbol_sizes_sf[i_symbol_sf]) {
      i_sample_symbol -= symbol_sizes_sf[i_symbol_sf];
      ++i_symbol_sf;
    }

    // Calculate system slot index and the symbol index within the slot.
    unsigned i_slot   = i_sf * nof_slots_per_subframe + i_symbol_sf / nof_symbols_per_slot;
    unsigned i_symbol = i_symbol_sf % nof_symbols_per_slot;

    // If the timestamp is not aligned with the beginning of a slot. Align with the next subframe.
    if ((i_symbol != 0) || (i_sample_symbol != 0)) {
      // Prepare result metadata and obtain baseband buffer from the pool.
      processing_result result = {};
      result.metadata.ts       = timestamp;
      result.metadata.is_empty = true;
      result.buffer            = buffer_pool.get();
      report_fatal_error_if_not(result.buffer, "Failed to retrieve a baseband buffer.");

      // Calculate the number of samples to the next subframe.
      unsigned nof_samples_to_next_sf = nof_samples_per_subframe - i_sample_sf;

      // Resize buffer and zero.
      result.buffer->resize(nof_samples_to_next_sf);
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        srsvec::zero(result.buffer->get_writer()[i_port]);
      }

      // Save results.
      save_result_as_entry(result);

      return result;
    }

    // Create slot point.
    slot_point slot(to_numerology_value(scs), i_slot);

    // Obtain buffer from the pool.
    baseband_gateway_buffer_ptr buffer = buffer_pool.get();
    srsran_assert(buffer, "Buffer pool is exhausted.");

    // Calculate the OFDM symbol indexes within the subframe.
    unsigned i_symbol_sf_begin = slot.subframe_slot_index() * nof_symbols_per_slot;
    unsigned i_symbol_sf_end   = i_symbol_sf_begin + nof_symbols_per_slot;

    // Calculate number of samples within the current_context.slot.
    unsigned nof_samples_slot =
        std::accumulate(symbol_sizes_sf.begin() + i_symbol_sf_begin, symbol_sizes_sf.begin() + i_symbol_sf_end, 0);

    // Resize buffer.
    buffer->resize(nof_samples_slot);

    // Generate random data.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      span<ci16_t> buffer_view = buffer->get_writer()[i_port];
      std::generate(buffer_view.begin(), buffer_view.end(), [this]() {
        unsigned index    = random_data_index;
        random_data_index = (random_data_index + 1) % random_data.size();
        return to_ci16(cf_t(random_data[index].real() * INT16_MAX, random_data[index].imag() * INT16_MAX));
      });
    }

    processing_result result = {
        .metadata = {.ts = timestamp, .is_empty = false, .tx_start = std::nullopt, .tx_end = std::nullopt},
        .buffer   = std::move(buffer)};

    // Save results.
    save_result_as_entry(result);

    return result;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  static constexpr unsigned max_nof_symbols_per_subframe =
      MAX_NSYMB_PER_SLOT * pow2(to_numerology_value(subcarrier_spacing::kHz240));

  void save_result_as_entry(const processing_result& result)
  {
    entry_t& entry = entries.emplace_back();
    entry.metadata = result.metadata;
    entry.samples  = baseband_gateway_buffer_read_only(result.buffer->get_reader());
  }

  std::mt19937                          rgen;
  std::uniform_real_distribution<float> avg_power_dist;
  std::uniform_real_distribution<float> peak_power_dist;

  subcarrier_spacing                                 scs;
  unsigned                                           nof_ports;
  unsigned                                           nof_samples_per_subframe;
  unsigned                                           nof_slots_per_subframe;
  unsigned                                           nof_symbols_per_slot;
  std::array<unsigned, max_nof_symbols_per_subframe> symbol_sizes_sf;

  baseband_gateway_buffer_pool buffer_pool;

  unsigned              random_data_index;
  std::vector<cf_t>     random_data;
  std::vector<entry_t>  entries;
  std::vector<unsigned> frame_slots_to_process;
};

class baseband_cfo_processor_spy : public lower_phy_cfo_controller
{
public:
  bool schedule_cfo_command(time_point time, float cfo_Hz, float cfo_drift_Hz_s) override { return false; }
};

class lower_phy_center_freq_controller_spy : public lower_phy_center_freq_controller
{
public:
  bool set_carrier_center_frequency(double carrier_center_frequency_Hz) override { return false; }
};

class lower_phy_tx_time_offset_controller_spy : public lower_phy_tx_time_offset_controller
{
public:
  void set_tx_time_offset(phy_time_unit tx_time_offset) override {}
};

class lower_phy_downlink_processor_spy : public lower_phy_downlink_processor
{
public:
  lower_phy_downlink_processor_spy(downlink_processor_configuration config_) :
    config(config_), downlink_proc_baseband_spy(config.scs, config.cp, config.rate, config.nof_tx_ports)
  {
  }

  void connect(downlink_processor_notifier& notifier_, pdxch_processor_notifier& pdxch_notifier_) override
  {
    notifier       = &notifier_;
    pdxch_notifier = &pdxch_notifier_;
  }

  void stop() override {}

  pdxch_processor_request_handler& get_downlink_request_handler() override { return pdxch_proc_request_handler_spy; }

  downlink_processor_baseband& get_baseband() override { return downlink_proc_baseband_spy; }

  const downlink_processor_configuration& get_config() const { return config; }

  downlink_processor_notifier* get_notifier() { return notifier; }

  pdxch_processor_notifier* get_pdxch_notifier() { return pdxch_notifier; }

  baseband_cfo_processor_spy& get_cfo_control() override { return cfo_processor_spy; }

  lower_phy_center_freq_controller& get_carrier_center_frequency_control() override { return center_freq_control_spy; }

  lower_phy_tx_time_offset_controller& get_tx_time_offset_control() override { return tx_time_offset_control_spy; }

  const pdxch_processor_request_handler_spy& get_pdxch_proc_request_handler_spy() const
  {
    return pdxch_proc_request_handler_spy;
  }

  const downlink_processor_baseband_spy& get_downlink_proc_baseband_spy() const { return downlink_proc_baseband_spy; }

  void clear()
  {
    notifier       = nullptr;
    pdxch_notifier = nullptr;
    pdxch_proc_request_handler_spy.clear();
    downlink_proc_baseband_spy.clear();
  }

private:
  downlink_processor_configuration        config;
  downlink_processor_notifier*            notifier;
  pdxch_processor_notifier*               pdxch_notifier;
  baseband_cfo_processor_spy              cfo_processor_spy;
  lower_phy_center_freq_controller_spy    center_freq_control_spy;
  lower_phy_tx_time_offset_controller_spy tx_time_offset_control_spy;
  pdxch_processor_request_handler_spy     pdxch_proc_request_handler_spy;
  downlink_processor_baseband_spy         downlink_proc_baseband_spy;
};

class lower_phy_downlink_processor_factory_spy : public lower_phy_downlink_processor_factory
{
public:
  std::unique_ptr<lower_phy_downlink_processor> create(const downlink_processor_configuration& config) override
  {
    std::unique_ptr<lower_phy_downlink_processor_spy> proc = std::make_unique<lower_phy_downlink_processor_spy>(config);
    entries.emplace_back(proc.get());
    return proc;
  }

  std::vector<lower_phy_downlink_processor_spy*>& get_entries() { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<lower_phy_downlink_processor_spy*> entries;
};

} // namespace srsran
