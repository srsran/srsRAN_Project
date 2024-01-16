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

#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/srsran_test.h"
#include <random>

namespace srsran {

class ofdm_symbol_modulator_spy : public ofdm_symbol_modulator
{
private:
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> dist;
  ofdm_modulator_configuration          configuration;

  struct modulate_entry {
    std::vector<cf_t>           output;
    const resource_grid_reader* grid;
    unsigned                    port_index;
    unsigned                    symbol_index;
  };
  std::vector<modulate_entry> modulate_entries;

public:
  ofdm_symbol_modulator_spy(const ofdm_modulator_configuration& config) : rgen(0), dist(-1, +1), configuration(config)
  {
    // Do nothing.
  }

  unsigned get_symbol_size(unsigned symbol_index) const override
  {
    unsigned sampling_rate_Hz = (configuration.dft_size * 15000) << configuration.numerology;
    return configuration.cp.get_length(symbol_index, to_subcarrier_spacing(configuration.numerology))
               .to_samples(sampling_rate_Hz) +
           configuration.dft_size;
  }

  void
  modulate(span<cf_t> output, const resource_grid_reader& grid, unsigned port_index, unsigned symbol_index) override
  {
    TESTASSERT_EQ(output.size(), get_symbol_size(symbol_index));

    modulate_entries.emplace_back();
    modulate_entry& entry = modulate_entries.back();
    entry.grid            = &grid;
    entry.port_index      = port_index;
    entry.symbol_index    = symbol_index;

    // Prepare output.
    entry.output.resize(output.size());
    std::generate(entry.output.begin(), entry.output.end(), [this]() { return cf_t(dist(rgen), dist(rgen)); });
    srsvec::copy(output, entry.output);
  }

  void clear_modulate_entries() { modulate_entries.clear(); }

  const ofdm_modulator_configuration& get_configuration() const { return configuration; }

  const std::vector<modulate_entry>& get_modulate_entries() const { return modulate_entries; }
};

class ofdm_modulator_factory_spy : public ofdm_modulator_factory
{
private:
  std::vector<ofdm_symbol_modulator_spy*> modulators;

public:
  std::unique_ptr<ofdm_symbol_modulator>
  create_ofdm_symbol_modulator(const ofdm_modulator_configuration& config) override
  {
    std::unique_ptr<ofdm_symbol_modulator_spy> ptr = std::make_unique<ofdm_symbol_modulator_spy>(config);
    modulators.push_back(ptr.get());
    return ptr;
  }
  std::unique_ptr<ofdm_slot_modulator> create_ofdm_slot_modulator(const ofdm_modulator_configuration& config) override
  {
    return nullptr;
  }

  std::vector<ofdm_symbol_modulator_spy*>& get_modulators() { return modulators; };
};

} // namespace srsran
