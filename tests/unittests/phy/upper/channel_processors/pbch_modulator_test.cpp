/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pbch_modulator_test_data.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"

using namespace srsran;

int main()
{
  std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
  TESTASSERT(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<pbch_modulator_factory> pbch_factory =
      create_pbch_modulator_factory_sw(modulator_factory, prg_factory);
  TESTASSERT(modulator_factory);

  std::unique_ptr<pbch_modulator> modulator = pbch_factory->create();
  TESTASSERT(modulator);

  for (const test_case_t& test_case : pbch_modulator_test_data) {
    resource_grid_writer_spy grid(test_case.config.ports.size(), SSB_DURATION_NSYMB, SSB_BW_RB);

    // Load input data
    const std::vector<uint8_t> testvector_data = test_case.data.read();

    // Run the entity process
    modulator->put(testvector_data, grid, test_case.config);

    // Load output golden data
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }
  return 0;
}
