/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dmrs_pbch_processor_test_data.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"

using namespace srsgnb;

int main()
{
  // Create DMRS-PBCH processor
  std::unique_ptr<dmrs_pbch_processor> dmrs_pbch = create_dmrs_pbch_processor();

  for (const test_case_t& test_case : dmrs_pbch_processor_test_data) {
    // Create resource grid
    resource_grid_writer_spy grid;

    // Map DMRS-PBCH using the test case arguments
    dmrs_pbch->map(grid, test_case.config);

    // Load output golden data
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }

  return 0;
}
