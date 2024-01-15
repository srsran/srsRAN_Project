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
#include "prach/prach_processor_test_doubles.h"
#include "puxch/puxch_processor_test_doubles.h"
#include "srsran/phy/lower/lower_phy_timing_context.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_factories.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_notifier.h"

namespace srsran {

class uplink_processor_notifier_spy : public uplink_processor_notifier
{
public:
  void on_half_slot(const lower_phy_timing_context& context) override { half_slots.emplace_back(context); }

  void on_full_slot(const lower_phy_timing_context& context) override { full_slots.emplace_back(context); }

  const std::vector<lower_phy_timing_context>& get_half_slots() const { return half_slots; }

  const std::vector<lower_phy_timing_context>& get_full_slots() const { return full_slots; }

  void clear_notifications()
  {
    half_slots.clear();
    full_slots.clear();
  }

private:
  std::vector<lower_phy_timing_context> half_slots;
  std::vector<lower_phy_timing_context> full_slots;
};

class uplink_processor_baseband_spy : public uplink_processor_baseband
{
public:
  struct entry_t {
    baseband_gateway_buffer_read_only buffer;
    baseband_gateway_timestamp        timestamp;
  };

  void process(const baseband_gateway_buffer_reader& buffer, baseband_gateway_timestamp timestamp) override
  {
    entries.emplace_back();
    entry_t& entry  = entries.back();
    entry.timestamp = timestamp;
    entry.buffer    = buffer;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<entry_t> entries;
};

class lower_phy_uplink_processor_spy : public lower_phy_uplink_processor
{
public:
  lower_phy_uplink_processor_spy(const uplink_processor_configuration& config_) : config(config_) {}

  void connect(uplink_processor_notifier& notifier_,
               prach_processor_notifier&  prach_notifier_,
               puxch_processor_notifier&  puxch_notifier_) override
  {
    notifier       = &notifier_;
    prach_notifier = &prach_notifier_;
    puxch_notifier = &puxch_notifier_;
  }

  const uplink_processor_configuration& get_config() const { return config; }

  prach_processor_request_handler& get_prach_request_handler() override { return prach_req_handler_spy; }

  puxch_processor_request_handler& get_puxch_request_handler() override { return puxch_req_handler_spy; }

  uplink_processor_baseband& get_baseband() override { return uplink_proc_baseband_spy; }

  uplink_processor_notifier* get_notifier() { return notifier; }

  prach_processor_notifier* get_prach_notifier() { return prach_notifier; }

  puxch_processor_notifier* get_puxch_notifier() { return puxch_notifier; }

  const uplink_processor_baseband_spy& get_uplink_proc_baseband_spy() const { return uplink_proc_baseband_spy; }

  const prach_processor_request_handler_spy& get_prach_req_handler_spy() const { return prach_req_handler_spy; }

  const puxch_processor_request_handler_spy& get_puxch_req_handler_spy() const { return puxch_req_handler_spy; }

  void clear()
  {
    notifier       = nullptr;
    prach_notifier = nullptr;
    puxch_notifier = nullptr;
    prach_req_handler_spy.clear();
    uplink_proc_baseband_spy.clear();
  }

private:
  uplink_processor_configuration      config;
  uplink_processor_notifier*          notifier       = nullptr;
  prach_processor_notifier*           prach_notifier = nullptr;
  puxch_processor_notifier*           puxch_notifier = nullptr;
  prach_processor_request_handler_spy prach_req_handler_spy;
  puxch_processor_request_handler_spy puxch_req_handler_spy;
  uplink_processor_baseband_spy       uplink_proc_baseband_spy;
};

class lower_phy_uplink_processor_factory_spy : public lower_phy_uplink_processor_factory
{
public:
  std::unique_ptr<lower_phy_uplink_processor> create(const uplink_processor_configuration& config) override
  {
    std::unique_ptr<lower_phy_uplink_processor_spy> proc = std::make_unique<lower_phy_uplink_processor_spy>(config);
    entries.emplace_back(proc.get());
    return proc;
  }

  std::vector<lower_phy_uplink_processor_spy*>& get_entries() { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<lower_phy_uplink_processor_spy*> entries;
};

} // namespace srsran