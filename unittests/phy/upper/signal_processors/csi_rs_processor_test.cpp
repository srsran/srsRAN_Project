/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../resource_grid_test_doubles.h"
#include "csi_rs_processor_test_data.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"

using namespace srsgnb;

int main()
{
  // Create CSI-RS processor.
  std::unique_ptr<csi_rs_processor> csi_rs = create_csi_rs_processor();

  for (const test_case_t& test_case : csi_rs_processor_test_data) {
    // Create resource grid.
    resource_grid_writer_spy grid(MAX_PORTS, MAX_NSYMB_PER_SLOT, test_case.config.start_rb + test_case.config.nof_rb);

    // Map NZP-CSI-RS using the test case arguments.
    csi_rs->map(grid, test_case.config);

    // Load output golden data.
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }
}
