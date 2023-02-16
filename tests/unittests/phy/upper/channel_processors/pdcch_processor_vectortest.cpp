/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_processor_test_data.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_formatters.h"

using namespace srsgnb;

int main()
{
  std::shared_ptr<crc_calculator_factory> crc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_factory);

  std::shared_ptr<polar_factory> polar_factory = create_polar_factory_sw();
  TESTASSERT(polar_factory);

  std::shared_ptr<pdcch_encoder_factory> encoder_factory = create_pdcch_encoder_factory_sw(crc_factory, polar_factory);
  TESTASSERT(encoder_factory);

  std::shared_ptr<channel_modulation_factory> mod_factory = create_channel_modulation_sw_factory();
  TESTASSERT(mod_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<pdcch_modulator_factory> modulator_factory =
      create_pdcch_modulator_factory_sw(mod_factory, prg_factory);
  TESTASSERT(modulator_factory);

  std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_pdcch_factory =
      create_dmrs_pdcch_processor_factory_sw(prg_factory);
  TESTASSERT(dmrs_pdcch_factory);

  std::shared_ptr<pdcch_processor_factory> factory =
      create_pdcch_processor_factory_sw(encoder_factory, modulator_factory, dmrs_pdcch_factory);
  TESTASSERT(factory);

  std::unique_ptr<pdcch_processor> processor = factory->create();
  TESTASSERT(processor);

  for (const test_case_t& test_case : pdcch_processor_test_data) {
    unsigned max_prb  = MAX_RB;
    unsigned max_symb = test_case.config.coreset.start_symbol_index + test_case.config.coreset.duration;

    // Create resource grid spy.
    resource_grid_writer_spy grid(MAX_PORTS, max_symb, max_prb);

    // Process.
    processor->process(grid, test_case.config);

    // Load output golden data
    const std::vector<resource_grid_writer_spy::expected_entry_t> expected = test_case.data.read();

    // Assert resource grid entries.
    grid.assert_entries(expected);
  }

  return 0;
}
