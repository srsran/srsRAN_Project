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

#include "../resource_grid_test_doubles.h"
#include "dmrs_pbch_map_test_data.h"
#include "srsgnb/phy/signal_processors/dmrs_pbch_processor.h"
#include <cassert>
#include <random>

using namespace srsgnb;

static const float assert_max_error = 1e-6;

int main()
{
  // Create DMRS-PBCH processor
  std::unique_ptr<dmrs_pbch_processor> dmrs_pbch = create_dmrs_pbch_processor();

  for (const test_case_t& test_case : pbch_encode_test_data) {
    // Create resource grid
    resource_grid_spy grid;

    // Map DMRS-PBCH using the test case arguments
    dmrs_pbch->map(grid, test_case.args);

    // Assert number of generated RE
    assert(test_case.signal.size() == grid.get_nof_entries());

    // Assert grid coordinates and value
    const std::vector<resource_grid_spy::entry_t> grid_entries = grid.get_entries();
    for (unsigned i = 0; i != grid_entries.size(); ++i) {
      assert(grid_entries[i].l == test_case.signal[i].l);
      assert(grid_entries[i].k == test_case.signal[i].k);

      float err = std::abs(grid_entries[i].value - test_case.signal[i].value);
      assert(err < assert_max_error);
    }
  }

  return 0;
}