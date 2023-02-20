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

#include "srsgnb/phy/lower/modulation/modulation_factories.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

namespace srsran {

class ofdm_symbol_demodulator_spy : public ofdm_symbol_demodulator
{
private:
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> dist;
  ofdm_demodulator_configuration        configuration;

  struct demodulate_entry {
    std::vector<cf_t>           input;
    const resource_grid_writer* grid;
    unsigned                    port_index;
    unsigned                    symbol_index;
  };
  std::vector<demodulate_entry> demodulate_entries;

public:
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

  const std::vector<demodulate_entry>& get_demodulate_entries() const { return demodulate_entries; }
};

class ofdm_demodulator_factory_spy : public ofdm_demodulator_factory
{
private:
  std::vector<ofdm_symbol_demodulator_spy*> demodulators;

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
};

} // namespace srsran
