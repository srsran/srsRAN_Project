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

static const float assert_max_error = 1e-6;

int main()
{
  // Create DMRS-PDSCH processor
  std::unique_ptr<dmrs_pdsch_processor> dmrs_pdsch = create_dmrs_pdsch_processor();

  for (const test_case_t& test_case : dmrs_pdsch_processor_test_data) {
    // Create resource grid
    resource_grid_spy grid;

    // Map DMRS-PDSCH using the test case arguments
    dmrs_pdsch->map(grid, test_case.config);

    // Assert number of generated RE
    srsran_assert(test_case.symbols.size() == grid.get_nof_put_entries(),
                  "Unmatched number of entries %d vs %d",
                  (int)test_case.symbols.size(),
                  grid.get_nof_put_entries());

    // Assert grid coordinates and value
    const std::vector<resource_grid_spy::entry_t> grid_entries = grid.get_put_entries();
    for (unsigned i = 0; i != grid_entries.size(); ++i) {
      srsran_assert(grid_entries[i].port == test_case.symbols[i].port,
                    "Unmatched port index %d vs %d",
                    grid_entries[i].port,
                    test_case.symbols[i].port);
      srsran_assert(grid_entries[i].l == test_case.symbols[i].l,
                    "Unmatched symbol index %d vs %d",
                    grid_entries[i].l,
                    test_case.symbols[i].l);
      srsran_assert(grid_entries[i].k == test_case.symbols[i].k,
                    "Unmatched subcarrier index %d vs %d",
                    grid_entries[i].k,
                    test_case.symbols[i].k);

      float err = std::abs(grid_entries[i].value - test_case.symbols[i].value);
      srsran_assert(err < assert_max_error,
                    "Unmatched value %+.3f%+.3f vs %+.3f%+.3f for i=%d",
                    grid_entries[i].value.real(),
                    grid_entries[i].value.imag(),
                    test_case.symbols[i].value.real(),
                    test_case.symbols[i].value.imag(),
                    i);
    }
  }

  return 0;
}
