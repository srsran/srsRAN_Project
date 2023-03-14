/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gateways/baseband/baseband_gateway_buffer.h"
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
    const baseband_gateway_buffer* samples;
    lower_phy_rx_symbol_context    context;
  };

  void process_symbol(const baseband_gateway_buffer& samples, const lower_phy_rx_symbol_context& context) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.samples  = &samples;
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
  void handle_request(resource_grid& grid, const resource_grid_context& context) override
  {
    // TBD.
  }
};

class puxch_processor_spy : public puxch_processor
{
public:
  void                             connect(puxch_processor_notifier& notifier) override {}
  puxch_processor_request_handler& get_request_handler() override { return request_handler; }
  puxch_processor_baseband&        get_baseband() override { return baseband; }

  const std::vector<puxch_processor_baseband_spy::entry_t>& get_baseband_entries() const
  {
    return baseband.get_entries();
  }

  void clear() { baseband.clear(); }

private:
  puxch_processor_request_handler_spy request_handler;
  puxch_processor_baseband_spy        baseband;
};

class puxch_processor_factory_spy : public puxch_processor_factory
{
public:
  std::unique_ptr<puxch_processor> create(const puxch_processor_configuration& /**/) override
  {
    std::unique_ptr<puxch_processor_spy> ptr = std::make_unique<puxch_processor_spy>();
    instance                                 = ptr.get();
    return std::move(ptr);
  }

  puxch_processor_spy& get_spy() { return *instance; }

private:
  puxch_processor_spy* instance = nullptr;
};

} // namespace srsran