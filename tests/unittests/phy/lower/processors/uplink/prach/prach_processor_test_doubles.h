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
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_factories.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_notifier.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_request_handler.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/srslog/srslog.h"
#include <vector>

namespace srsran {

class prach_processor_baseband_spy : public prach_processor_baseband
{
public:
  struct entry_t {
    baseband_gateway_buffer_read_only samples;
    symbol_context                    context;
  };

  void process_symbol(const baseband_gateway_buffer_reader& samples, const symbol_context& context) override
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

class prach_processor_request_handler_spy : public prach_processor_request_handler
{
public:
  struct entry_t {
    prach_buffer_context context;
    const prach_buffer*  buffer;
  };

  void handle_request(prach_buffer& buffer, const prach_buffer_context& context) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.context  = context;
    entry.buffer   = &buffer;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<entry_t> entries;
};

class prach_processor_spy : public prach_processor
{
public:
  void                             connect(prach_processor_notifier& notifier) override {}
  void                             stop() override {}
  prach_processor_request_handler& get_request_handler() override { return request_handler; }
  prach_processor_baseband&        get_baseband() override { return baseband; }

  const std::vector<prach_processor_baseband_spy::entry_t>& get_baseband_entries() const
  {
    return baseband.get_entries();
  }

  void clear() { baseband.clear(); }

private:
  prach_processor_request_handler_spy request_handler;
  prach_processor_baseband_spy        baseband;
};

class prach_processor_factory_spy : public prach_processor_factory
{
public:
  std::unique_ptr<prach_processor> create() override
  {
    std::unique_ptr<prach_processor_spy> ptr = std::make_unique<prach_processor_spy>();
    instance                                 = ptr.get();
    return std::move(ptr);
  }

  prach_processor_spy& get_spy() const { return *instance; }

private:
  prach_processor_spy* instance = nullptr;
};

} // namespace srsran
