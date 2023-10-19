/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "pdsch_modulator_test_data.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/srsvec/bit.h"

using namespace srsran;

int main()
{
  std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
  TESTASSERT(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<pdsch_modulator_factory> pdsch_factory =
      create_pdsch_modulator_factory_sw(modulator_factory, prg_factory);
  TESTASSERT(modulator_factory);

  std::unique_ptr<pdsch_modulator> pdsch = pdsch_factory->create();
  TESTASSERT(pdsch);

  for (const test_case_t& test_case : pdsch_modulator_test_data) {
    bounded_bitset<MAX_RB> prb_mask =
        test_case.config.freq_allocation.get_prb_mask(test_case.config.bwp_start_rb, test_case.config.bwp_size_rb);
    int prb_idx_high = prb_mask.find_highest();
    TESTASSERT(prb_idx_high > 1);
    unsigned max_prb   = static_cast<unsigned>(prb_idx_high + 1);
    unsigned max_symb  = get_nsymb_per_slot(cyclic_prefix::NORMAL);
    unsigned max_ports = test_case.config.precoding.get_nof_ports();

    // Prepare resource grid and resource grid mapper spies.
    resource_grid_writer_spy              grid(max_ports, max_symb, max_prb);
    std::unique_ptr<resource_grid_mapper> mapper = create_resource_grid_mapper(max_ports, NRE * max_prb, grid);

    // Read codeword.
    std::vector<uint8_t> data = test_case.data.read();

    // Pack data.
    dynamic_bit_buffer packed_data(data.size());
    srsvec::bit_pack(packed_data, data);

    // Prepare codewords.
    static_vector<bit_buffer, pdsch_constants::MAX_NOF_CODEWORDS> codewords;
    codewords.emplace_back(packed_data);

    // Modulate.
    pdsch->modulate(*mapper, codewords, test_case.config);

    // Read resource grid data.
    std::vector<resource_grid_writer_spy::expected_entry_t> rg_entries = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(rg_entries);
  }

  return 0;
}