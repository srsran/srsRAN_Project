

#include "pdcch_modulator_test_data.h"
#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsgnb {
struct pdcch_modulator_config_t {
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;
};

std::unique_ptr<pdcch_modulator> create_pdcch_modulator(pdcch_modulator_config_t& config);

} // namespace srsgnb

using namespace srsgnb;

int main()
{
  // Create PDCCH modulator configuration.
  pdcch_modulator_config_t config = {create_modulation_mapper(), create_pseudo_random()};

  // Create PDCCH modulator.
  std::unique_ptr<pdcch_modulator> pdcch = create_pdcch_modulator(config);

  for (const test_case_t& test_case : pdcch_modulator_test_data) {
    // Create resource grid spy.
    resource_grid_writer_spy grid;

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
