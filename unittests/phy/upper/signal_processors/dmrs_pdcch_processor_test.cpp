/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../resource_grid_test_doubles.h"
#include "dmrs_pdcch_processor_test_data.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdcch_processor.h"
#include <random>

using namespace srsgnb;

int main()
{
  // Create DMRS-PDSCH processor.
  std::unique_ptr<dmrs_pdcch_processor> dmrs_pdcch = create_dmrs_pdcch_processor();
  
  for (const test_case_t& test_case : dmrs_pdcch_processor_test_data) {
    // Create resource grid.
    resource_grid_writer_spy grid;

    // Map DMRS-PDCCH using the test case arguments.
    dmrs_pdcch->map(grid, test_case.config);
    
    // Load output golden data
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }

  return 0;
}
