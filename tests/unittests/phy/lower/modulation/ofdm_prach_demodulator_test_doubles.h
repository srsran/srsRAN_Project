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

#include "srsran/phy/lower/modulation/modulation_factories.h"

namespace srsran {

class ofdm_prach_demodulator_spy : public ofdm_prach_demodulator
{
public:
  struct demodulate_entry {
    prach_buffer*     buffer;
    std::vector<cf_t> input;
    configuration     config;
  };

  void demodulate(prach_buffer& buffer, span<const cf_t> input, const configuration& config) override
  {
    demodulate_entries.emplace_back();
    demodulate_entry& entry = demodulate_entries.back();
    entry.buffer            = &buffer;
    entry.input.resize(input.size());
    std::copy(input.begin(), input.end(), entry.input.begin());
    entry.config = config;
  }

  const std::vector<demodulate_entry> get_demodulate_entries() const { return demodulate_entries; }

private:
  std::vector<demodulate_entry> demodulate_entries;
};

class ofdm_prach_demodulator_factory_spy : public ofdm_prach_demodulator_factory
{
public:
  std::unique_ptr<ofdm_prach_demodulator> create() override
  {
    // Create spy.
    std::unique_ptr<ofdm_prach_demodulator_spy> demodulator_spy = std::make_unique<ofdm_prach_demodulator_spy>();

    // Save spy reference.
    entries.push_back(demodulator_spy.get());

    // Return the smart pointer.
    return demodulator_spy;
  }

  std::vector<ofdm_prach_demodulator_spy*>& get_entries() { return entries; }

  unsigned get_nof_total_demodulate_entries() const
  {
    unsigned count = 0;

    for (const ofdm_prach_demodulator_spy* spy : entries) {
      count += spy->get_demodulate_entries().size();
    }

    return count;
  }

  std::vector<ofdm_prach_demodulator_spy::demodulate_entry> get_total_demodulate_entries() const
  {
    std::vector<ofdm_prach_demodulator_spy::demodulate_entry> result;
    result.reserve(get_nof_total_demodulate_entries());

    for (const ofdm_prach_demodulator_spy* spy : entries) {
      const std::vector<ofdm_prach_demodulator_spy::demodulate_entry>& demod_entries = spy->get_demodulate_entries();
      result.insert(result.end(), demod_entries.begin(), demod_entries.end());
    }

    return result;
  }

private:
  std::vector<ofdm_prach_demodulator_spy*> entries;
};

} // namespace srsran