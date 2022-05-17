/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdsch_modulator_test_data.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsgnb {
struct pdsch_modulator_config_t {
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;
};

std::unique_ptr<pdsch_modulator> create_pdsch_modulator(pdsch_modulator_config_t& config);

} // namespace srsgnb

using namespace srsgnb;

int main()
{
  // Create PDSCH modulator configuration.
  pdsch_modulator_config_t config = {create_modulation_mapper(), create_pseudo_random()};

  // Create PDSCH modulator.
  std::unique_ptr<pdsch_modulator> pdsch = create_pdsch_modulator(config);

  for (const test_case_t& test_case : pdsch_modulator_test_data) {
    // Create resource grid spy.
    resource_grid_writer_spy grid("a");

    // Read codeword.
    std::vector<uint8_t> data = test_case.data.read();

    // Prepare codewords.
    static_vector<span<const uint8_t>, pdsch_modulator::MAX_NOF_CODEWORDS> codewords(1);
    codewords[0] = data;

    // Modulate.
    pdsch->modulate(grid, codewords, test_case.config);

    // Read resource grid data.
    std::vector<resource_grid_writer_spy::expected_entry_t> rg_entries = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(rg_entries);
  }

  return 0;
}