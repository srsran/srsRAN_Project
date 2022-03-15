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
#include "dmrs_pdsch_processor_test_data.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include <random>

using namespace srsgnb;

int main()
{
  // Create DMRS-PDSCH processor.
  std::unique_ptr<dmrs_pdsch_processor> dmrs_pdsch = create_dmrs_pdsch_processor();

  for (const test_case_t& test_case : dmrs_pdsch_processor_test_data) {
    // Create resource grid.
    resource_grid_writer_spy grid;

    // Map DMRS-PDSCH using the test case arguments.
    dmrs_pdsch->map(grid, test_case.config);

    // Assert resource grid entries.
    grid.assert_entries(test_case.symbols);
  }

  return 0;
}
