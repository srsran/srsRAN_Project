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
#include "dmrs_pbch_map_test_data.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pbch_processor.h"
#include <cassert>
#include <random>

using namespace srsgnb;

int main()
{
  // Create DMRS-PBCH processor
  std::unique_ptr<dmrs_pbch_processor> dmrs_pbch = create_dmrs_pbch_processor();

  for (const test_case_t& test_case : dmrs_pbch_processor_test_data) {
    // Create resource grid
    resource_grid_writer_spy grid;

    // Map DMRS-PBCH using the test case arguments
    dmrs_pbch->map(grid, test_case.args);

    // Assert resource grid entries.
    grid.assert_put_entries(test_case.symbols);
  }

  return 0;
}