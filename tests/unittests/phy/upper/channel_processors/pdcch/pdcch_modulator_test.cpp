/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "pdcch_modulator_test_data.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/pdcch/factories.h"

using namespace srsran;

int main()
{
  std::shared_ptr<modulation_mapper_factory> modulator_factory = create_modulation_mapper_factory();
  TESTASSERT(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
  TESTASSERT(precoding_factory);

  std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory =
      create_resource_grid_mapper_factory(precoding_factory);
  TESTASSERT(rg_mapper_factory);

  std::shared_ptr<pdcch_modulator_factory> pdcch_factory =
      create_pdcch_modulator_factory_sw(modulator_factory, prg_factory, rg_mapper_factory);
  TESTASSERT(modulator_factory);

  std::unique_ptr<pdcch_modulator> pdcch = pdcch_factory->create();
  TESTASSERT(pdcch);

  for (const test_case_t& test_case : pdcch_modulator_test_data) {
    TESTASSERT(test_case.config.rb_mask.count() > 1);
    int      prb_idx_high = test_case.config.rb_mask.find_highest();
    unsigned max_prb      = static_cast<unsigned>(prb_idx_high + 1);
    unsigned max_symb     = test_case.config.start_symbol_index + test_case.config.duration;
    unsigned max_ports    = test_case.config.precoding.get_nof_ports();

    // Prepare resource grid and resource grid mapper spies.
    resource_grid_writer_spy              grid(max_ports, max_symb, max_prb);
    std::unique_ptr<resource_grid_mapper> mapper = rg_mapper_factory->create();

    // Load input codeword from a testvector
    const std::vector<uint8_t> test_codeword = test_case.data.read();

    // Modulate.
    pdcch->modulate(grid, test_codeword, test_case.config);

    // Load output golden data
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }

  return 0;
}
