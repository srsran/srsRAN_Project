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

class ofdm_symbol_demodulator_spy : public ofdm_symbol_demodulator
{
public:
  struct demodulate_entry {
    std::vector<cf_t>           input;
    const resource_grid_writer* grid;
    unsigned                    port_index;
    unsigned                    symbol_index;
  };

  ofdm_symbol_demodulator_spy(const ofdm_demodulator_configuration& config) :
    rgen(0), dist(-1, +1), configuration(config)
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
  demodulate(resource_grid_writer& grid, span<const cf_t> input, unsigned port_index, unsigned symbol_index) override
  {
    TESTASSERT_EQ(input.size(), get_symbol_size(symbol_index));

    demodulate_entries.emplace_back();
    demodulate_entry& entry = demodulate_entries.back();
    entry.grid              = &grid;
    entry.port_index        = port_index;
    entry.symbol_index      = symbol_index;

    entry.input.resize(input.size());
    srsvec::copy(entry.input, input);
  }

  void clear_demodulate_entries() { demodulate_entries.clear(); }

  const ofdm_demodulator_configuration& get_configuration() const { return configuration; }

  const std::vector<demodulate_entry>& get_demodulate_entries() const { return demodulate_entries; }

private:
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> dist;
  ofdm_demodulator_configuration        configuration;
  std::vector<demodulate_entry>         demodulate_entries;
};

class ofdm_demodulator_factory_spy : public ofdm_demodulator_factory
{
public:
  std::unique_ptr<ofdm_symbol_demodulator>
  create_ofdm_symbol_demodulator(const ofdm_demodulator_configuration& config) override
  {
    std::unique_ptr<ofdm_symbol_demodulator_spy> ptr = std::make_unique<ofdm_symbol_demodulator_spy>(config);
    demodulators.push_back(ptr.get());
    return ptr;
  }
  std::unique_ptr<ofdm_slot_demodulator>
  create_ofdm_slot_demodulator(const ofdm_demodulator_configuration& config) override
  {
    return nullptr;
  }

  std::vector<ofdm_symbol_demodulator_spy*>& get_demodulators() { return demodulators; };

private:
  std::vector<ofdm_symbol_demodulator_spy*> demodulators;
};

} // namespace srsran
