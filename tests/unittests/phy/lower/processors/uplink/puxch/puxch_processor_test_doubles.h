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

#include "../../../../../gateways/baseband/baseband_gateway_buffer_test_doubles.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_factories.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_notifier.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_request_handler.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/srslog/srslog.h"
#include <vector>

namespace srsran {

class puxch_processor_baseband_spy : public puxch_processor_baseband
{
public:
  struct entry_t {
    baseband_gateway_buffer_read_only samples;
    lower_phy_rx_symbol_context       context;
  };

  void process_symbol(const baseband_gateway_buffer_reader& samples,
                      const lower_phy_rx_symbol_context&    context) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.samples  = samples;
    entry.context  = context;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<entry_t> entries;
};

class puxch_processor_request_handler_spy : public puxch_processor_request_handler
{
public:
  struct entry_t {
    resource_grid*        grid;
    resource_grid_context context;
  };

  void handle_request(resource_grid& grid, const resource_grid_context& context) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.grid     = &grid;
    entry.context  = context;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

private:
  std::vector<entry_t> entries;
};

class puxch_processor_spy : public puxch_processor
{
public:
  puxch_processor_spy(const puxch_processor_configuration& config_) : config(config_) {}

  void connect(puxch_processor_notifier& notifier_) override { notifier = &notifier_; }

  puxch_processor_request_handler& get_request_handler() override { return request_handler; }

  puxch_processor_baseband& get_baseband() override { return baseband; }

  const puxch_processor_configuration& get_configuration() const { return config; }

  const puxch_processor_notifier* get_notifier() const { return notifier; }

  const std::vector<puxch_processor_baseband_spy::entry_t>& get_baseband_entries() const
  {
    return baseband.get_entries();
  }

  void clear() { baseband.clear(); }

private:
  puxch_processor_notifier*           notifier = nullptr;
  puxch_processor_configuration       config;
  puxch_processor_request_handler_spy request_handler;
  puxch_processor_baseband_spy        baseband;
};

class puxch_processor_factory_spy : public puxch_processor_factory
{
public:
  std::unique_ptr<puxch_processor> create(const puxch_processor_configuration& config) override
  {
    std::unique_ptr<puxch_processor_spy> ptr = std::make_unique<puxch_processor_spy>(config);
    instance                                 = ptr.get();
    return std::move(ptr);
  }

  puxch_processor_spy& get_spy() { return *instance; }

private:
  puxch_processor_spy* instance = nullptr;
};

} // namespace srsran