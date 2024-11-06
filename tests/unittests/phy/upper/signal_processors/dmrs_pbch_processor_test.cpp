/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "dmrs_pbch_processor_test_data.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"

using namespace srsran;

int main()
{
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<dmrs_pbch_processor_factory> dmrs_pbch_factory = create_dmrs_pbch_processor_factory_sw(prg_factory);
  TESTASSERT(dmrs_pbch_factory);

  // Create DMRS-PBCH processor
  std::unique_ptr<dmrs_pbch_processor> dmrs_pbch = dmrs_pbch_factory->create();
  TESTASSERT(dmrs_pbch);

  for (const test_case_t& test_case : dmrs_pbch_processor_test_data) {
    // Create resource grid
    resource_grid_writer_spy grid(test_case.config.ports.size(), SSB_DURATION_NSYMB, SSB_BW_RB);

    // Map DMRS-PBCH using the test case arguments
    dmrs_pbch->map(grid, test_case.config);

    // Load output golden data
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }

  return 0;
}
