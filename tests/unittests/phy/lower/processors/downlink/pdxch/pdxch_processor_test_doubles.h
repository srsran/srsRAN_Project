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

#include "../../../../../gateways/baseband/baseband_gateway_buffer_test_doubles.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_factories.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_notifier.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "srsran/phy/lower/processors/lower_phy_center_freq_controller.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/srslog/srslog.h"
#include <random>
#include <vector>

namespace srsran {

class pdxch_processor_baseband_spy : public pdxch_processor_baseband
{
public:
  struct entry_t {
    slot_context                      context;
    lower_phy_baseband_metrics        metrics;
    baseband_gateway_buffer_read_only samples;
  };

  // Constructs a PDxCH processor baseband spy that processes all slots and symbols.
  pdxch_processor_baseband_spy(subcarrier_spacing scs, cyclic_prefix cp, sampling_rate srate, unsigned nof_ports_) :
    avg_power_dist(0.01, 1.0),
    peak_power_dist(1.0, 10.0),
    nof_ports(nof_ports_),
    nof_slots_per_subframe(get_nof_slots_per_subframe(scs)),
    nof_symbols_per_slot(get_nsymb_per_slot(cp)),
    buffer_pool(1, nof_ports, srate.to_kHz()),
    random_data_index(0),
    random_data(1009)
  {
    // Generate random data for populating the buffers.
    std::uniform_real_distribution<float> sample_dist(-1, 1);
    std::generate(random_data.begin(), random_data.end(), [this, &sample_dist]() {
      return cf_t(sample_dist(rgen), sample_dist(rgen));
    });

    // Calculate symbol sizes in a subframe.
    for (unsigned i_symbol_sf = 0; i_symbol_sf != nof_symbols_per_slot * nof_slots_per_subframe; ++i_symbol_sf) {
      symbol_sizes_sf[i_symbol_sf] =
          srate.get_dft_size(scs) + cp.get_length(i_symbol_sf, scs).to_samples(srate.to_Hz());
    }
  }

  slot_result process_slot(slot_context context) override
  {
    // Obtain buffer from the pool.
    baseband_gateway_buffer_ptr buffer = buffer_pool.get();
    srsran_assert(buffer, "Buffer pool is exhausted.");

    // Calculate the OFDM symbol indexes within the subframe.
    unsigned i_symbol_sf_begin = context.slot.subframe_slot_index() * nof_symbols_per_slot;
    unsigned i_symbol_sf_end   = i_symbol_sf_begin + nof_symbols_per_slot;

    // Calculate number of samples within the current_context.slot.
    unsigned nof_samples_slot =
        std::accumulate(symbol_sizes_sf.begin() + i_symbol_sf_begin, symbol_sizes_sf.begin() + i_symbol_sf_end, 0);

    // Resize buffer.
    buffer->resize(nof_samples_slot);

    // Generate random data.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      span<cf_t> buffer_view = buffer->get_writer()[i_port];
      std::generate(buffer_view.begin(), buffer_view.end(), [this]() {
        unsigned index    = random_data_index;
        random_data_index = (random_data_index + 1) % random_data.size();
        return random_data[index];
      });
    }

    slot_result result = {.metrics = {.avg_power = avg_power_dist(rgen)}, .buffer = std::move(buffer)};

    // Save results.
    entry_t& entry = entries.emplace_back();
    entry.context  = context;
    entry.samples  = baseband_gateway_buffer_read_only(result.buffer->get_reader());
    entry.metrics  = result.metrics;

    return result;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  static constexpr unsigned max_nof_symbols_per_subframe =
      MAX_NSYMB_PER_SLOT * pow2(to_numerology_value(subcarrier_spacing::kHz240));

  std::mt19937                          rgen;
  std::uniform_real_distribution<float> avg_power_dist;
  std::uniform_real_distribution<float> peak_power_dist;

  unsigned                                           nof_ports;
  unsigned                                           nof_slots_per_subframe;
  unsigned                                           nof_symbols_per_slot;
  std::array<unsigned, max_nof_symbols_per_subframe> symbol_sizes_sf;

  baseband_gateway_buffer_pool buffer_pool;

  unsigned              random_data_index;
  std::vector<cf_t>     random_data;
  std::vector<entry_t>  entries;
  std::vector<unsigned> frame_slots_to_process;
};

class pdxch_processor_request_handler_spy : public pdxch_processor_request_handler
{
public:
  void handle_request(const shared_resource_grid& grid, const resource_grid_context& context) override
  {
    // TBD.
  }
};

class pdxch_processor_spy : public pdxch_processor, private lower_phy_center_freq_controller
{
public:
  pdxch_processor_spy(const pdxch_processor_configuration& config_) :
    config(config_), baseband(config.scs, config.cp, config.srate, config.nof_tx_ports)
  {
  }

  void                             connect(pdxch_processor_notifier& notifier_) override { notifier = &notifier_; }
  pdxch_processor_request_handler& get_request_handler() override { return request_handler; }
  pdxch_processor_baseband&        get_baseband() override { return baseband; }
  void                             stop() override {}

  const pdxch_processor_configuration& get_configuration() const { return config; }

  const pdxch_processor_notifier* get_notifier() const { return notifier; }

  lower_phy_center_freq_controller& get_center_freq_control() override { return *this; }

  const std::vector<pdxch_processor_baseband_spy::entry_t>& get_baseband_entries() const
  {
    return baseband.get_entries();
  }

  void clear() { baseband.clear(); }

private:
  // See interface for documentation.
  bool set_carrier_center_frequency(double carrier_center_frequency_Hz) override { return false; }

  pdxch_processor_notifier*           notifier = nullptr;
  pdxch_processor_configuration       config;
  pdxch_processor_request_handler_spy request_handler;
  pdxch_processor_baseband_spy        baseband;
};

class pdxch_processor_factory_spy : public pdxch_processor_factory
{
public:
  pdxch_processor_factory_spy() = default;

  std::unique_ptr<pdxch_processor> create(const pdxch_processor_configuration& config) override
  {
    std::unique_ptr<pdxch_processor_spy> ptr;

    ptr = std::make_unique<pdxch_processor_spy>(config);

    instance = ptr.get();
    return std::move(ptr);
  }

  pdxch_processor_spy& get_spy()
  {
    srsran_assert(instance != nullptr, "Instance not available.");
    return *instance;
  }

private:
  pdxch_processor_spy* instance = nullptr;
};

} // namespace srsran
