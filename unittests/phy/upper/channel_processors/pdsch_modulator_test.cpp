/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdsch_modulator_test_data.h"
#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/sequence_generators/sequence_generator_factories.h"

using namespace srsgnb;

int main()
{
  std::shared_ptr<modulation_mapper_factory> modulator_factory = create_modulation_mapper_sw_factory();
  TESTASSERT(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<pdsch_modulator_factory> pdsch_factory =
      create_pdsch_modulator_factory_sw(modulator_factory, prg_factory);
  TESTASSERT(modulator_factory);

  std::unique_ptr<pdsch_modulator> pdsch = pdsch_factory->create();
  TESTASSERT(pdsch);

  for (const test_case_t& test_case : pdsch_modulator_test_data) {
    // Create resource grid spy.
    resource_grid_writer_spy grid("a");

    // Read codeword.
    std::vector<uint8_t> data = test_case.data.read();

    // Prepare codewords.
    static_vector<span<const uint8_t>, pdsch_modulator::MAX_NOF_CODEWORDS> codewords(1);
    codewords[0] = data;

    // Modulate.
    pdsch->modulate(grid, codewords, test_case.config);

    // Read resource grid data.
    std::vector<resource_grid_writer_spy::expected_entry_t> rg_entries = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(rg_entries);
  }

  return 0;
}