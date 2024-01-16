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

#include "../../support/resource_grid_mapper_test_doubles.h"
#include "dmrs_pdsch_processor_test_data.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"

using namespace srsran;

int main()
{
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_processor_factory =
      create_dmrs_pdsch_processor_factory_sw(prg_factory);
  TESTASSERT(dmrs_processor_factory);

  // Create DMRS-PDSCH processor.
  std::unique_ptr<dmrs_pdsch_processor> dmrs_pdsch = dmrs_processor_factory->create();
  TESTASSERT(dmrs_pdsch);

  for (const test_case_t& test_case : dmrs_pdsch_processor_test_data) {
    int prb_idx_high = test_case.config.rb_mask.find_highest();
    TESTASSERT(prb_idx_high > 1);
    unsigned max_prb   = static_cast<unsigned>(prb_idx_high + 1);
    unsigned max_symb  = get_nsymb_per_slot(cyclic_prefix::NORMAL);
    unsigned max_ports = test_case.config.precoding.get_nof_ports();

    // Prepare resource grid and resource grid mapper spies.
    resource_grid_writer_spy              grid(max_ports, max_symb, max_prb);
    std::unique_ptr<resource_grid_mapper> mapper = create_resource_grid_mapper(max_ports, NRE * max_prb, grid);

    // Map DMRS-PDSCH using the test case arguments.
    dmrs_pdsch->map(*mapper, test_case.config);

    // Load output golden data.
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }

  return 0;
}
