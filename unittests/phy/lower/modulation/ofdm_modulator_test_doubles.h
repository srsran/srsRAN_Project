/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_MODULATOR_TEST_DOUBLES_H
#define SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_MODULATOR_TEST_DOUBLES_H

#include "srsgnb/phy/lower/modulation/modulation_factories.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

namespace srsgnb {

class ofdm_symbol_modulator_spy : public ofdm_symbol_modulator
{
private:
  srslog::basic_logger&                 logger;
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
  ofdm_symbol_modulator_spy(srslog::basic_logger& logger_, const ofdm_modulator_configuration& config) :
    logger(logger_), rgen(0), dist(-1, +1), configuration(config)
  {
    // Do nothing.
  }

  unsigned get_symbol_size(unsigned symbol_index) const override
  {
    return configuration.cp.get_length(symbol_index, configuration.numerology, configuration.dft_size) +
           configuration.dft_size;
  }

  void
  modulate(span<cf_t> output, const resource_grid_reader& grid, unsigned port_index, unsigned symbol_index) override
  {
    logger.debug("Modulate - symbol_size={}, port_index={}, symbol_index={}", output.size(), port_index, symbol_index);

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

  const std::vector<modulate_entry>& get_modulate_entries() const { return modulate_entries; }
};

class ofdm_modulator_factory_spy : public ofdm_modulator_factory
{
private:
  unsigned                                count;
  std::string                             log_level;
  std::vector<ofdm_symbol_modulator_spy*> modulators;

public:
  ofdm_modulator_factory_spy(std::string log_level_) : count(0), log_level(log_level_) {}

  std::unique_ptr<ofdm_symbol_modulator>
  create_ofdm_symbol_modulator(const ofdm_modulator_configuration& config) override
  {
    srslog::basic_logger& logger = srslog::fetch_basic_logger("Modulator " + std::to_string(count++), false);
    logger.set_level(srslog::str_to_basic_level(log_level));
    std::unique_ptr<ofdm_symbol_modulator_spy> ptr = std::make_unique<ofdm_symbol_modulator_spy>(logger, config);
    modulators.push_back(ptr.get());
    return ptr;
  }
  std::unique_ptr<ofdm_slot_modulator> create_ofdm_slot_modulator(const ofdm_modulator_configuration& config) override
  {
    return nullptr;
  }

  std::vector<ofdm_symbol_modulator_spy*>& get_modulators() { return modulators; };
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_MODULATOR_TEST_DOUBLES_H
