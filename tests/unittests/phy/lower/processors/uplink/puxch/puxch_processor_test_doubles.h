/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
#include "srsran/phy/support/shared_resource_grid.h"
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

  bool process_symbol(const baseband_gateway_buffer_reader& samples,
                      const lower_phy_rx_symbol_context&    context) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.samples  = samples;
    entry.context  = context;
    return true;
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
    const resource_grid*  grid;
    resource_grid_context context;
  };

  void handle_request(const shared_resource_grid& grid, const resource_grid_context& context) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.grid     = &grid.get();
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