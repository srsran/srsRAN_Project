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

#pragma once
#include "../../../../gateways/baseband/baseband_gateway_buffer_test_doubles.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_factories.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_notifier.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
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

  void handle_request(const resource_grid_reader& grid, const resource_grid_context& context) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.context  = context;
    entry.grid     = &grid;
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
    baseband_gateway_buffer_read_only buffer;
    baseband_gateway_timestamp        timestamp;
  };

  baseband_gateway_transmitter_metadata process(baseband_gateway_buffer_writer& buffer,
                                                baseband_gateway_timestamp      timestamp) override
  {
    for (unsigned i_channel = 0, i_channel_end = buffer.get_nof_channels(); i_channel != i_channel_end; ++i_channel) {
      span<cf_t> samples = buffer.get_channel_buffer(i_channel);
      std::generate(samples.begin(), samples.end(), [this]() { return cf_t(sample_dist(rgen), sample_dist(rgen)); });
    }

    entries.emplace_back();
    entry_t& entry  = entries.back();
    entry.timestamp = timestamp;
    entry.buffer    = baseband_gateway_buffer_read_only(buffer);

    baseband_gateway_transmitter_metadata md;
    md.is_empty = false;
    return md;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> sample_dist;
  std::vector<entry_t>                  entries;
};

class lower_phy_downlink_processor_spy : public lower_phy_downlink_processor
{
public:
  lower_phy_downlink_processor_spy(downlink_processor_configuration config_) : config(config_) {}

  void connect(downlink_processor_notifier& notifier_, pdxch_processor_notifier& pdxch_notifier_) override
  {
    notifier       = &notifier_;
    pdxch_notifier = &pdxch_notifier_;
  }

  pdxch_processor_request_handler& get_downlink_request_handler() override { return pdxch_proc_request_handler_spy; }

  downlink_processor_baseband& get_baseband() override { return downlink_proc_baseband_spy; }

  const downlink_processor_configuration& get_config() const { return config; }

  downlink_processor_notifier* get_notifier() { return notifier; }

  pdxch_processor_notifier* get_pdxch_notifier() { return pdxch_notifier; }

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
  downlink_processor_configuration    config;
  downlink_processor_notifier*        notifier;
  pdxch_processor_notifier*           pdxch_notifier;
  pdxch_processor_request_handler_spy pdxch_proc_request_handler_spy;
  downlink_processor_baseband_spy     downlink_proc_baseband_spy;
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