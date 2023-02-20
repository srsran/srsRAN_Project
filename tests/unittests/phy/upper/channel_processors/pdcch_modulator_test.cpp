/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_modulator_test_data.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"

using namespace srsran;

int main()
{
  std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
  TESTASSERT(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<pdcch_modulator_factory> pdcch_factory =
      create_pdcch_modulator_factory_sw(modulator_factory, prg_factory);
  TESTASSERT(modulator_factory);

  std::unique_ptr<pdcch_modulator> pdcch = pdcch_factory->create();
  TESTASSERT(pdcch);

  for (const test_case_t& test_case : pdcch_modulator_test_data) {
    int prb_idx_high = test_case.config.rb_mask.find_highest();
    TESTASSERT(prb_idx_high > 1);
    unsigned max_prb  = static_cast<unsigned>(prb_idx_high + 1);
    unsigned max_symb = test_case.config.start_symbol_index + test_case.config.duration;

    // Create resource grid spy.
    resource_grid_writer_spy grid(MAX_PORTS, max_symb, max_prb);

    // Load input codeword from a testvector
    const std::vector<uint8_t> test_codeword = test_case.data.read();

    // Modulate.
    pdcch->modulate(grid, test_codeword, test_case.config);

    // Load output golden data
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }

  return 0;
}
